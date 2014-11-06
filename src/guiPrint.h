#ifndef GUI_PRINT_H
#define GUI_PRINT_H


#include <cstdlib>
#include <string>       //�/ std::string
#include <sstream>          //��� cpp-����� - std::ostringstream
#include "types.h"
#include "font.h"           //��� cpp-����� - GET_FONT()



namespace guiPrint
{
    //Print #1: � ������� "<���������>"
    void Print(
            const std::string &txt, 
            const u32 y, const u32 x/*=10*/,
            const u32 r/*=10*/, const u32 g/*=220*/, const u32 b/*=220*/, const u32 a/*=100*/
        );	//��� �������.�������� ������ � ���������� (��. ����)
    

    //Print #2: � ������� "<���������><var>"
    template <typename T> void Print(
            const std::string &txt, 
            const T &var, 
            const u32 y, const u32 x/*=10*/,
            const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/
        );
}//of namespace guiPrint



//////////////////////////////////////////////////////////////
//  ��, ��� �� �������� ������ templates ���� � .cpp-�����  //
//////////////////////////////////////////////////////////////

//#include <string>
//#include <sstream>          //��� std::ostringstream
//#include "font.h"           //��� GET_FONT()



//Print #2: � ������� "<���������><var>"
template <typename T> void guiPrint::Print(
        const std::string &txt, 
        const T &var, 
        const u32 y, const u32 x,/*=10,*/
        const u32 r,/*=10,*/ const u32 g,/*=10,*/ const u32 b,/*=10,*/ const u32 a/*=100*/
    )
{ 												
    static std::ostringstream strStream; 		
    strStream.str("");	 	//strStream.flush() �� ��������
    strStream<< txt<< var; 									
    GET_FONT("dlg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a), x,y,0, strStream.str().c_str());
}



#endif//GUI_PRINT_H