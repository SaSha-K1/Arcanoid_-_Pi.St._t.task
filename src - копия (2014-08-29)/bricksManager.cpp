
#include "bricksManager.h"
#include "brick.h"
#include "myExcptn.h"
#include "obj.h"
#include "controller.h"



//// C-tor ////
CArkanoidController::CBricksManager::CBricksManager(const u32 numOfBricksOnLvl)
    :m_uNUM_OF_BRICKS_ON_LVL(numOfBricksOnLvl)
    //#OUTDATED:
    //, papBricksAniObjects(nullptr)
{

}



//// AddBrick() ////
void CArkanoidController::CBricksManager::AddBrick(CAniObject* baseBrick, vector2 position, CScene* sc)
{
    if (!m_vpBricks.empty())	//если это первый кирпич, то значит не создаём копии аниобъекта, а берём уже готовый, который загружен из xml
    {
        m_vpBricks.push_back(new CBrick(new CAniObject(*baseBrick), position));
        sc->AddAni(m_vpBricks./*front()*/back()->GetBrickAniObj()/**/, 1/**/);	//Добавить ани-объект в сцену 
        ///@@@ Тут^ последний атрибут iscopy - разобраться, что это
    }
    else
        m_vpBricks.push_back(new CBrick(baseBrick, position));
}



//// DelBrick() ////
///@@@ #TODO: оч.длинный метод - попробовать раздробить
void CArkanoidController::CBricksManager::DelBrick(u32 i, CScene* sc)
{

#if LOCKS==1
    //Костыли: перенёс сюда от немного снизу
    while (CBrick::BricksLockOn(2) != 1)	//включаю замок, чтобы никто не пользовался пока итераторами и индексами
    {

#if LOW_LVL_LOCKS==1
        //if ( CBrick::getBricksLockVal() == 2) break;	//Костыли: добавил эту ветку строку условия
        if ( CBrick::BricksLockOn(2) != 1) 
            return;
        else
            break;
#endif//LOW_LVL_LOCKS

    }
#endif//LOCKS

#if DEBUG==1
    if (m_vpBricks.size()<=i) throw CMyExcptn(14);
#endif

    if (CBrick::DSTR_ST == m_vpBricks[i]->m_state)
    {
        m_vpBricks[i]->GetBrickAniObj()->StartMovement(MV_SC01_BRICK);
        m_vpBricks[i]->m_state = CBrick::DSTR_EN;


        //Костыли: тут это имеет смысл только пока BricksLockOn() переехал выше этого места. Когда вернётся, убрать это.
#if LOCKS==1
#if DEBUG==1
        if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
#else
        CBrick::BricksLockOff(2)
#endif//DEBUG
#endif//LOCKS

        return;
    }
    if (m_vpBricks[i]->m_state == CBrick::DSTR_EN  &&  m_vpBricks[i]->GetBrickAniObj()->IsStopped())
    {
        m_vpBricks[i]->m_state = CBrick::OFF;	/*Костылю*/
//#if LOCKS==1
//      while (CBrick::BricksLockOn() != 1)	//включаю замок, чтобы никто не пользовался пока итераторами и индексами
//      {
//      }
//#endif

#if DONT_DELETE_ANI_BRICKS==0
        //Если это не Базовый кирпич
        if (i!=0)
        {
            sc->RemoveAni(m_vpBricks[i]->GetBrickAniObj());	//Удалить ани-объект из сцены
            delete m_vpBricks[i]->GetBrickAniObj();
        }

        //Если это первый (базовый) кирпич, то обрабатываю отдельно - первые 2 строки для него выполнять нельзя, (т.к. остальные его копии - будут ли нормально работать после его удаления) Да и создавал его не я
        else
#endif//DONT_DELETE_ANI_BRICKS
        {
            m_vpBricks[i]->GetBrickAniObj()->SetPos(-300,-300); //убираю из игрового поля
            m_vpBricks[i]->GetBrickAniObj()->SetStatic(/*ANI_SC01_BRICK*/ST_SC01_BRICK_BEGIN);
            m_vpBricks[i]->m_state = CBrick::OFF;
        }
		
        //Дальше код универсален для любого кирпича
        delete m_vpBricks[i];

        m_vpBricks.erase(m_vpBricks.begin()+i);	//Костыли - пробовал вместо этой строки обменять перед удалением с последним или 1ым эл-том - проблема не ушла
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

        m_uGameScore+=10;

        if (m_vpBricks.empty()  &&  m_GameState==RUN)
        {
            m_GameState=WON;
        }
//#if LOCKS==1
//#if DEBUG==1
//      if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//      CBrick::BricksLockOff(2)
//#endif
//#endif
    }

//Костыли: перенёс под скобку
#if LOCKS==1
#if DEBUG==1
        if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
#else
        CBrick::BricksLockOff(2)
#endif//DEBUG
#endif//LOCKS

}



//// InitOfBricksAniObjects() ////
void CArkanoidController::CBricksManager::InitOfBricksAniObjects()
{	
    //#OUTDATED: т.к. юзаю .push_back() vector'а, и, поэтому, общий индекс для m_vpBricks и vpBricksAniObjs уже не нужен, перехожу на итераторы
    //for (char i = 0; i < m_vpBricks.size(); ++i)
    //{
    //    vpBricksAniObjs[i] = m_vpBricks[i]->GetBrickAniObj();
    //}
    for (auto& it = m_vpBricks.begin(); it != m_vpBricks.end(); ++it)
    {
        vpBricksAniObjs.push_back( (*it)->GetBrickAniObj() );
    }
}

