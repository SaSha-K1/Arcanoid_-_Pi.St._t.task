#include "guiPrint.h"



//Print #1: в формате "<Текстовка>"
void guiPrint::Print(
        const std::string &txt, 
        const u32 y, const u32 x/*=10*/,
        const u32 r/*=10*/, const u32 g/*=220*/, const u32 b/*=220*/, const u32 a/*=100*/
    )
{ 													
    static std::ostringstream strStream; 						
    strStream.str("");	 					
    strStream<< txt; 								
    GET_FONT("dlg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a), x,y,0, strStream.str().c_str());
}