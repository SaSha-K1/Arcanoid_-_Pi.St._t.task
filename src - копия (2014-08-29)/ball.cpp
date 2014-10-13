
#include "ball.h"
#include "myExcptn.h"


//Static members:
//#OUTDATED (Не пойму, зачем ..FiPlus делал static'ом):     float   CArkanoidController::CBallsManager::CBall::m_sfFiPlus = 0;
CArkanoidController::CBallsManager::CBall*  CArkanoidController::CBallsManager::CBall::m_spIsAttachedToPad = nullptr;
const u32                                   CArkanoidController::CBallsManager::CBall::m_suNUM_PIXELS_4_SHIFT = 3;
const float                                 CArkanoidController::CBallsManager::CBall::m_fDEFAULT_BALL_VEL = 200;
const u32                                   CArkanoidController::CBallsManager::CBall::m_suNUM_OF_HIT_CHCK_PNTS = 8;    //сейчас ==8 : (N(north),E(east),S,W + 4 промежуточных). !В случае изменения, увязать с новым знач-ем enum DIRECTION и (возможно) вектор m_vfBallHitChckPntsAngles.
std::vector<float>                          CArkanoidController::CBallsManager::CBall::m_vfBallHitChckPntsAngles;
std::vector<vector2>                        CArkanoidController::CBallsManager::CBall::m_vHitChckPnts;

CArkanoidController::CBallsManager::CBall::CBall(
        CPictureObject* const pBall, 
        //#OUTDATED (ввожу static const'анту в класс СБол):     const u32 numOfHitChckPnts,
        //#OUTDATED: std::array<vector2,/*uNumOfHitChckPnts*/0U>&  aBallHitChckPnts,     ///#WARNING: сработает ли "0U" ..?
        const bool              bAttachToPad
    ) 
    : m_pBallPicObj(pBall), 
    m_fVelocity(m_fDEFAULT_BALL_VEL), 
    m_fFi(0)
    //#OUTDATED (ввожу static const'анту в класс СБол):     m_uNumOfHitChckPnts(numOfHitChckPnts),
    //#DEL_IT?: m_fBallR(pBallPicObj->GetSize().y/2.0)
{ 
    if (m_vHitChckPnts.empty())
    {
        InitBallHitChckPntsAnglesVec();
        InitHitChckPntsVec();
    }
    if (bAttachToPad)
    {
        m_spIsAttachedToPad = this; 
        LocateAttachedToPadBall();	//хотя, если двинуть Pad, то шарик позиционируется и без этого, Но для фен-шуя 
    }
}

//////////////////////\ PREPARE_ZONE \\\\\\\\\\\\\\\\\\\\\/
void CArkanoidController::CBallsManager::CBall::InitBallHitChckPntsAnglesVec()
{
    float newChckPnt;
    m_vfBallHitChckPntsAngles.push_back(0.0);
    for (u32 i=1;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)  //
    {
        newChckPnt = 2*PI*i/m_suNUM_OF_HIT_CHCK_PNTS;
        if (PI < newChckPnt)    //Конвертирую угол их диапазона [0; 2*PI) в диапазон (-PI; PI].     //Тут можно было использовать и NormalizeFi(), если переписать её, чтобы принимала параметр, но действие простое, а в NormalizeFi() больше проверок и действий. Решил, что это лишнее усложнение
            newChckPnt -= 2*PI;
        m_vfBallHitChckPntsAngles.push_back(newChckPnt);
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


void CArkanoidController::CBallsManager::CBall::InitHitChckPntsVec()
{
    float ballR = m_pBallPicObj->GetSize().y/2.0;
    float currAng;
    //#OUTDATED:    for (u32 i=0;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)    // aBallHitChckPnts.size(); ++i)
    for (auto& it = m_vfBallHitChckPntsAngles.begin();  m_vfBallHitChckPntsAngles.end() != it;  ++it)
    {
        //#OUTDATED:    currAng = m_vfBallHitChckPntsAngles[i]; //для лаконичности. Считаю, что можно, т.к. этот метод отрабатывает только однажды.

        //Использую выведенные формулы определения растровых x и y из уравнения окружности шарика:
        //Последнее условное слагаемое (1 или 0) нужно: т.к. тут имеем дело с растровым изображением, то, ..
        //.. коль нам нужен первый пиксель вне контура шарика, то в, случае поиска координат такой точки ..
        //.. _в_ направлении оси координат, прибавляем 1-цу; в обратном направлении - отнимаем.

        //#OUTDATED ():
        //aBallHitChckPnts[i] = vector2(
        ///@@ STOPED HERE (28/08/14)
        m_vHitChckPnts.push_back(
            vector2(
                (ballR-1) + ballR * sin(*it) 
                    +((*it > 0   &&  *it < PI) ? 1 : 0),    ///@@@ /*myround(*/ надо ли?
                (ballR-1) - ballR * cos(*it) 
                    +(( (*it > -PI  &&  *it < -HALFPI)  ||  (*it > HALFPI  /*&&  currAng <= PI*/) ) ? 1 : 0)
                    //Тут закомментированная часть условия не нужна, если угол не может быть больше PI (как сейчас)
            )
        );
    }
}
//X//X//X//X//X//X//X\ PREPARE_ZONE \\X\\X\\X\\X\\X\\X\\X/


void CArkanoidController::CBallsManager::CBall::ComputeNewFi(const DIRECTION dir)
{
    if (N==dir || S==dir)	//тут попарные сравнения, т.к. правила рассчёта fi одинаковы для этих случаев
    {
        m_fFi = -m_fFi + PI +m_sfFiPlus;	//корректировка угла в зависимости от точки удара по Pad'у только для S датчика.
        m_sfFiPlus=0;	//использовали бонус и обнулили.
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


void CArkanoidController::CBallsManager::CBall::NormalizeFi()
{
#if DEBUG==1
    if (2*PI <= m_fFi  ||  -2*PI >= m_fFi) throw CMyExcptn(3);
#endif
    if (PI < m_fFi)
        m_fFi=m_fFi-2*PI*(int( (m_fFi+PI)/(2*PI) ));		//Посл.множитель - убираем столько раз по 2пи, чтобы ..
                                                            //.. рез-тат == (-PI ; PI].     //(m_fFi+PI) - тут +PI, чтобы ближайшее ..
                                                            //.. меньшее нечётное целое число PI в составе m_fFi при делении нацело ..
                                                            //.. на 2*PI давало +1. Это нужно,чтобы вписать m_fFi в диапазон (-PI;PI].
    else if (-PI >= m_fFi)
        m_fFi=m_fFi+2*PI*(int( (m_fFi-PI)/(-2*PI) ));
}


vector2 CArkanoidController::CBallsManager::CBall::ComputeNewMoveCenter(u32 moveCCoordScale/* = pow(10.0, 3) */) const  //Закомментированы значения by def., ..
                                                                                   //.. установленные в объявлении метода.
                                                                                  //Пар-тр `moveCCoordScale` - const множитель ..
                                                                                 //.. координаты центра движения (по-сути может ..
                                                                                //.. быть другим числом, главное, чтобы был дост. ..
                                                                               //.. далёк от единицы). Иначе может черезчур ..
                                                                              //.. увеличиться значение второй координаты. ..
{
    vector2 mc;
    //#OUTDATED: const u32 k = MOVE_C_COORD_SCALE;

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


void CArkanoidController::CBallsManager::CBall::ComputeStartFi(
        const float rndStartAng,/* = 10*(float)PI/180, */   //Закомментированы значения by def., установленные в объявлении метода.
        const u32 multiplForGen/* = pow(10.0, 4) */         //Закомментированы значения by def., установленные в объявлении метода.
     )    
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad)
        throw CMyExcptn(6);
    else
#endif
    {
        m_fFi = (float)CRndGen::GetInst().Generate(static_cast<u32>(2 * rndStartAng * multiplForGen)) 
            / multiplForGen - rndStartAng;
    }
}


vector2 CArkanoidController::CBallsManager::CBall::GetNewMoveCenter(const DIRECTION dir)
{
#if DEBUG==1
    if (NO==dir) throw CMyExcptn(5);
#endif

    if (this != m_spIsAttachedToPad)		
    {
        m_pBallPicObj->StopMoveTo();	///@@@ ск.вс. не нужно
        computeNewFi(dir);
    }
    else /*if (this == m_spIsAttachedToPad)*/ computeStartFi();
    return computeNewMoveCenter();
}


bool CArkanoidController::CBallsManager::CBall::TryToInvertIsAttachedToPad() 
{ 
    if (this == m_spIsAttachedToPad)	        { m_spIsAttachedToPad = nullptr; return 1; }
    else if (m_spIsAttachedToPad == nullptr)    { m_spIsAttachedToPad = this; return 1; }
    else /*if other ball is attached*/	return 0; 
}


void CArkanoidController::CBallsManager::CBall::LocateAttachedToPadBall()
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad) throw CMyExcptn(9);
#endif
    m_pBallPicObj->SetPos( g_pPad->GetCenter().x - m_pBallPicObj->GetSize().x / 2,   
                           g_fPaddleY - m_pBallPicObj->GetSize().y - 1 );
}

//Рассчёт поправки (m_sfFiPlus) к углу отражения (m_fFi) в зависимости от удаления точки контакта с Pad'ом от середины последнего
void CArkanoidController::CBallsManager::CBall::ComputeFiPlus()
{
    float dlt = m_pBallPicObj->GetCenter().x - g_pPad->GetCenter().x;	//Степень отклонения шарика от центра Pad'а при ударе
    float weightedDlt = dlt/g_pPad->GetSize().x/2; //delta взвешенная по отношению к 1/2 длины Pad'а (т.е. получается в пределах [0, 1]
    m_sfFiPlus= (PI/2/*4*/)*weightedDlt*((fabs(m_fFi)-PI/2)/PI/2);	//Максимальная корректировка к углу == PI/4, корректируется ..
                                                                   //.. взвешенной dlt'ой и разницей m_fFi и PI/2, выраженной в ..
                                                                  //.. долях PI/2 (т.е. тоже [0, 1]). Для m_fFi ==почти PI/2 ..
                                                                 //.. (3,1417/2) угол отлёта всё равно получается < PI/2. Т.е. ..
                                                                //.. корректировка безопасная - шарик не полетит вниз из-за неё.
}


void CArkanoidController::CBallsManager::CBall::IsHitWithScreenBorders(std::vector<DIRECTION>& vTrggrdHitChckPnts) const
{
    if (0  >=  m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y)     vTrggrdHitChckPnts.push_back(N);
    if (0  >=  m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x)     vTrggrdHitChckPnts.push_back(W);
    else if (m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x  >=  g_fSceneWidth)    vTrggrdHitChckPnts.push_back(E);
}


void CArkanoidController::CBallsManager::CBall::IsHitWithPad(
        std::vector<DIRECTION>& vTrggrdHitChckPnts,  
        bool& needToCheckBricks, 
        const float fMaxDltYWithPad,  
        const float& fDltPxlsToChckHit
     )
{
    //#OUTDATED: const float dlt = DLT_PXLS_TO_CHCK_HIT; //Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. успевает пролететь сразу ..
    //                                        //.. на несколько пикселей между итерациями просчёта оптимизирующих (см. "#OPT") проверок.

    //Оптимизация (далее #OPT): если шарик выше горизонтали Pad'а, или если улетел уже ниже (горизонтали Pad'a +10), ..
    //.. то коллизию с Pad'ом искать не нужно - начинаем перебор акт.объектов начиная со 2го (Pad всегда 1ый)
    if ( m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y  <  g_fPaddleY - fDltPxlsToChckHit  ||	
         m_pBallPicObj->GetPos().y  >  g_fPaddleY + fMaxDltYWithPad  ||   //if центр шарика ниже середины Pad'а,то уже game over.
         m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x  <  g_pPad->GetPos().x - fDltPxlsToChckHit  ||
         m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x  >  g_pPad->GetPos().x + g_pPad->GetSize().x + fDltPxlsToChckHit )
         /*То ничего. Рабочая, но менее вероятная, ветка - следующая*/;
    else
    {
        for (int d=E/*2*/; d<=W/*6*/; ++d)	// диапазон [2, 6] == [E, W] (остальные точки проверять для Pad'а не нужно)
        {                                   //#WARNING(possibly). Возможно заменить implicit преобразование типов явным.
            if ( g_pPad->Hit(m_aHitChckPnts[d] + Vec3To2(m_pBallPicObj->GetPos())/*,0*/) )
            {
                vTrggrdHitChckPnts.push_back(static_cast<DIRECTION>(d));
                needToCheckBricks=0;
                //Рассчёт поправки (m_sfFiPlus) к углу (m_fFi) в зависимости от удаления точки контакта с Pad'ом от середины Pad'а:
                if (S == vTrggrdHitChckPnts.back())	//не корректирую угол, если касание по углу или боков.поверхности
                    ComputeFiPlus();
            }
        }
    }
}


void CArkanoidController::CBallsManager::CBall::IsHitWithBricks(std::vector<DIRECTION>& vTrggrdHitChckPnts,  const float& fDltPxlsToChckHit)
{
#if LOCKS==1
        while (CBrick::BricksLockOn(1) != 1)	///In isHit()
        {	/*включаю замок на изменения в m_vpBricks. Если уже включён кем-то другим, жду*/
#if LOW_LVL_LOCKS==1		
            //Костыли: вставляю временно для проверки
            if (CBrick::BricksLockOn(1) != 1)
                return NO;
            else
                break;
#endif//LOW_LVL_LOCKS
        }
#endif//LOCKS

//#OUTDATED: const float dlt = DLT_PXLS_TO_CHCK_HIT; //Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. успевает пролететь сразу ..
//                                        //.. на несколько пикселей между итерациями просчёта оптимизирующих (см. "#OPT") проверок.

        //for (auto it = m_vpBricks.begin(); it!=m_vpBricks.end(); ++it)	//была проблема с инвалидным итератором из-за многопоточности
        //for (auto it = m_vpBricks.rbegin(); it!=m_vpBricks.rend(); ++it)	///@@@ использую ITERAOR N
        //for (char ib = m_vpBricks.size()-1; ib>=0; --ib)
        for (char ib = 0; ib < m_vpBricks.size(); ++ib)	//Костыли
        {		
            /*КОСТЫЛЬ*/ CAniObject ao = m_vpBricks[ib]->getBrickAniObj();	//Костыли: пробую сохранять себе копию объекта - не помогает
            //CAniObject *ao = (*it)->getBrickAniObj();	//для лаконичности
            //CAniObject ao (m_vpBricks[ib]->getBrickAniObj());	//для лаконичности
                //if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao->GetPos().y -dlt  ||	//Opt.: проверка пересечения описывающих прямоугольников
                //m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao->GetPos().y + ao->GetSize().y +dlt ||	//opt
                //m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao->GetPos().x -dlt  ||					//opt
                //m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao->GetPos().x + ao->GetSize().x +dlt)	//opt
            //#OPT: проверка пересечения описывающих прямоугольников:
            if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao.GetPos().y -fDltPxlsToChckHit  ||	
                m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao.GetPos().y + ao.GetSize().y +fDltPxlsToChckHit || 
                m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao.GetPos().x -fDltPxlsToChckHit  || 
                m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao.GetPos().x + ao.GetSize().x +fDltPxlsToChckHit) 
                continue;
	
            for (u32 i=0; i<uNumOfHitChckPnts; ++i)
            {
                if ( /*ao->*/ao.Hit(m_aHitChckPnts[i] + vec3To2(m_pBallPicObj->GetPos())/*,0*/) )
                {
                    vTrggrdHitChckPnts.push_back(static_cast<DIRECTION>(i));
                    if (/*(*it)*/ m_vpBricks[ib]/**/->m_state == CBrick::ING)
                        /*(*it)*/ m_vpBricks[ib]/**/->m_state=CBrick::DSTR_ST;
                }
            }
        }
#if LOCKS==1
#if DEBUG==1
        if (CBrick::BricksLockOff(1)==0) throw CMyExcptn(16); /*ВЫключаю замок на изменения в m_vpBricks. Если уже включён кем-то другим, жду*/
#else
        CBrick::BricksLockOff(1)
#endif//DEBUG
#endif//LOCKS
}


//Корректировка и интерпретация результатов
//#UGLY: попробовать уйти от такого длинного обращения к типу DIRECTION может ч/з typedef - читал в теме nested classes, что typedef вроде должен быть виден и вовне. Ну в крайн.случ. попробовать использовать суффикс-синтаксис ("auto .. )-> ...")
CArkanoidController::CBallsManager::CBall::DIRECTION CArkanoidController::CBallsManager::CBall::InterpretIsHitResults(std::vector<DIRECTION>& vTrggrdHitChckPnts) const
{
    if (vTrggrdHitChckPnts.empty()) return NO;	//нет коллизий

    DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<DIRECTION>(vTrggrdHitChckPnts[0]+1);	
        // Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1й из 2х частн.случаев предыд.триггера
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2й из 2х частн.случаев предыд.триггера	//тут второй набор условия пока невозможен, но оставляю для возможных будущих нужд
#if DEBUG==1
        else throw CMyExcptn(7);
#endif
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
#endif
    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//т.е. if единств.датчик сработал
    else throw CMyExcptn(11);	//Не нужно - проверка на .empty() была в начале

    return HitReview(dir);
}


//#UGLY: попробовать уйти от такого длинного обращения к типу DIRECTION может ч/з typedef - читал в теме nested classes, что typedef вроде должен быть виден и вовне. Ну в крайн.случ. попробовать использовать суффикс-синтаксис ("auto .. )-> ...")
CArkanoidController::CBallsManager::CBall::DIRECTION CArkanoidController::CBallsManager::CBall::IsHit(const float fDltPxlsToChckHit/* = 3*/)   //Закомментировано знач-е by def., установленное в объявлении метода.
{																		///@@@ !!! ОПТИМИЗИРОВАТЬ !!!
    if (this == m_spIsAttachedToPad  ||  RUN != m_GameState) return NO;
    bool needToCheckBricks=1;	//Ввёл эту булку для отсечения проверки кирпичей: Т.к. кирпичи проверяются последними, то, ..
                                //.. если выявлено столкновение с Pad'ом, то проверка кирпичей бессмысленна.
    std::vector<DIRECTION> vTrggrdHitChckPnts;	///@@@Может после отладки сделать его сразу размерным (ск.вс. 2 достаточно бу) - НЕТ - он заполняется.

//Поиск столкновения с <^> краями экрана
    IsHitWithScreenBorders(vTrggrdHitChckPnts);

//Поиск столкновения с Pad'ом
    IsHitWithPad(vTrggrdHitChckPnts, needToCheckBricks, g_pPad->GetCenter().y - g_pBall->GetCenter().y, fDltPxlsToChckHit);

//Поиск столкновения с кирпичами
    if (needToCheckBricks)	//см.пояснение выше - у описания этой переменной
        IsHitWithBricks(vTrggrdHitChckPnts, fDltPxlsToChckHit);

//Корректировка и интерпретация результатов
    InterpretIsHitResults(vTrggrdHitChckPnts);
    if (vTrggrdHitChckPnts.empty()) return NO;	//нет коллизий

    DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<DIRECTION>(vTrggrdHitChckPnts[0]+1);	// Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1й из 2х частн.случаев предыд.триггера
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2й из 2х частн.случаев предыд.триггера	//тут второй набор условия пока невозможен, но оставляю для возможных будущих нужд
#if DEBUG==1
        else throw CMyExcptn(7);
#endif
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
#endif
    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//т.е. if единств.датчик сработал
    else throw CMyExcptn(11);	//Не нужно - проверка на .empty() была в начале

    return HitReview(dir);
}

//Проверка на повторное определение уже обработанного контакта, пересчёт траектории по которому уже был произведён
//#UGLY: попробовать уйти от такого длинного обращения к типу DIRECTION может ч/з typedef - читал в теме nested classes, что typedef вроде должен быть виден и вовне. Ну в крайн.случ. попробовать использовать суффикс-синтаксис ("auto .. )-> ...")
CArkanoidController::CBallsManager::CBall::DIRECTION CArkanoidController::CBallsManager::CBall::HitReview(DIRECTION dir) const
{
#if DEBUG==1
    if (m_fFi>PI || m_fFi<= -PI) throw CMyExcptn(12);
#endif
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







void CArkanoidController::CBallsManager::CBall::InitFewPixelsShift()
{
    float currAng;
    for (u32 i=0; i < aBallHitChckPnts.size(); ++i)
    {
        currAng = aBallHitChckPntsAngles[i]; //для лаконичности. Считаю, что можно, т.к. этот метод отрабатывает только однажды.
        mFewPixelsShift[static_cast<DIRECTION>(i)] = vector2(   //#COMPILER(type_cast)
            NUM_PIXELS_4_SHIFT *-sin(currAng), 
            NUM_PIXELS_4_SHIFT * cos(currAng)
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
