#ifndef GUI_PRINT_H
#define GUI_PRINT_H


#include <cstdlib>
#include <string>       //д/ std::string
#include <sstream>          //для cpp-части - std::ostringstream
#include "types.h"
#include "font.h"           //для cpp-части - GET_FONT()



namespace guiPrint
{
    //Print #1: в формате "<Текстовка>"
    void Print(
            const std::string &txt, 
            const u32 y, const u32 x/*=10*/,
            const u32 r/*=10*/, const u32 g/*=220*/, const u32 b/*=220*/, const u32 a/*=100*/
        );	//эти дефолтн.значения заданы в объявлении (см. выше)
    

    //Print #2: в формате "<Текстовка><var>"
    template <typename T> void Print(
            const std::string &txt, 
            const T &var, 
            const u32 y, const u32 x/*=10*/,
            const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/
        );
}//of namespace guiPrint



//////////////////////////////////////////////////////////////
//  То, что до озарения насчёт templates было в .cpp-файле  //
//////////////////////////////////////////////////////////////

//#include <string>
//#include <sstream>          //для std::ostringstream
//#include "font.h"           //для GET_FONT()



//Print #2: в формате "<Текстовка><var>"
template <typename T> void guiPrint::Print(
        const std::string &txt, 
        const T &var, 
        const u32 y, const u32 x,/*=10,*/
        const u32 r,/*=10,*/ const u32 g,/*=10,*/ const u32 b,/*=10,*/ const u32 a/*=100*/
    )
{ 												
    static std::ostringstream strStream; 		
    strStream.str("");	 	//strStream.flush() не работает
    strStream<< txt<< var; 									
    GET_FONT("dlg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a), x,y,0, strStream.str().c_str());
}



#endif//GUI_PRINT_H