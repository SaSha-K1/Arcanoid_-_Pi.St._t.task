#ifndef DBG_PRINT_H
#define DBG_PRINT_H



#include <cstdlib>
#include <string>
#include <sstream>          //для cpp-части - std::ostringstream
#include "globals.h"
#include "types.h"      //можно выключить, пока включён "globals.h", содержащий это инклуд.
#include "font.h"           //для cpp-части - GET_FONT()


#if DEBUG==1 //! Объявления для Debug-версии
                ///@@@?! Не уверен, что так корректно - cpp-шник получится пустым при DEBUG!=1, и в .h файле будут только инклуды.

///@@@ Пока только склеил h и cpp. Вообще лучше namespace кинуть до конца файла и возможно вообще уйти от объявлений ф-ций, а сразу давать О-ние.

namespace dbgPrnt
{
    // DbgPrint #1: в виде "<Текст>"
    void DbgPrint(
            const std::string &txt, 
            const u32 y,    const u32 x=10,                                        //Координаты
            const u32 r=1, const u32 g=255, const u32 b=255, const u32 a=100     //Цвет
        );														



    // DbgPrint #2: в виде "<Текст><var>"
    template <typename T> void DbgPrint(
            const std::string &txt,
            const T &var,
            const u32 y,    const u32 x=10,                                     //Координаты
            const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100     //Цвет
        );																			



    // DbgPrint #3: в виде "<Текст><var1><delim><var2>"
    template <typename T1, typename T2> void DbgPrint(
            const std::string &txt,
            const T1 &var1, 
            const T2 &var2, 
            const std::string &delim, 
            const u32 y, const u32 x=10,                                        //Координаты
            const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100     //Цвет
        );																							
}//of namespace

#endif//DEBUG==1												



//////////////////////////////////////////////////////////////
//  То, что было до озарения насчёт templates в .cpp-файле  //
//////////////////////////////////////////////////////////////

//#include <sstream>          //для std::ostringstream
//#include "font.h"           //для GET_FONT()



#if DEBUG==1 //! Объявления для Debug-версии																																


// DbgPrint #2: в виде "<Текст><var>"
template <typename T> void dbgPrnt::DbgPrint(
        const std::string &txt,
        const T &var,       ///@@@ если парсер будет париться, какую из ф-ий DbgPrint() подставлять, то попробовать тут принимать ptr на var-ую - может тип переменной - указатель будет определять ф-ю однозначно
                                                                                        // /*вот так*/ закомментированы def.значения (заданы в decl.)
        const u32 y,    const u32 x/*=10*/,                                                 //Координаты
        const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/     //Цвет
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
        const std::string &delim,                                                       // /*вот так*/ закомментированы def.значения (заданы в decl.)
        const u32 y, const u32 x/*=10*/,                                                    //Координаты
        const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/     //Цвет
    )	
{															
    static std::ostringstream strStream;										
    strStream.str("");																	
    strStream<< txt<< var1<< delim<< var2; 						
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																								



#endif//DEBUG==1	



#endif//DBG_PRINT_H