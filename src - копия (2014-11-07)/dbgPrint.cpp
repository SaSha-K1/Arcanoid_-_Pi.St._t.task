#include "dbgPrint.h"



#if DEBUG==1 //! ���������� ��� Debug-������																																
                ///@@@?! �� ������, ��� ��� ��������� - cpp-���� ��������� ������ ��� DEBUG!=1, � � .h ����� ����� ������ �������.

// DbgPrint #1: � ���� "<�����>"
void dbgPrnt::DbgPrint(
        const std::string &txt,                                                             // /*��� ���*/ ���������������� def.�������� (������ � decl.)
        const u32 y, const u32 x/*=10*/,                                                    //����������
        const u32 r/*=1*/, const u32 g/*=255*/, const u32 b/*=255*/, const u32 a/*=100*/    //����
    )		
{ 																			
    static std::ostringstream strStream; 									
    strStream.str("");	 										
    strStream<< txt; 															
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());	
}															


#endif //DEBUG