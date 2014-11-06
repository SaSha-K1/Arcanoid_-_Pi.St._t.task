
#ifndef MY_EXCPTN_H
#define MY_EXCPTN_H



#include <fstream>      //д/ std::ofstream
#include <ctime>    ///@@@ added only 2014/11/06 - bacause of ball.cpp curses(ругается)



class CMyExcptn
{
public:
	int m_code;
	CMyExcptn (int i) : m_code(i) {}
};

///@@@ Warning C4996: 'asctime': This function or variable may be unsafe. Consider using asctime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details. 297
///@@@ Warning C4996: 'localtime': This function or variable may be unsafe. Consider using localtime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details. 297
#define EXCPTN_CATCH catch (const CMyExcptn myEx)\
    {\
        time_t t = time(NULL);\
        switch (myEx.m_code)\
        {\
            case 1: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"1.Unexpected last `else` in CBall::computeNewMoveCenter().\"" << std::endl; break;\
            case 2: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"2.Unexpected last `else` in CBall::computeNewFi().\"\n" << std::endl; break;\
            case 3: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"3.Condition (m_fFi>=2*PI || m_fFi<=-2*PI) in CBall::normalizeFi() didn't satisfied.\"" << std::endl;  break;\
            case 4: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"4.vHP.size() is >2! in IsHit().\"" << std::endl; break;\
            case 5: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"5.CBall::getNewMoveCenterIfHit() got dir==NO.\"" << std::endl; break;\
            case 6: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"6.CBall::computeStartFi() was called by ball, that isn't attached to paddle.\"" << std::endl; break;\
            case 7: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"7.In IsHit()  vHP.size()==2 but values combination is unexpected.\"" << std::endl; break;\
            case 9: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"9.The locateAttachedToPadBall() was called for ball, that isn't attached to Pad.\"" << std::endl; break;\
            case 10: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"10.The delBall() get i, that is bigger than vector size().\"" << std::endl; break;\
            case 11: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"11.Unexpected last `else` in CBall::IsHit().\"\n" << std::endl; break;\
            case 12: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"12.The m_fFi>PI || <=-PI in CBall::IsHit() before `switch(dir)`.\"\n" << std::endl; break;\
            case 13: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"13.The `default` condition-branch in `switch(dir)` in CBall::IsHit().\"\n" << std::endl; break;\
            case 14: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"14.The DelBrick() get i, that is bigger than vector size().\"" << std::endl; break;\
            case 15: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"15.m_vpBricks.empty() condition isn't complete, but \"WON\" state is set.\"" << std::endl; break;\
            case 16: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"16.GetReservedKey() != GetCurrValOfLockCode() in _BALL::IsHitWithBricks() before ReleaseAccessHandler().\"" << std::endl; break;\
            case 17: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"17.CBrick::BricksLockOff() returned \"0\" in DelBrick().\"" << std::endl; break;\
            case 18: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"18.In CLaoHandle C-tor m_uLockCode != 0.\"" << std::endl; break;\
            case 19: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"19.In CLaoHandle D-tor m_uLockCode != reserved copy of key from C-tor.\"" << std::endl; break;\
            case 20: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"20.CLaoHandle C-tor got `key` parameter == 0. (Must be >0).\"" << std::endl; break;\
            case 21: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"21.m_LavBricks.CreateAccessHandler(1) returned ptr == `nullptr` in RetrieveBrick().\"" << std::endl; break;\
            case 22: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"22.m_LavBricks.CreateAccessHandler(2) returned ptr == `nullptr` in IsHitWithBricks().\"" << std::endl; break;\
            case 23: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"23.In _BALL::InitHitChckPntsVec() size of vector m_svfBallHitChckPntsAngles != m_suNUM_OF_HIT_CHCK_PNTS.\"" << std::endl; break;\
            case 24: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"24.CLavHandler C-tor have got reference `rLockCode`, that != 0.\"" << std::endl; break;\
            case 25: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"25.In case `LOST` in the `CArkanoidController::ProcessWinLostState()` vector of bricks isn't empty.\"" << std::endl; break;\
            case 26: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"26.ReleaseAccessHandler() returned not `0` in _BALL::IsHitWithBricks().\"" << std::endl; break;\
            default: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"Unexpected exception!\"" << std::endl; break;\
        }\
    }


extern std::ofstream exceptPrintFile;



#endif //MY_EXCPTN_H