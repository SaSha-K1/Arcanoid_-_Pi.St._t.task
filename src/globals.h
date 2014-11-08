#ifndef GLOBALS_H
#define GLOBALS_H


#include "types.h"


#define DEBUG 1
#define HARD_DEBUG 1    //вклчючает доп.проверки и расширенные выводы в лог.
#define SEARCH_4_VEC_ERR 2  ///@@@ Когда найду, убрать и это и все боки "#if SEARCH_4_VEC_ERR==1", а также исключить из проекта файлы search4VecErr.h и .cpp
                            // "2" - временная задержка м/у итерациями OnUpdate() и OnRender()

//Костыли
#define LOCKS 1		//1 - включает код с BricksLock'ами;  0 - выкл.
#define LOW_LVL_LOCKS 0		//можно включить только, если LOCKS==1. Эта опция заставляет циклы BricksLock'ов не ждать освобождения замка, а просто пропускать свой ход, надеясь, получить доступ при следующей итерации. (0-Выкл; 1-Вкл)
#define NUMBER_OF_BRICKS 12	//варианты:  2 - (для > убодного дебага); и 12 - как планировалось
#define DONT_DELETE_ANI_BRICKS 1	//Пробившись часов 8 над ошибкой потоков (подробно расписал в письме Юлии Маляревич), решил сдаться и обойти её, не удаляя ани-объекты. Если ещё решу побороться, нужно просто выключить эту переменную (1-Вкл; 0-Выкл)
#define OS WIN

#define CTG(x) (tan(HALFPI - (x)))

const float HALFPI = PI/2;	//экономлю время работы CTG()

///@@@ TRY to back `&`. Temporary replaced it because of WRN: C4239: nonstandard extension used : 'argument' : conversion from 'vector2' to 'vector2 &' 129
vector2 Vec3To2(const vector3 /*&*/v3);     //правильнее было бы поместить в "types.h", но пишу тут, чтобы ..
                                            //.. не расползаться слишком своим кодом по не моим файлам.

///@@ Stoped here (2014/09/02): в след.строках классы, упомянутые после первого :: уже inaccessible - перенести в public?;  
///После этого продолжаю искать `m_vpBricks` и заменять конструкциями с `m_LavBricks` (в частности в f bricksManager.h строка 72 продолжить)
//// Types: ////
//#OUTDATED (перенёс в соотв-щие .h файлы, т.к. иначе пришлось бы сюда инклудить эти .h-файлы, что было бы круговым инклудом, т.к. к ним уже данный файл подключен):
//typedef CArkanoidController::CBricksManager             _BRICKS_MNGR;
//typedef CArkanoidController::CBricksManager::CBrick     _BRICK;
//typedef CLockableAccessVec<_BRICK*>::CLavHandler        _LAV_OFBRICKS_HNDLR;



#endif //GLOBALS_H

