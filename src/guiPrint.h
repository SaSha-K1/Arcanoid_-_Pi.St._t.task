#ifndef GUI_PRINT_H
#define GUI_PRINT_H


#include <cstdlib>
#include <string>       //д/ std::string
#include "types.h"


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



#endif//GUI_PRINT_H