
#ifndef MY_EXCPTN_H
#define MY_EXCPTN_H



class CMyExcptn
{
public:
	int m_code;
	CMyExcptn (int i) : m_code(i) {}
};


#define EXCPTN_CATCH catch (const CMyExcptn myEx)\
    {\
        time_t t = time(NULL);\
        switch (myEx.m_code)\
        {\
            case 1: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"1.Unexpected last `else` in CBall::computeNewMoveCenter().\"" << std::endl; break;\
            case 2: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"2.Unexpected last `else` in CBall::computeNewFi().\"\n" << std::endl; break;\
            case 3: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"3.Condition (m_fFi>=2*PI || m_fFi<=-2*PI) in CBall::normalizeFi() didn't satisfied.\"" << std::endl;  break;\
            case 4: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"4.vHP.size() is >2! in isHit().\"" << std::endl; break;\
            case 5: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"5.CBall::getNewMoveCenterIfHit() got dir==NO.\"" << std::endl; break;\
            case 6: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"6.CBall::computeStartFi() was called by ball, that isn't attached to paddle.\"" << std::endl; break;\
            case 7: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"7.In isHit()  vHP.size()==2 but values combination is unexpected.\"" << std::endl; break;\
            case 9: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"9.The locateAttachedToPadBall() was called for ball, that isn't attached to Pad.\"" << std::endl; break;\
            case 10: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"10.The delBall() get i, that is bigger than vector size().\"" << std::endl; break;\
            case 11: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"11.Unexpected last `else` in CBall::isHit().\"\n" << std::endl; break;\
            case 12: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"12.The m_fFi>PI || <=-PI in CBall::isHit() before `switch(dir)`.\"\n" << std::endl; break;\
            case 13: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"13.The `default` condition-branch in `switch(dir)` in CBall::isHit().\"\n" << std::endl; break;\
            case 14: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"14.The delBrick() get i, that is bigger than vector size().\"" << std::endl; break;\
            case 15: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"15.m_vpBricks.empty() condition isn't complete, but \"WON\" state is set.\"" << std::endl; break;\
            case 16: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"16.CBrick::BricksLockOff() returned \"0\" in isHit().\"" << std::endl; break;\
            case 17: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"17.CBrick::BricksLockOff() returned \"0\" in delBrick().\"" << std::endl; break;\
            default: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"Unexpected exception!\"" << std::endl; break;\
        }\
    }


std::ofstream exceptPrintFile("exceptPrintFile.txt", ios::app);



#endif //MY_EXCPTN_H