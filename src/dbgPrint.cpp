#include "dbgPrint.h"
#include <sstream>          //для std::ostringstream
#include "font.h"           //для GET_FONT()



#if DEBUG==1 //! Объявления для Debug-версии																																

// DbgPrint #1: в виде "<Текст>"
void dbgPrnt::DbgPrint(
        const std::string &txt, 
        const u32 y, const u32 x=10,                                        //Координаты
        const u32 r=1, const u32 g=255, const u32 b=255, const u32 a=100     //Цвет
    )		
{ 																			
    static std::ostringstream strStream; 									
    strStream.str("");	 										
    strStream<< txt; 															
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());	
}															



// DbgPrint #2: в виде "<Текст><var>"
template <typename T> void dbgPrnt::DbgPrint(
        const std::string &txt,
        const T &var,       ///@@@ если парсер будет париться, какую из ф-ий DbgPrint() подставлять, то попробовать тут принимать ptr на var-ую - может тип переменной - указатель будет определять ф-ю однозначно
        const u32 y,    const u32 x=10,                                     //Координаты
        const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100     //Цвет
    )
{ 																									
    static std::ostringstream strStream; 												
    strStream.str("");	 											
    strStream<< txt<< var; 																
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																				



// DbgPrint #3: в виде "<Текст><var1><delim><var2>"
template <typename T1, typename T2> void dbgPrnt::DbgPrint(
        const std::string &txt,
        const T1 &var1, 
        const T2 &var2, 
        const std::string &delim, 
        const u32 y, const u32 x=10,                                        //Координаты
        const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100     //Цвет
    )	
{															
    static std::ostringstream strStream;										
    strStream.str("");																	
    strStream<< txt<< var1<< delim<< var2; 						
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																								



#endif//DEBUG==1												