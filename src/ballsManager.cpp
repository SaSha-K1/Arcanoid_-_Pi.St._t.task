
#include <algorithm>        //для std::for_each()
#include "ballsManager.h"
#include "pictureobj.h"     //для ->StopMoveTo() и др.
#include "ball.h"           //вызываю C-tor и обращаюсь к др.методам



//// C-tor ////
_BALLS_MNGR::CBallsManager (const std::vector<CPictureObject*>&    rvBallsPictObjects)
    : m_uMAX_BALL_NUM(4)    //4, т.к. сейчас придумал 4 типа шариков
{
    //Init of map m_mBallsPictObjects:

    //#OUTDATED (напишу красивее - ну ... см. коммент ниже):
    //for (int i=0; i<rvBallsPictObjects.size(); ++i)
    //    m_mBallsPictObjects[ rvBallsPictObjects[i] ] = 0;

    //#NOTE: Хотел написать красивее, чем обычный перебор по индексу или итераторами. ..
    //.. По итогу +2 строки (со скобками), длина кода +/- такая же, понятность хуже, но зато выпендрился, что немного знаю лямбды ..
    //.. и алгоритмы stl =)  Рабочий код так бы писать не стал, это для тестового, чтобы показать, что понимаю, как работает и умею ..
    //.. использовать, если найду подходящее место для применения.
    std::for_each(
        rvBallsPictObjects.begin(),  
        rvBallsPictObjects.end(),  
        [this] (CPictureObject* pPictObj) /*mutable*/
            {
                /*this->*/m_mBallsPictObjects[pPictObj] = false/*0*/;    ///@@@ #WARNING: по идее тут объекты из capture-list'а менять должно быть нельзя. Если так, то раскомментить `mutable` в сигнатуре лямбды строкой выше
            }
    );

}


//// AddBall() ////
//#OUTDATED: void _BALLS_MNGR::AddBall(std::array<vector2,m_suNUM_OF_HIT_CHCK_PNTS> &ballHitChckPnts, bool attachToPad)
void _BALLS_MNGR::AddBall(const bool attachToPad,  /*const*/ CPictureObject* const pPad)
{
    if (m_vBalls.size() < m_uMAX_BALL_NUM)     ///@@@ если эта проверка перекочует гарантированно во все места вызова AddBall(), то тут можно обрамив #if DEBUG'ом, заменить на throw exception
    {
        //#OUTDATED:
        //for (u32 i = 0; i<m_uMAX_BALL_NUM; ++i)
        //    if ( ballsPictObjectsUsage[i] == 0 )
        //    {
        //        m_vBalls.push_back(new CBall(ballsPictObjects[i], ballHitChckPnts, attachToPad));
        //        ballsPictObjectsUsage[i]=1;
        //        break; //!!!
        //    }

        //std::for_each(m_mBallsPictObjects.begin(), m_mBallsPictObjects.end(), [] (..      //тут ещё и `break;` не работает - нужно BOOST_FOREACH
        for (auto/*&*/ it = m_mBallsPictObjects.begin();  it != m_mBallsPictObjects.end();  ++it)
            if (/*true*/false == (*it).second)
            {
                m_vBalls.push_back(new CBall((*it).first, attachToPad, pPad));
                (*it).second = 1;
                return;   //break;
            }
    }
}


//// DelBall() ////
void _BALLS_MNGR::DelBall(const u32 i,  CArkanoidController::GAME_STATE& rGameState)
{
#if DEBUG==1
    if (m_vBalls.size() <= i) throw CMyExcptn(10);
#endif

        m_vBalls[i]->GetBallPicObj()->StopMoveTo();
        m_vBalls[i]->GetBallPicObj()->SetPos(-200,-200);    //выводим из области видимости
        //Помечаем CPict.Obj. данного шарика как неиспользуемый:
        //#OUTDATED (теперь это пишется одной строкой! =D ):
        //for (auto& it2 = ballsPictObjects.begin(); it2!=ballsPictObjects.end(); ++it2)  ///@@@ назвать итератор просто "it" - проверить работоспособность+стабильность
        //{
        //    if ( m_vBalls[i]->GetBallPicObj() == (*it2) )
        //    {
        //        ballsPictObjectsUsage[std::distance(ballsPictObjects.begin(), it2)]=false;	
        //        break; /*не было*/
        //    }
        //}

        ( *m_mBallsPictObjects.find(m_vBalls[i]->GetBallPicObj()) ).second = false;

	
        delete m_vBalls[i];
        m_vBalls.erase(m_vBalls.begin()+i);
	
        if (m_vBalls.empty() && rGameState==RUN)
            rGameState = LOST;		//`processWinLostState();` не вызываю - отрабатываю в OnRender(), иначе всё, что сделаю, останется "за кадром"
}


//// CleanBalls() ////
void _BALLS_MNGR::CleanBalls(
        const float                         fSceneH,  
        const float                         fSceneW,
        CArkanoidController::GAME_STATE&    rGameState
    )
{
    if (!m_vBalls.empty())
    {
        for (char i=m_vBalls.size()-1;  i >= 0;  --i) //Не итератором, т.к. теряют валидность при .erase()	//(auto it=m_vBalls.begin();  it != m_vBalls.end();  ++it)
        {
            CPictureObject *bo = m_vBalls[i]->GetBallPicObj();
            if (bo->GetPos().x < -bo->GetSize().x -5  ||  bo->GetPos().x > fSceneW +5  ||  
                bo->GetPos().y < -bo->GetSize().y -5  ||  bo->GetPos().y > fSceneH +5)
            {
                DelBall(i, rGameState);	//условие проигрыша проверяю в методе delBall()
                //Не использую тут итераторы, а перебираю в обратном порядке, потому что внутри DelBall() удаляется it и, ==> происходит ресайз вектора, наружный цикл более неработоспособен, т.к. итераторы уже недействительны.
            }
        }//for
    }//if
}


//// ProcessIsHit() ////
void _BALLS_MNGR::ProcessIsHit(     //Провека коллизий и пересчёт траекторий
            //std::vector<CBall*> vpBalls
            CPictureObject*                 pPad,
            CLockableAccessVec<_BRICK*>*    pLavBricks,
            float                           fSceneW,
            CArkanoidController::GAME_STATE gameState
        )
{   ///@@ Stoped_here 2014/09/16
    _BALL::_DIRECTION dir;
    for (auto/*&*/ itb = m_vBalls.begin();  itb != m_vBalls.end();  ++itb)	///@@@ Использование ITERATOR'а 
    {
        //#XI: dir = (*itb)->IsHit( (*itb)->GetBallPicObj(), pPad, *pLavBricks, fSceneW, gameState );
        dir = (*itb)->IsHit( pPad, *pLavBricks, fSceneW, gameState );
        if (dir != -1)
        {   ///@@@ поменять след.2 вызова местами (вначале сдвинуть, потом запустить), Хотя наверное нет, т.к. до момента, когда шарик полетит в противопол.направлении, он может уже снова войти в контакт с тем же препятствием, ну и дальше получится хрень.. подумать
            (*itb)->GetBallPicObj()->MoveToPolarRo(FLT_MAX,  (*itb)->GetNewMoveCenter(dir),  (*itb)->GetVel());
           //сдвигаю шарик на 2-3пикселz, чтобы выйти из тек.коллизии. Пояснчения причин и альтернативы см. в прим.к описанию метода _BALL::InitFewPixelsShift()
           (*itb)->GetBallPicObj()->SetPos( Vec3To2((*itb)->GetBallPicObj()->GetPos()) + _BALL::GetNumOfPixelsForShift(dir) );
        }
    }

}


//// MoveBallWithPad() ////
void _BALLS_MNGR::MoveBallWithPad(CPictureObject* pPad, float fCurrPadY)
{
    if (_BALL::GetIsAttachedToPaddle())	//этим условием экономлю время на проход по циклу на кажд.обработке оборачивающей ф-ции, т.к. часто оно не будет выполняться 
    {
        CPictureObject *bo = _BALL::GetIsAttachedToPaddle()->GetBallPicObj();
        bo->SetPos(pPad->GetCenter().x - bo->GetSize().x/2,   fCurrPadY - bo->GetSize().y-1 );
    }

}


//// BallThrowIn() ////
void _BALLS_MNGR::BallThrowIn()
{
    if (_BALL::GetIsAttachedToPaddle()) 
    {
        /*CBall*/_BALL* b = /*CBall*/_BALL::GetIsAttachedToPaddle();
        CPictureObject* bo = b->GetBallPicObj();
        bo->MoveToPolarRo(FLT_MAX, b->GetNewMoveCenter(_BALL::S), b->GetVel());	
        b->TryToInvertIsAttachedToPad();
    }
}


//// CleanBalls() ////
void _BALLS_MNGR::CleanBalls(CArkanoidController::GAME_STATE& gameState)
{
    if (!m_vBalls.empty())
        for (char i=m_vBalls.size()-1;  i>=0; --i)
            DelBall(i, gameState);
}

#if DEBUG==1
    float _BALLS_MNGR::GetBallFi (const u32 ballNum) const
    { 
        if (ballNum < m_vBalls.size() )
            return m_vBalls[ballNum]->GetFi(); 
        else
            return 999.99;  //999.99 == error
    }
#endif //DEBUG