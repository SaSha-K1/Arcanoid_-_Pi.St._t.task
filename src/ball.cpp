
#include "pictureobj.h"     //для pPad->GetCenter()
#include "random.h"         //для CRndGen::GetInst()
#include "ball.h"
#include "myExcptn.h"
#include "brick.h"  ///@@@ Это плохо. Нужно, т.к. обращаюсь к ->GetBrickAniObj(), ->m_state, и BRICK_STATE. Всё это в методе IsHitWithBrick().   #TODO: Попытаться избавиться от такой зависимости этих 2х классов (возможно, заменив зависимостью от интерфейсного класса CAbstractBrick, от которого унаследовать уже CBrick).


//// TYPES:
typedef /*typename/**/ CLockableAccessVec<_BRICK*>::CLavHandler _LAV_OFBRICKS_HNDLR;  // `typename` may be not allowed out of `template`  ///: err C2899: typename cannot be used outside a template declaration	10



//// Static data-members:
_BALL*                  _BALL::m_spIsAttachedToPad      = nullptr;
const u32               _BALL::m_suNUM_PIXELS_4_SHIFT   = 3;
const float             _BALL::m_fDEFAULT_BALL_VEL      = 200;
const u32               _BALL::m_suNUM_OF_HIT_CHCK_PNTS = 8;    //Сейчас ==8 : (N(north),E(east),S,W + 4 промежуточных). ..
                                                                //.. !В случае изменения, увязать с новым знач-ем: enum _DIRECTION и ..
                                                                //.. (возможно) вектор vfBallHitChckPntsAngles (в методе ..
                                                                //.. InitBallHitChckPnts(). Также глянуть на всякий метод IsHitWithPad().
std::vector<float>                  _BALL::m_svfBallHitChckPntsAngles;
std::vector<vector2>                _BALL::m_svHitChckPnts;
std::map<_BALL::_DIRECTION,vector2>  _BALL::m_smFewPixelsShift;
///@@ Stoped_here 2014/09/14 - двигаться дальше по controller .h / .cpp 


//// C-tor ////
_BALL::CBall(
        CPictureObject*            const  pBall, 
        const bool                        bAttachToPad,
        /*const*/ CPictureObject*  const  pPad
    ) 
    : m_pBallPicObj(pBall), 
    m_fVelocity(m_fDEFAULT_BALL_VEL), 
    m_fFi(0),
    m_fFiPlus(0)
{ 
    if (m_svHitChckPnts.empty())    // `true` -> значит, создаём 1й шарик
    {
        InitHitChckPntsVec();
        InitFewPixelsShift();
    }
    if (bAttachToPad)
    {
        m_spIsAttachedToPad = this; 
        LocateAttachedToPadBall(pPad);	//хотя, если двинуть Pad, то шарик позиционируется и без этого, Но для фен-шуя 
    }
    InitFewPixelsShift();
}


//////////////////////\ PREPARE_ZONE \\\\\\\\\\\\\\\\\\\\\/
    ///@@просто перенести этот метод ниже в этом же файле

//#OUDATED ( всё же разбиваю обратно на 2 метода: Init..Angles() и Init..Pnts(), т.к. вектор углов нужен в InitFewPxlsShift() ):
//// InitHitChckPntsVec() ////
//void _BALL::InitHitChckPntsVec()
//{
//    std::vector<float> vfBallHitChckPntsAngles;   ///@@@ ? актуальна ли эта инфа?: //#IMPORTANT: в случ.необходимости изменения кол-ва эл-тов, менять только на чётное кол-во >=2. Увязать с новым знач-ем enum _DIRECTION и class CBall static member m_suNUM_OF_HIT_CHCK_PNTS.
//    // 1of2) Initializing of vector of ball hit check points ANGLES:
//    float newChckPnt;
//    vfBallHitChckPntsAngles.push_back(0.0);
//    for (u32 i=1;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)  //начинаю с 1, т.к. 0-вой эл-т инициализирую перед циклом
//    {
//        newChckPnt = 2 * PI * i / m_suNUM_OF_HIT_CHCK_PNTS;
//        if (PI < newChckPnt)    //Конвертирую угол их диапазона [0; 2*PI) в диапазон (-PI; PI].     
//                                //.. Тут можно было использовать и NormalizeFi(), если переписать её, чтобы принимала параметр, но ..
//                                //.. действие простое, а в NormalizeFi() больше проверок и действий. Решил, что это лишнее усложнение.
//            newChckPnt -= 2 * PI;
//        vfBallHitChckPntsAngles.push_back(newChckPnt);
//    }
//        //#OUTDATED: array инициализировал так:  
//        //      { 0,          //N
//        //        PI/4,       //NE
//        //        HALFPI,     //E
//        //        PI*3/4,     //SE
//        //        PI,         //S
//        //        -PI*3/4,    //SW
//        //        -HALFPI,    //W
//        //        -PI/4 };    //NW
//
//    // 2of2) Initializing of vector of hit check points:
//    float ballR = m_pBallPicObj->GetSize().y / 2.0;
//    float currAng;
//    for (auto& it = vfBallHitChckPntsAngles.begin();  vfBallHitChckPntsAngles.end() != it;  ++it)
//    {
//        //Использую выведенные формулы определения растровых x и y из уравнения окружности шарика:
//        //Последнее условное слагаемое (1 или 0) нужно: т.к. тут имеем дело с растровым изображением, то, ..
//        //.. коль нам нужен первый пиксель вне контура шарика, то в, случае поиска координат такой точки ..
//        //.. _в_ направлении оси координат, прибавляем 1-цу; в обратном направлении - отнимаем.
//        m_svHitChckPnts.push_back(
//            vector2(
//                (ballR-1) + ballR * sin(*it) 
//                    +((*it > 0   &&  *it < PI) ? 1 : 0),    ///@@@ /*myround(*/ надо ли?
//                (ballR-1) - ballR * cos(*it) 
//                    +(( (*it > -PI  &&  *it < -HALFPI)  ||  (*it > HALFPI  /*&&  currAng <= PI*/) ) ? 1 : 0)
//                    //Тут закомментированная часть условия не нужна, если угол не может быть больше PI (как сейчас)
//            )
//        );
//    }
//}


//// InitBallHitChckPntsAnglesVec() ////
///@@ Stoped here 2014/09/12 - и переходить к брикам.
void _BALL::InitBallHitChckPntsAnglesVec()  //Initializing of vector of ball hit check points ANGLES
{
    float newChckPnt;
    m_svfBallHitChckPntsAngles.push_back(0.0);
    for (u32 i=1;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)  //начинаю с 1, т.к. 0-вой эл-т инициализирую перед циклом
    {
        newChckPnt = 2 * PI * i / m_suNUM_OF_HIT_CHCK_PNTS;
        if (PI < newChckPnt)    //Конвертирую угол их диапазона [0; 2*PI) в диапазон (-PI; PI].     
                                //.. Тут можно было использовать и NormalizeFi(), если переписать её, чтобы принимала параметр, но ..
                                //.. действие простое, а в NormalizeFi() больше проверок и действий. Решил, что это лишнее усложнение.
            newChckPnt -= 2 * PI;
        m_svfBallHitChckPntsAngles.push_back(newChckPnt);
    }
        //#OUTDATED: array инициализировал так:  
        //      { 0,          //N
        //        PI/4,       //NE
        //        HALFPI,     //E
        //        PI*3/4,     //SE
        //        PI,         //S
        //        -PI*3/4,    //SW
        //        -HALFPI,    //W
        //        -PI/4 };    //NW
}


//// InitHitChckPntsVec() ////
void _BALL::InitHitChckPntsVec()
{
//#if DEBUG==1
//    if (m_suNUM_OF_HIT_CHCK_PNTS != m_svfBallHitChckPntsAngles.size()) throw CMyExcptn(23);
//#endif //DEBUG

    // 1of2) Initializing of vector of ball hit check points ANGLES:
    InitBallHitChckPntsAnglesVec();

#if DEBUG==1
    if (m_suNUM_OF_HIT_CHCK_PNTS != m_svfBallHitChckPntsAngles.size()) throw CMyExcptn(23);
#endif //DEBUG


    // 2of2) Initializing of vector of hit check points:
    float ballR = m_pBallPicObj->GetSize().y / 2.0;
    float currAng;  ///@@@? WRN C4101: 'currAng' : unreferenced local variable 141
    for (auto/*&*/ it = m_svfBallHitChckPntsAngles.begin();  m_svfBallHitChckPntsAngles.end() != it;  ++it)
    {
        //Использую выведенные формулы определения растровых x и y из уравнения окружности шарика:
        //Последнее условное слагаемое (1 или 0) нужно: т.к. тут имеем дело с растровым изображением, то, ..
        //.. коль нам нужен первый пиксель вне контура шарика, то в, случае поиска координат такой точки ..
        //.. _в_ направлении оси координат, прибавляем 1-цу; в обратном направлении - отнимаем.
        m_svHitChckPnts.push_back(
            vector2(
                (ballR-1) + ballR * sin(*it) 
                    +(
                        (*it > 0   &&  *it < PI) 
                        ? 1 
                        : 0
                    ),    ///@@@ /*myround(*/ надо ли?
                (ballR-1) - ballR * cos(*it) 
                    +(
                        ( (*it > -PI  &&  *it < -HALFPI)  ||  (*it > HALFPI  /*&&  currAng <= PI*/) ) 
                        ? 1 
                        : 0
                    )
                    //Тут закомментированная часть условия не нужна, если угол не может быть больше PI (как сейчас)
            )
        );
    }
}


//X//X//X//X//X//X//X\ PREPARE_ZONE \\X\\X\\X\\X\\X\\X\\X/


//// ComputeNewFi() ////
void _BALL::ComputeNewFi(const _DIRECTION dir)
{
    if (N==dir || S==dir)	//тут попарные сравнения, т.к. правила рассчёта Fi одинаковы для этих случаев
    {
        m_fFi = -m_fFi + PI +m_fFiPlus;	//корректировка угла в зависимости от точки удара по Pad'у только для S датчика.
        m_fFiPlus=0;	//использовали бонус и обнулили.
    }
    else if (NE==dir || SW==dir)
        m_fFi = -m_fFi -PI/2.0;
    else if (E==dir || W==dir)
        m_fFi = -m_fFi;
    else /*if (SE==dir || NW==dir)*/	//последнее условие можно и не проверять
#if DEBUG==1
        if (SE==dir || NW==dir)
#endif
            m_fFi = -m_fFi + PI/2;
#if DEBUG==1
        else throw CMyExcptn(2);
#endif

    NormalizeFi();
}


//// NormalizeFi() ////
void _BALL::NormalizeFi()
{
#if DEBUG==1
    if (2*PI <= m_fFi  ||  -2*PI >= m_fFi) throw CMyExcptn(3);
#endif
    if (PI < m_fFi)
        m_fFi = m_fFi - 2 * PI * (int( (m_fFi+PI) / (2*PI) ));		//Посл.множитель - убираем столько раз по 2пи, чтобы ..
                                                                    //.. рез-тат == (-PI ; PI].     //(m_fFi+PI) - тут +PI, чтобы ..
                                                                    //.. ближайшее меньшее нечётное целое число PI в составе m_fFi при ..
                                                                    //.. делении нацело на 2*PI давало +1. Это нужно,чтобы вписать m_fFi ..
                                                                    //.. в диапазон (-PI;PI].
    else if (-PI >= m_fFi)
        m_fFi = m_fFi + 2 * PI * (int( (m_fFi-PI) / (-2*PI) ));
}


//// ComputeNewMoveCenter() ////
vector2 _BALL::ComputeNewMoveCenter(const u32 moveCCoordScale/* = pow(10.0, 3) */) const
//Внутри /**/ - знач-я by def., установленные в объявл-ии метода. Тут справочно.
//Пар-тр `moveCCoordScale` - const множитель координаты центра движения (по-сути может быть другим числом, ..
//.. главное, чтобы был дост. далёк от единицы). Иначе может черезчур увеличиться значение второй координаты.
{
    vector2 mc;
    if (-PI/4 <= m_fFi  &&  PI/4 >= m_fFi) 	// ^ up
                                        mc.y = m_pBallPicObj->GetPos().y + moveCCoordScale,
                                        mc.x = m_pBallPicObj->GetPos().x - moveCCoordScale * tan(m_fFi);
    else if (( m_fFi > -PI  &&  m_fFi <= -3/4*PI )  ||  ( m_fFi >= 3/4*PI  &&  m_fFi <= PI )) 	// v down
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale,
                                        mc.x = m_pBallPicObj->GetPos().x + moveCCoordScale * tan(PI+m_fFi);
    else if (m_fFi > PI/4  &&  m_fFi < 3/4*PI) 	// > right
                                        mc.x = m_pBallPicObj->GetPos().x - moveCCoordScale,
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale * CTG(PI-m_fFi);	//Тут и в след.ветке ..
                                                                                                //.. юзаю ctg, т.к. ч/з tg считать ..
                                                                                                //.. опасно в этих областях углов.
    else if (m_fFi > -3/4*PI  &&  m_fFi < -PI/4) 	// < left
                                        mc.x = m_pBallPicObj->GetPos().x + moveCCoordScale,
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale * CTG(m_fFi);
#if DEBUG==1
    else throw CMyExcptn(1);
#endif
    return mc;
}


//// ComputeStartFi() ////
void _BALL::ComputeStartFi(
        const float rndStartAng,/* = 10*(float)PI/180, */   //Внутри /**/ - знач-я by def., установленные в объявл-ии метода. Тут справочно
        const u32   multiplForGen/* = pow(10.0, 4) */       //Внутри /**/ - знач-я by def., установленные в объявл-ии метода. Тут справочно
     )    
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad)
        throw CMyExcptn(6);
    else
#endif
    {
        m_fFi = (float)CRndGen::GetInst().Generate( static_cast<u32>(2 * rndStartAng * multiplForGen) ) / multiplForGen - rndStartAng;
    }
}


//// GetNewMoveCenter() ////
vector2 _BALL::GetNewMoveCenter(const _DIRECTION dir)
{
#if DEBUG==1
    if (NO==dir) throw CMyExcptn(5);
#endif

    if (this != m_spIsAttachedToPad)		
    {
        m_pBallPicObj->StopMoveTo();	///@@@ можно без этой строчки. Убрав, проверить стабильность работы.
        ComputeNewFi(dir);
    }
    else ComputeStartFi();  //По сути это ветка  "if (this == m_spIsAttachedToPad)"
    return ComputeNewMoveCenter();
}


//// TryToInvertIsAttachedToPad() ////
bool _BALL::TryToInvertIsAttachedToPad()    ///@@@ 1) возможно разнести на 2 метода, т.к. это вроде швейцарский нож получился;
                                        ///@@@ 2) нужно ли возвращаемое знечение?
                                    ///@@@ 3) проверить по коду, нужны ли проверки `if` в методе. Если нет, то и из названия убрать "TryTo".
{ 
    if (this == m_spIsAttachedToPad)	        { m_spIsAttachedToPad = nullptr;    return 1; }     ///@@@ возможно сбросить пар-р velocity шарика на default, иначе полетит с последней скоростью. Может лучше сбросить на прикреплении к Pad'у, нотогда не гуд, если скорость игры (по сути == дефолтной скорости шариков) увеличилась с момента прикрепления шарика
    else if (m_spIsAttachedToPad == nullptr)    { m_spIsAttachedToPad = this;       return 1; }
    else return 0; //По сути это ветка  "if other ball is attached"
}


//// LocateAttachedToPadBall() ////
void _BALL::LocateAttachedToPadBall(/*const*/ CPictureObject* const pPad)
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad) throw CMyExcptn(9);
#endif
    m_pBallPicObj->SetPos(
            pPad->GetCenter().x - m_pBallPicObj->GetSize().x / 2,   
            pPad->GetPos().y - m_pBallPicObj->GetSize().y - 1   //#OUTDATED (чтобы не передавать ещё 1 пар-р, беру просто координату Pad'а): g_fPaddleY - m_pBallPicObj->GetSize().y - 1
        );
}


//// ComputeFiPlus() ////
void _BALL::ComputeFiPlus(CPictureObject* pPad)
//Это рассчёт поправки (m_fFiPlus) к углу отражения (m_fFi) в зависимости от удаления точки контакта с Pad'ом от середины последнего.
{
    float dlt = m_pBallPicObj->GetCenter().x - pPad->GetCenter().x;	//Степень отклонения шарика от центра Pad'а при ударе
    float weightedDlt = dlt / pPad->GetSize().x / 2; //delta взвешенная по отношению к 1/2 длины Pad'а (т.е. получается в пределах [0, 1]
    m_fFiPlus = (PI/2) * weightedDlt * ( (fabs(m_fFi)-PI/2) / PI / 2 );	//Максимальная корректировка к углу == PI/4, корректируется ..
                                                                           //.. взвешенной dlt'ой и разницей m_fFi и PI/2, выраженной в ..
                                                                          //.. долях PI/2 (т.е. тоже [0, 1]). Для m_fFi ==почти PI/2 ..
                                                                         //.. (3,1417/2) угол отлёта всё равно получается < PI/2. Т.е. ..
                                                                        //.. корректировка безопасная - шарик не полетит вниз из-за неё.
}


//// IsHitWithScreenBorders() ////
void _BALL::IsHitWithScreenBorders(
        const float             fSceneW,
        std::vector<_DIRECTION>& rsltTrggrdHitChckPnts  //модифицируемый vector Triggered Hit Check Points - сработавшие датчики контакта
    ) const
{
    if (0  >=  m_svHitChckPnts[N].y + m_pBallPicObj->GetPos().y)             rsltTrggrdHitChckPnts.push_back(N);
    if (0  >=  m_svHitChckPnts[W].x + m_pBallPicObj->GetPos().x)             rsltTrggrdHitChckPnts.push_back(W);
    else if (m_svHitChckPnts[E].x + m_pBallPicObj->GetPos().x  >=  fSceneW)  rsltTrggrdHitChckPnts.push_back(E);
}


//// IsHitWithPad() ////
void _BALL::IsHitWithPad(
        //Inp.:    
        CPictureObject* const   pPad,
        const float             fMaxDltYWithPad,  
        const float             fDltPxlsToChckHit,   //Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. успевает ..
                                                    //.. пролететь сразу на несколько пикселей между итерациями просчёта ..
                                                    //.. оптимизирующих (см. "#OPT") проверок.
        //Outp.:
        std::vector<_DIRECTION>& rsltTrggrdHitChckPnts,  
        bool&                   rsltNeedToCheckBricks
     )
{
    //Оптимизация (далее #OPT): если шарик выше горизонтали Pad'а, или если улетел уже ниже (горизонтали Pad'a +10), ..
    //.. то коллизию с Pad'ом искать не нужно - начинаем перебор акт.объектов начиная со 2го (Pad всегда 1ый)
    if ( m_svHitChckPnts[S].y + m_pBallPicObj->GetPos().y   <   pPad->GetPos().y - fDltPxlsToChckHit  ||	
         m_pBallPicObj->GetPos().y   >   pPad->GetPos().y + fMaxDltYWithPad  ||   //if центр шарика ниже середины Pad'а,то уже game over.
         m_svHitChckPnts[E].x + m_pBallPicObj->GetPos().x   <   pPad->GetPos().x - fDltPxlsToChckHit  ||
         m_svHitChckPnts[W].x + m_pBallPicObj->GetPos().x   >   pPad->GetPos().x + pPad->GetSize().x + fDltPxlsToChckHit )
         /* , ТО - ничего. А рабочая ветка - следующая (она менее вероятна) */;
    else
    {
        //Рассчитываю диапазон датчиков HitChckPnts, кот-е нужно проверять при поиске столкновения ..
        //.. с Pad'ом - т.е. датчики нижней дуги полуокружности шарика включая точки концов:
        u32 begin = int (m_suNUM_OF_HIT_CHCK_PNTS/4.0 + 0.5);   // N/4 + .5
        u32 end = m_suNUM_OF_HIT_CHCK_PNTS - begin;             // N-(..)
        
        for (u32 d=begin;  d<=end;  ++d)  //Для 8 датчиков диапазон [2, 6] == [E, W]
        {                               //#OUTDATED (было актуально д/ диапазона выраженного в _DIRECTION): #WARNING(possibly). Возможно заменить implicit преобразование типов явным.
            if ( pPad->Hit(m_svHitChckPnts[d] + Vec3To2(m_pBallPicObj->GetPos())/*,0*/) )   /// Тут WRN: "C4239: nonstandard extension used : 'argument' : conversion from 'vector2' to 'vector2 &' "  - это опер-р "+" перегруженный д/ `vector2` принимает 2ой пар-р по ссылке. Видимо, всё ок.

            {
                rsltTrggrdHitChckPnts.push_back(static_cast<_DIRECTION>(d));
                rsltNeedToCheckBricks = 0;
                //Рассчёт поправки (m_fFiPlus) к углу (m_fFi) в зависимости от удаления точки контакта с Pad'ом от середины Pad'а:
                if (S == rsltTrggrdHitChckPnts.back())	//не корректирую угол, если касание по углу или боков.поверхности
                    ComputeFiPlus(pPad);
            }
        }
    }
}


//// IsHitWithBricks() ////
void _BALL::IsHitWithBricks(
        const float                         fDltPxlsToChckHit,      // :Inp.    //Зазор - за сколько пикселей до контакта меняем проверку ..
                                                                    //.. контакта вмещающих rect'ов на детальную проверку.
        /*const*/ CLockableAccessVec<_BRICK*>&  lavBricks,
        std::vector<_DIRECTION>&             rsltTrggrdHitChckPnts   // :Outp.
    )
{
//#OUTDATED:
//#if LOCKS==1    ///@@ Stoped here (2014-08-29) - читать о мьютексах - как их правильно организовать - в какой scope объявлять?
//        while (_BRICK::BricksLockOn(1) != 1)	///In IsHit()
//        {	/*включаю замок на изменения в m_vpBricks {{объект заменён на: `m_LavBricks`}}. Если уже включён кем-то другим, жду*/

            std::auto_ptr<_LAV_OFBRICKS_HNDLR> pLavHndlr = lavBricks.CreateAccessHandler(2);
#if DEBUG==1
            if (nullptr == pLavHndlr.get()) throw CMyExcptn(22);
#endif //DEBUG

//#TODO: если не будет работать без этого, пересмотреть этот код:
//#if LOW_LVL_LOCKS==1	
//            //Костыли: вставляю временно для проверки
//            if (_BRICK::BricksLockOn(1) != 1)
//                return NO;
//            else
//                break;
//#endif //LOW_LVL_LOCKS

//#OUTDATED:
//        }
//#endif //LOCKS

        //! Перебираем кирпичи:
        ///@@@ убрать лишнее:
        //for (auto it = m_vpBricks.begin(); it!=m_vpBricks.end(); ++it)	//была проблема с инвалидным итератором из-за многопоточности
        //for (auto it = m_vpBricks.rbegin(); it!=m_vpBricks.rend(); ++it)	///@@@ использую ITERAOR N
        //for (char ib = m_vpBricks.size()-1; ib>=0; --ib)
        //#OUTDATED: for (char ib = 0; ib < m_vpBricks.size(); ++ib)	//Костыли
        for (u32 ib = 0;  ib < (*pLavHndlr)->size();  ++ib)	//Костыли
        {		
            /*КОСТЫЛЬ*/ CAniObject ao = /*m_vpBricks*/(*(*pLavHndlr))[ib]->GetBrickAniObj();	//Костыли: пробую сохранять себе копию объекта - не помогает
            //CAniObject *ao = (*it)->getBrickAniObj();	//для лаконичности
            //CAniObject ao (m_vpBricks[ib]->getBrickAniObj());	//для лаконичности
                //if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao->GetPos().y -dlt  ||	//Opt.: проверка пересечения описывающих прямоугольников
                //m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao->GetPos().y + ao->GetSize().y +dlt ||	//opt
                //m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao->GetPos().x -dlt  ||					//opt
                //m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao->GetPos().x + ao->GetSize().x +dlt)	//opt
            //#OPT: проверка пересечения описывающих прямоугольников:
            if (m_svHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao.GetPos().y + ao.GetSize().y +fDltPxlsToChckHit || 
                m_svHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao.GetPos().x -fDltPxlsToChckHit  || 
                m_svHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao.GetPos().x + ao.GetSize().x +fDltPxlsToChckHit ||
                m_svHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao.GetPos().y -fDltPxlsToChckHit)
                continue;   //! Т.е., если нет коллизий описывающих прямоугольников (за вычетом зазора "- fDltPxlsToChckHit"), то к след. кирпичу.
	///@@@ #OPT: коль я уж сюда выбрасываюсь из предватительной проверки по ректам, то тут можно делать проверку только для 3 точек (макс. для 5 - т.е. по 1/2 шарика)
    ///.. д/ этого переделываю предвар.условия, проверяя обратные им и выставляя флажки. ... хотя ... х.е.з. получается, что проверять нужно тогда не 4 условия, а х2, т.к. нужно знать не факт пересечения линии, а попадания в диапазон, иначе все 4 условия будут срабатывать одновременно.
    
    ///@@@ с др.стороны, набор хит.чек.пнтов, кот. нужно проверять, зависит от текущего фи. ПОДУМАТЬ НАД ЭТИМ!
            //! Детальная проверка уже по хит-чек-поинтам (т.е. по точкам - датчикам столкновения)
            //! Перебираем датчики и если есть столкновение, записываем в вектор ..
            //!.. `rsltTrggrdHitChckPnts` имя сработавшего датчика (из enum _DIRECTION).
            //!.. Также при столкновении меняем значение свойства кирпича m_state из "ING" (in game) на "DSTR_ST" (destruction start).
            for (u32 i=0;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)
            {
                if ( /*ao->*/ao.Hit(m_svHitChckPnts[i] + Vec3To2(m_pBallPicObj->GetPos())/*,0*/) )  //! `m_svHitChckPnts[i]` - относительные ..
                {                                                                                   //!.. координаты датчика отн-но координат ..
                                                                                                    //!.. шарика (т.е. верхн.левого его угла).
                    rsltTrggrdHitChckPnts.push_back(static_cast<_DIRECTION>(i));                     
                    if (/*(*it)*/ (*(*pLavHndlr))[ib]/**/->m_state == _BRICK::ING)      // "==" comparison
                        /*(*it)*/ (*(*pLavHndlr))[ib]/**/->m_state = _BRICK::DSTR_ST;   // "="  assignment
                }
            }
        }
//#OUTDATED: #if LOCKS==1
#if DEBUG==1
            //if (pLavHndlr->GetReservedKey() == pLavHndlr->GetCurrValOfLockCode()) throw CMyExcptn(16); /*ВЫключаю замок на изменения в m_vpBricks.*/ ///@@@ прим-я не то
        if (pLavHndlr->GetReservedKey() != pLavHndlr->GetCurrValOfLockCode()) throw CMyExcptn(16);
        
        if (0 != lavBricks.ReleaseAccessHandler(pLavHndlr))     //! ВЫключаю замок на изменения в lavBricks.
            throw CMyExcptn(26);

//#OUTDATED:
//#else
//        CBrick::BricksLockOff(1)
#elif
        lavBricks.ReleaseAccessHandler(pLavHndlr);

#endif //DEBUG
//#OUTDATED: #endif //LOCKS
}


//Корректировка и интерпретация результатов
//#UGLY: попробовать уйти от такого длинного обращения к типу _DIRECTION может ч/з typedef - читал в теме nested classes, что typedef вроде должен быть виден и вовне. Ну в крайн.случ. попробовать использовать суффикс-синтаксис ("auto .. )-> ...")
_BALL::_DIRECTION  _BALL::InterpretIsHitResults(std::vector<_DIRECTION>& vTrggrdHitChckPnts) const
{
    if (vTrggrdHitChckPnts.empty()) return NO;	//нет коллизий

    _DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        ///#WRONG? : сам в примечаниях пишу, что сработать датчики могут в любой последовательности, а тут  " vTrggrdHitChckPnts[0]+1 "  - это чего ж именно "+" а не "-" 1 ??!
        ///if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[0]+1);	// Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
        
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	// Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
        {
            if (vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0] == 2)
                dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[0]+1);
            else //i.e. (vTrggrdHitChckPnts[0]-vTrggrdHitChckPnts[1] == 2)
                dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[1]+1);
        }
        // Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1й из 2х частн.случаев предыд.триггера
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2й из 2х частн.случаев предыд.триггера	//тут второй набор условия пока невозможен, но оставляю для возможных будущих нужд
#if DEBUG==1
        else 
        {
  #if HARD_DEBUG==1
            time_t currT = time(NULL);
            exceptPrintFile << asctime(localtime(&currT))<< "!!!: vTrggrdHitChckPnts.size()==2, but combination is unexpected:"
                "  [0]-"<< vTrggrdHitChckPnts[0]<< 
                ", [1]-"<< vTrggrdHitChckPnts[1]<< std::endl;
  #endif //HARD_DEBUG==1
            throw CMyExcptn(7);
        }
#endif //DEBUG==1
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2)
    {
  #if HARD_DEBUG==1
        time_t currT = time(NULL);
        exceptPrintFile << asctime(localtime(&currT))<< "!!!: vTrggrdHitChckPnts.size() > 2. Values combination is:"    ///@@@ #WRN: `asctime()` & `localtime()` are unsafe - use `asctime_s()` & `localtime_s()` instead (см.также use of this func. in other places in the PRO)
            "  [0]-"<< vTrggrdHitChckPnts[0]<< 
            ", [1]-"<< vTrggrdHitChckPnts[1]<< 
            ", [2]-"<< vTrggrdHitChckPnts[2]<< 
            ", [3]-"<< vTrggrdHitChckPnts[3]<< 
            ", [4]-"<< vTrggrdHitChckPnts[4]<< 
            ", [5]-"<< vTrggrdHitChckPnts[5]<< 
            ", [6]-"<< vTrggrdHitChckPnts[6]<< 
            ", [7]-"<< vTrggrdHitChckPnts[7]<< std::endl;
  #endif //HARD_DEBUG==1
        throw CMyExcptn(4);		//assert(vHP.size()<=2);
    }
#endif //DEBUG==1

    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//т.е. if единств.датчик сработал
    else throw CMyExcptn(11);	//Не нужно - проверка на .empty() была в начале

    return HitReview(dir);
}


//#UGLY: попробовать уйти от такого длинного обращения к типу _DIRECTION может ч/з typedef - читал в теме nested classes, что typedef вроде должен быть виден и вовне. Ну в крайн.случ. попробовать использовать суффикс-синтаксис ("auto .. )-> ...")
_BALL::_DIRECTION  _BALL::IsHit(
        //#XI: CPictureObject* const               pCurrBall,
        CPictureObject* const               pPad,
        /*const*/ CLockableAccessVec<_BRICK*>&  lavBricks,      // :Inp.
        const float                         fSceneW,
        CArkanoidController::GAME_STATE     gameState,
        const float                         fDltPxlsToChckHit /*=3*/  //Закомментировано знач-е by def., установленное в объявлении метода.
    )   
{																		///@@@ !!! ОПТИМИЗИРОВАТЬ !!!
    if (this == m_spIsAttachedToPad  ||  RUN != gameState) return NO;
    bool needToCheckBricks = 1;	//Ввёл эту булку для отсечения проверки кирпичей: Т.к. кирпичи проверяются последними, то, ..
                                //.. если выявлено столкновение с Pad'ом, то проверка кирпичей бессмысленна.
    std::vector<_DIRECTION> vTrggrdHitChckPnts;	//vec. of triggered ball hit check points 
                                                ///@@@Может после отладки сделать его сразу размерным (ск.вс. 2 достаточно бу) - НЕТ - он заполняется.

//Поиск столкновения с <^> краями экрана
    IsHitWithScreenBorders(fSceneW, vTrggrdHitChckPnts);

//Поиск столкновения с Pad'ом
    //#XI: IsHitWithPad(pPad, pPad->GetCenter().y - pCurrBall->GetCenter().y, fDltPxlsToChckHit, vTrggrdHitChckPnts, needToCheckBricks);
    IsHitWithPad(pPad, pPad->GetCenter().y - GetBallPicObj()->GetCenter().y, fDltPxlsToChckHit, vTrggrdHitChckPnts, needToCheckBricks);

//Поиск столкновения с кирпичами
    if (needToCheckBricks)	//см.пояснение выше - у описания этой переменной
        IsHitWithBricks(fDltPxlsToChckHit, lavBricks, vTrggrdHitChckPnts);

//Корректировка и интерпретация результатов
    return InterpretIsHitResults(vTrggrdHitChckPnts);

//#OLD (перенёс в `InterpretIsHitResults()`):
//    if (vTrggrdHitChckPnts.empty()) return NO;	//нет коллизий
//
//    _DIRECTION dir;
//    if (vTrggrdHitChckPnts.size()==2)
//    {
//        ///#WRONG? : сам в примечаниях пишу, что сработать датчики могут в любой последовательности, а тут  " vTrggrdHitChckPnts[0]+1 "  - это чего ж именно "+" а не "-" 1 ??!
//        ///if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[0]+1);	// Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
//        
//        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	// Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
//        {
//            if (vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0] == 2)
//                dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[0]+1);
//            else //i.e. (vTrggrdHitChckPnts[0]-vTrggrdHitChckPnts[1] == 2)
//                dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[1]+1);
//        }
//        else if ( (vTrggrdHitChckPnts[0]==N  && vTrggrdHitChckPnts[1]==W ) || (vTrggrdHitChckPnts[0]==W  && vTrggrdHitChckPnts[1]==N ) )  dir= NW;  //1й из 2х частн.случаев предыд.триггера
//        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) || (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) )  dir= N;   //2й из 2х частн.случаев предыд.триггера	//тут второй набор условия пока невозможен, но оставляю для возможных будущих нужд
//#if DEBUG==1
//        else throw CMyExcptn(7);
//#endif
//    }
//
//#if DEBUG==1
//    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
//#endif
//
//    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//т.е. if единств.датчик сработал
//
//#if DEBUG==1
//    else throw CMyExcptn(11);	//Не нужно - проверка на .empty() была в начале
//#endif
//
//    return HitReview(dir);
}


//Проверка на повторное определение уже обработанного контакта, пересчёт траектории по которому уже был произведён
//#UGLY: попробовать уйти от такого длинного обращения к типу _DIRECTION может ч/з typedef - читал в теме nested classes, что typedef вроде должен быть виден и вовне. Ну в крайн.случ. попробовать использовать суффикс-синтаксис ("auto .. )-> ...")
_BALL::_DIRECTION  _BALL::HitReview(_DIRECTION dir) const
{
#if DEBUG==1
    if (m_fFi>PI || m_fFi<= -PI) throw CMyExcptn(12);
#endif
    //! Далее отсекаю якобы контакты, когда в векторе направления движения шарика нет составляющей, ..
    //!.. направленной в сторону сработавшего датчика. Т.е. это либо ложное срабатывание датчика, ..
    //!.. когда шарик уже отскочил после уже обработанного контакта, но не успел ещё отлететь ..
    //!.. достаточно далеко, чтобы соответствующий(-ие) датчик(-и) вышли из зоны срабатывания. ..
    //!.. Либо это срабатывание датчика без контакта - очень частный случай, когда шарик летит чётко ..
    //!.. параллельно касательной к поверхности своей окружности, проведённой через точку этого ..
    //!.. датчика (т.е. что-то вроде "чирка" но без ударения - поверхности максимум скользнули одна ..
    //!.. по другой).
    switch (dir)
    {
        case N:  if (m_fFi>= PI/2 || m_fFi<=-PI/2) dir=NO; break;
        case NE: if (m_fFi>= PI*3/4 || m_fFi<=-PI/4) dir=NO; break;
        case E:  if (m_fFi == PI || m_fFi<= 0) dir=NO; break;
        case SE: if (m_fFi>= -PI*3/4 && m_fFi<= PI/4) dir=NO; break;
        case S:  if (m_fFi>= -PI/2 && m_fFi<= PI/2) dir=NO; break;
        case SW: if (m_fFi>= -PI/4 && m_fFi<= PI*3/4) dir=NO; break;
        case W:  if (m_fFi >= 0) dir=NO; break;
        case NW: if (m_fFi>= PI/4 || m_fFi<= -PI*3/4) dir=NO; break;
        default: throw CMyExcptn(13);
    }
    return dir;
}
    //xxx END_of class BALL xxx// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


//// InitFewPixelsShift() ////
void _BALL::InitFewPixelsShift()
{
    float currAng;
    for (u32 i=0; i < m_svHitChckPnts.size(); ++i)
    {
        currAng = m_svfBallHitChckPntsAngles[i]; //для лаконичности. Считаю, что можно, т.к. этот метод отрабатывает только однажды.
        m_smFewPixelsShift[static_cast<_DIRECTION>(i)] = vector2(   //#COMPILER(type_cast)
                                                            m_suNUM_PIXELS_4_SHIFT *-sin(currAng), 
                                                            m_suNUM_PIXELS_4_SHIFT * cos(currAng)
                                                        ); ///@@@ /*myround(*/ надо ли?
    }

    //#OUTDATED:
    //mFewPixelsShift [N]  = vector2( 0, 3);      // ( 0, 3)
    //mFewPixelsShift [NE] = vector2(-2, 2);      // (-2, 2)
    //mFewPixelsShift [E]  = vector2(-3, 0);      // (-3, 0)
    //mFewPixelsShift [SE] = vector2(-2,-2);      // (-2,-2)
    //mFewPixelsShift [S]  = vector2( 0,-3);      // ( 0,-3)
    //mFewPixelsShift [SW] = vector2( 2,-2);      // ( 2,-2)
    //mFewPixelsShift [W]  = vector2( 3, 0);      // ( 3, 0)
    //mFewPixelsShift [NW] = vector2( 2, 2);      // ( 2, 2)
}
