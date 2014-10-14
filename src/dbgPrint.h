#ifndef DBG_PRINT_H
#define DBG_PRINT_H


#include <cstdlib>
#include <string>
#include <sstream>          //��� cpp-����� - std::ostringstream
#include "globals.h"
#include "types.h"
#include "font.h"           //��� cpp-����� - GET_FONT()

///@@@ ���� ������ ������ h � cpp. ������ ����� namespace ������ �� ����� ����� � �������� ������ ���� �� ���������� �-���, � ����� ������ �-���.

namespace dbgPrnt
{
    #if DEBUG==1 //! ���������� ��� Debug-������																																

    // DbgPrint #1: � ���� "<�����>"
    void DbgPrint(
            const std::string &txt, 
            const u32 y,    const u32 x=10,                                        //����������
            const u32 r=1, const u32 g=255, const u32 b=255, const u32 a=100     //����
        );														



    // DbgPrint #2: � ���� "<�����><var>"
    template <typename T> void DbgPrint(
            const std::string &txt,
            const T &var,
            const u32 y,    const u32 x=10,                                     //����������
            const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100     //����
        );																			



    // DbgPrint #3: � ���� "<�����><var1><delim><var2>"
    template <typename T1, typename T2> void DbgPrint(
            const std::string &txt,
            const T1 &var1, 
            const T2 &var2, 
            const std::string &delim, 
            const u32 y, const u32 x=10,                                        //����������
            const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100     //����
        );																							

    #endif//DEBUG==1												
}//of namespace



//////////////////////////////////////////////////////////////
//  ��, ��� ���� �� �������� ������ templates � .cpp-�����  //
//////////////////////////////////////////////////////////////

//#include <sstream>          //��� std::ostringstream
//#include "font.h"           //��� GET_FONT()



#if DEBUG==1 //! ���������� ��� Debug-������																																

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



// DbgPrint #2: � ���� "<�����><var>"
template <typename T> void dbgPrnt::DbgPrint(
        const std::string &txt,
        const T &var,       ///@@@ ���� ������ ����� ��������, ����� �� �-�� DbgPrint() �����������, �� ����������� ��� ��������� ptr �� var-�� - ����� ��� ���������� - ��������� ����� ���������� �-� ����������
                                                                                        // /*��� ���*/ ���������������� def.�������� (������ � decl.)
        const u32 y,    const u32 x/*=10*/,                                                 //����������
        const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/     //����
    )
{ 																									
    static std::ostringstream strStream; 												
    strStream.str("");	 											
    strStream<< txt<< var; 																
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																				



// DbgPrint #3: � ���� "<�����><var1><delim><var2>"
template <typename T1, typename T2> void dbgPrnt::DbgPrint(
        const std::string &txt,
        const T1 &var1, 
        const T2 &var2, 
        const std::string &delim,                                                       // /*��� ���*/ ���������������� def.�������� (������ � decl.)
        const u32 y, const u32 x/*=10*/,                                                    //����������
        const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/     //����
    )	
{															
    static std::ostringstream strStream;										
    strStream.str("");																	
    strStream<< txt<< var1<< delim<< var2; 						
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																								



#endif//DEBUG==1	



#endif//DBG_PRINT_H