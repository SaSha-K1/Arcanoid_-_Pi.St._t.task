
#include "obj.h"            //call of Copy C-tor in AddBrick()
#include "scene.h"          //call of ->AddAni()
#include "controller.h" ///@@@try to get rid of this include. BUT it's included in .h file
#include "bricksManager.h"
#include "brick.h"          //call of C-tor in AddBrick()
#include "myExcptn.h"



//// C-tor ////
_BRICKS_MNGR::CBricksManager(const u32 numOfBricksOnLvl)
    :m_uNUM_OF_BRICKS_ON_LVL(numOfBricksOnLvl)
    //#OUTDATED:
    //, papBricksAniObjects(nullptr)
{

}


//// AddBrick() ////
void _BRICKS_MNGR::AddBrick(
        /**/const/**/ CAniObject* const baseBrick, 
        const vector2           position, 
        CScene* const           sc  //@@ вначале нет `const`, ибо есть вызов неконст.метода AddAni()
    )
{
    std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(3);
    if (!(*pLavHndlr)->empty())	//#NOTE: опер-ры `*` и `->` перегружены д/ _LAV_HANDLER    //Если это первый кирпич, то значит не создаём копии аниобъекта, а берём уже готовый, который загружен из xml
    {
        (*pLavHndlr)->push_back(new _BRICK(new CAniObject(*baseBrick), position));
        sc->AddAni((*pLavHndlr)->back()->GetBrickAniObj()/**/, 1/**/);	//Добавить ани-объект в сцену 
        ///@@@ Тут^ последний атрибут iscopy - разобраться, что это. Юлия написала вызывать метод с пар-ром "0"
    }
    else
        (*pLavHndlr)->push_back(new _BRICK(baseBrick, position));   ///@@@ а 1й кирпич чего в сцену не добавляем? он там уже изначально прямиком из xml? или мы его уже не вводили в сцену?? (90%,что 1й вар.)
}



//// DelBrick() ////
///@@@ #TODO: оч.длинный метод - попробовать раздробить
void _BRICKS_MNGR::DelBrick(
            const u32                       i, 
            CScene* const                   sc,
            CArkanoidController* const      pArkCntrllr,
            std::auto_ptr<_LAV_HANDLER>*    ppLavHndlr /*= nullptr*/     //Закомментировано знач-е by def., установленное в объявлении метода.     //@@нет ни одного `const`'а, т.к. и указатель переопределяю и вызываю неконст.методы для разыменованного ptr'а.
        )   ///@@@ #UGLY: подумать - быть может заменить shared_ptr, чтобы не городить это ужасное тройное разыменование. (пока auto_ptr, вынужден работать ч/з ptr на этот auto_ptr, т.к. ссылку, инициировав `nullptr`, потом не смогу использовать, а если передать в ф-цию auto_ptr по значению, то уничтожится исходный объект. В крайнем случ. перейти может всё-таки на ссылку: код будет понятнее, НО придётся объявлять в методе ещё одну ссылку, в которую копировать знач-е ссылки-параметра, если не 0)
{
//#OUTDATED (блокировка сейчас вшита, над необх-тью LOW_LVL_LOCKS подумать позже):
//#if LOCKS==1
//    //Костыли: перенёс сюда от немного снизу
//    while (CBrick::BricksLockOn(2) != 1)	//включаю замок, чтобы никто не пользовался пока итераторами и индексами
//    {
//
//#if LOW_LVL_LOCKS==1
//        //if ( CBrick::getBricksLockVal() == 2) break;	//Костыли: добавил эту ветку строку условия
//        if ( CBrick::BricksLockOn(2) != 1) 
//            return;
//        else
//            break;
//#endif //LOW_LVL_LOCKS
//
//    }
//#endif //LOCKS
    if (nullptr == ppLavHndlr)
//        std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(4);
        ppLavHndlr = &(m_LavBricks.CreateAccessHandler(4));

#if DEBUG==1
    if ( (*(*ppLavHndlr))->size() <= i ) throw CMyExcptn(14);   //#NOTE: опер-ры `*` и `->` перегружены д/ _LAV_HANDLER
#endif

    if ( _BRICK::DSTR_ST == (*(*(*ppLavHndlr)))[i]->m_state )   ///@@@может тут можно так: "/*_BRICK::*/DSTR_ST ..."
    //#NOTE: 
    //тут   1й (*..) - от ptr'а на `auto_ptr` переходим к `auto_ptr`
    //      2й (*..) - от `auto_ptr` переходим к _LAV_HANDLER
    //      3й (*..) - т.к. в _LAV_HANDLER опер-ры `*` и `->` перегружены, то тут от _LAV_HANDLER переходим сразу к std::vector<_BRICK*> ..
    //.. ptr, на который есть member data класса.
    //      ну и [i] - уже обращаеся к эл-ту вектора типа _BRICK*
    ///.. конечно, это жесть!
    {
        (*(*(*ppLavHndlr)))[i]->GetBrickAniObj()->StartMovement(MV_SC01_BRICK);
        (*(*(*ppLavHndlr)))[i]->m_state = _BRICK::DSTR_EN;

//#OUTDATED (блокировка сейчас вшита, throw excptn if != reserved key, вшито в D-tor):
//        //Костыли: тут это имеет смысл только пока BricksLockOn() переехал выше этого места. Когда вернётся, убрать это.
//#if LOCKS==1
//#if DEBUG==1
//        if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//        CBrick::BricksLockOff(2)
//#endif //DEBUG
//#endif //LOCKS

        return;
    }
    if ( _BRICK::DSTR_EN == (*(*(*ppLavHndlr)))[i]->m_state  &&  (*(*(*ppLavHndlr)))[i]->GetBrickAniObj()->IsStopped() )
    {
        (*(*(*ppLavHndlr)))[i]->m_state = _BRICK::OFF;	/*Костылю*/
//#if LOCKS==1
//      while (CBrick::BricksLockOn() != 1)	//включаю замок, чтобы никто не пользовался пока итераторами и индексами
//      {
//      }
//#endif

#if DONT_DELETE_ANI_BRICKS==0
        //Если это не Базовый кирпич
        if (i!=0)
        {
            sc->RemoveAni((*(*(*ppLavHndlr)))[i]->GetBrickAniObj());	//Удалить ани-объект из сцены
            delete (*(*(*ppLavHndlr)))[i]->GetBrickAniObj();
        }

        //Если это первый (базовый) кирпич, то обрабатываю отдельно - первые 2 строки для него выполнять нельзя, (т.к. остальные его копии - будут ли нормально работать после его удаления) Да и создавал его не я
        else
#endif //DONT_DELETE_ANI_BRICKS
        {
            (*(*(*ppLavHndlr)))[i]->GetBrickAniObj()->SetPos(-300,-300); //убираю из игрового поля
            (*(*(*ppLavHndlr)))[i]->GetBrickAniObj()->SetStatic(/*ANI_SC01_BRICK*/ST_SC01_BRICK_BEGIN);     //@@? Тут я вроде внешний вид от схлопнувшегося меняю на нормальный
            (*(*(*ppLavHndlr)))[i]->m_state = _BRICK::OFF;
        }
		
        //Дальше код универсален для любого кирпича
        delete (*(*(*ppLavHndlr)))[i];

        (*(*ppLavHndlr))->erase((*(*ppLavHndlr))->begin()+i);	//Костыли - пробовал вместо этой строки обменять перед удалением с последним или 1ым эл-том - проблема не ушла
        ////Костыли: перед удалением меняю с последним
        //if (m_vpBricks.size()>1)
        //{
        //  //CBrick *tmpB = *(m_vpBricks.begin()+i);
        //  //*(m_vpBricks.begin()+i)=*(m_vpBricks.end()-1);
        //  //*(m_vpBricks.end()-1) = tmpB;
        //  //m_vpBricks.erase(m_vpBricks.end()-1);
        //  CBrick *tmpB = *(m_vpBricks.begin()+i);
        //  *(m_vpBricks.begin()+i)=*(m_vpBricks.begin());
        //  *(m_vpBricks.begin()) = tmpB;
        //  m_vpBricks.erase(m_vpBricks.begin());
        //}

        pArkCntrllr->m_uGameScore +=10;

        if ((*(*ppLavHndlr))->empty()  &&  pArkCntrllr->m_GameState==RUN)
        {
            pArkCntrllr->m_GameState=WON;
        }
//#if LOCKS==1
//#if DEBUG==1
//      if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//      CBrick::BricksLockOff(2)
//#endif
//#endif
    }

//#OUTDATED (блокировка сейчас вшита, throw excptn if != reserved key, вшито в D-tor):
////Костыли: перенёс под скобку
//#if LOCKS==1
//#if DEBUG==1
//        if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//        CBrick::BricksLockOff(2)
//#endif //DEBUG
//#endif //LOCKS

}


//// CleanBricks() ////
void  _BRICKS_MNGR::CleanBricks(
        CScene* const                   sc,
        CArkanoidController* const      pArkCntrllr
    )
{
    //@@#BACKUP (if где-то применяю GetLavHndlr(), то и тут заменить строку этой -->):  std::auto_ptr<_LAV_HANDLER> pLavHndlr = GetLavHndlr(7);
    std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(7);
    for (char i=(*pLavHndlr)->size()-1;  i>=0;  --i)
    ///@@@ #TODO: ввести статический флаг "Что-то изменилось с кирпичами", который вкл-ть когда случилось касание или отыгралась анимация, чтобы проверку эту по всем кирпичам зря не гонять.
    ///@@@ #TODO: кроме этого в кач-ве этого флага быть может передавать индекс кирпича, который нужно обрабоать - тогда избавимся тут от перебора.     //может частично использовать для этих целей паттерн Observer
    
	if ( (*(*pLavHndlr))[i]->m_state == /*_BRICK*/CBrick::DSTR_ST || (*(*pLavHndlr))[i]->m_state == _BRICK/*CBrick*/::DSTR_EN )
		DelBrick(i, sc, pArkCntrllr, &pLavHndlr);
}


//// RetrieveBrick() ////
void  _BRICKS_MNGR::RetrieveBrick(/*std::vector<*/ const std::vector<vector2>* /*>*/ const  /*vpvBricksOnLvlsCoords*/pvBricksOnCurrLvlCoords)
{
    std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(1);
#if DEBUG==1
    if (nullptr == pLavHndlr.get()) throw CMyExcptn(21);
#endif //DEBUG
    for (u32 i = 0;  i < m_vpBricksAniObjs.size();  ++i)
        (*pLavHndlr)->push_back( /**/(/**/ new /*CBricksManager::CBrick*/_BRICK(m_vpBricksAniObjs[i] /**/)/**/, /*(*vpvBricksOnLvlsCoords[0])*/(*pvBricksOnCurrLvlCoords)[i]));   ///@@@ храним уже ptr на CLavHandler. Поэтому ВОЗМОЖНО переписать m_vpBricks.push_back(... через опер-р -> вместо опер-ра точки.
}   ///@@@ Попробовать убрать скобки () вокруг `new`. Вроде они не нужны, но КОМП-р ругается //Справочно: oper-r `new` и `delete` имеют > приоритет, чем опер-р "," (но тут "," - separator, а не oper-r) 


//// GetLavHndlr() ////
std::auto_ptr<_LAV_HANDLER>  _BRICKS_MNGR::GetLavHndlr(const u32 key)
{
    return m_LavBricks.CreateAccessHandler(key);
}


#if DONT_DELETE_ANI_BRICKS==1
//// InitOfBricksAniObjects() ////
void _BRICKS_MNGR::InitOfBricksAniObjects()
{	
    //#OUTDATED: т.к. юзаю .push_back() vector'а, и, поэтому, общий индекс для m_vpBricks и m_vpBricksAniObjs уже не нужен, перехожу на итераторы
    //for (char i = 0; i < m_vpBricks.size(); ++i)
    //{
    //    m_vpBricksAniObjs[i] = m_vpBricks[i]->GetBrickAniObj();
    //}
    std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(5);
    for (auto& it = (*pLavHndlr)->begin(); it != (*pLavHndlr)->end(); ++it)
    {
        m_vpBricksAniObjs.push_back( (*it)->GetBrickAniObj() );     ///@@@тут видимо не совсем так, как хотелось бы отрабатывает перегруженный опер-р `->`. Возможно добавить ещё одну скобку с разыменованием
    }
}
#endif //DONT_DELETE_ANI_BRICKS


//// StartAddBricks() ////
void _BRICKS_MNGR::StartAddBricks(
        CScene*                 sc, 
        std::vector<vector2>*   pvBricksOnLvlCoords
    )
{
        AddBrick(sc->FindAniObject(ANI_SC01_BRICK), (*pvBricksOnLvlCoords)[0], sc);
        //#OUTDATED: переделал цикл под итераторы
        //for (u32 ibr=1; ibr < m_uNUM_OF_BRICKS_ON_LVL1; ++ibr)	//frm "1", т.к. 0й кирпич внесли отдельно - он базовый
        //{
        //    addBrick(m_vpBricks[0]->getBrickAniObj(), m_aBricksCoordsLvl1[ibr], sc);
        //}
        for (auto it = pvBricksOnLvlCoords->begin()+1;  it != pvBricksOnLvlCoords->end(); ++it)	//frm "1", т.к. 0й кирпич внесли отдельно - он базовый  //ITERATOR using
        {   ///@@@ Тут^^^ маг.число [0]. Избавиться!
            AddBrick(/*m_pBricksMngr->m_vpBricks[0]->GetBrickAniObj()*/sc->FindAniObject(ANI_SC01_BRICK),  *it,  sc);    ///@@@ #TODO: разобраться, какого ругается на "ptr на неполный тип" КОМ-р, с 1ым пар-ром `m_pBricksMngr->...`
        }
        /*после отладки дописать ещё кубики до общ.кол-ва 12 и исправить соотв. константу*/
        //вообще наверное тут лучше научиться работать с xml и загружать из него всю переменную информацию по уровню.
}


///@@@ At the end see list "#TODO" in file "PipeStudio Test task refactoring list.txt"