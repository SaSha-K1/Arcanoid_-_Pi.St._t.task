#include "dbgPrint.h"



#if DEBUG==1 //! Объявления для Debug-версии																																
                ///@@@?! Не уверен, что так корректно - cpp-шник получится пустым при DEBUG!=1, и в .h файле будут только инклуды.

// DbgPrint #1: в виде "<Текст>"
void dbgPrnt::DbgPrint(
        const std::string &txt,                                                             // /*вот так*/ закомментированы def.значения (заданы в decl.)
        const u32 y, const u32 x/*=10*/,                                                    //Координаты
        const u32 r/*=1*/, const u32 g/*=255*/, const u32 b/*=255*/, const u32 a/*=100*/    //Цвет
    )		
{ 																			
    static std::ostringstream strStream; 									
    strStream.str("");	 										
    strStream<< txt; 															
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());	
}															


#endif //DEBUG