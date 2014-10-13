#ifndef DBG_PRINT_H
#define DBG_PRINT_H


#include <cstdlib>
#include <string>
#include "globals.h"
#include "types.h"


namespace dbgPrnt
{
    #if DEBUG==1 //! Объявления для Debug-версии																																

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

    #endif//DEBUG==1												
}//of namespace

#endif//DBG_PRINT_H