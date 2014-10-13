
#include "ballsManager.h"
#include "pictureobj.h"

//C-tor
CArkanoidController::CBallsManager::CBallsManager (
        std::array<CPictureObject*, 0U/*MAX_BALL_NUM*/>&  raBallsPictObjects   ///#WARNING: сработает ли "0U"..?
    )
    :m_uNUM_OF_BRICKS_ON_LEVEL1(12)
{
    //Init of map m_mBallsPictObjects:
    for (int i=0; i<raBallsPictObjects.size(); ++i)
        m_mBallsPictObjects[ raBallsPictObjects[i] ] = 0;
}


void CArkanoidController::CBallsManager::AddBall(std::array<vector2,uNumOfHitChckPnts> &ballHitChckPnts, bool attachToPad)
{
    if (g_vBalls.size() < MAX_BALL_NUM)
    {
        for (u32 i = 0; i<MAX_BALL_NUM; ++i)
            if ( ballsPictObjectsUsage[i] == 0 )
            {
                g_vBalls.push_back(new CBall(ballsPictObjects[i], ballHitChckPnts, attachToPad));
                ballsPictObjectsUsage[i]=1;
                break; //!!!
            }
    }
}


void CArkanoidController::CBallsManager::DelBall(u32 i)
{
#if DEBUG==1
    if (g_vBalls.size()<=i) throw CMyExcptn(10);
#endif

        g_vBalls[i]->getBallPicObj()->StopMoveTo();
        g_vBalls[i]->getBallPicObj()->SetPos(-200,-200);
        //Помечаем CPict.Obj. данного шарика как неиспользуемый:
        for (auto it2 = ballsPictObjects.begin(); it2!=ballsPictObjects.end(); ++it2)
        {
            if ( g_vBalls[i]->getBallPicObj() == (*it2) )
            {
                ballsPictObjectsUsage[std::distance(ballsPictObjects.begin(), it2)]=false;	
                break; /*не было*/
            }
        }

	
        delete g_vBalls[i];
        g_vBalls.erase(g_vBalls.begin()+i);
	
        if (g_vBalls.empty() && m_GameState==RUN)
            m_GameState = LOST;		//`processWinLostState();` не вызываю - отрабатываю в OnRender(), иначе всё, что сделаю, останется "за кадром"
}




