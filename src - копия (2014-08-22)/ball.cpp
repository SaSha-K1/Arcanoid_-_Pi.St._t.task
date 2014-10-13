
#include "ball.h"


float Ball::fi_Plus=0;

Ball *Ball::isAttachedToPaddle_ = nullptr;

void Ball::computeNewFi (Direction dir)
{
	if (dir==N || dir==S)	//тут попарные сравнения, т.к. правила рассчёта fi одинаковы для этих случаев
	{
		fi_ = -fi_ + PI +fi_Plus;	//корректировка угла в зависимости от точки удара по Pad'у только для S датчика.
		fi_Plus=0;	//использовали бонус и обнулили.
	}
	else if (dir==NE || dir==SW)
		fi_ = -fi_ -PI/2.0;
	else if (dir==E || dir==W)
		fi_ = -fi_;
	else /*if (dir==SE || dir==NW)*/	//последнее условие можно и не проверять
#if DEBUG==1
		if (dir==SE || dir==NW)
#endif
			fi_ = -fi_ + PI/2;
#if DEBUG==1
		else throw MyExcptn(2);
#endif

	normalizeFi();
}


void Ball::normalizeFi ()
{
#if DEBUG==1
	if (fi_>=2*PI || fi_<=-2*PI) throw MyExcptn(3);
#endif
	if (fi_>PI)
		fi_=fi_-2*PI*(int( (fi_+PI)/(2*PI) ));		//посл.множитель - убираем столько раз по 2пи, чтобы рез-тат == (-PI ; PI].	//(fi_+PI) - тут +PI, чтобы сделать из 
	else if (fi_<=-PI)
		fi_=fi_+2*PI*(int( (fi_-PI)/(-2*PI) ));
}


vector2 Ball::computeNewMoveCenter ()
{
	vector2 mc;
	const u32 k = MOVE_C_COORD_SCALE;

	if (fi_ >=- PI/4  &&  fi_ <= PI/4) 	// ^ up
										mc.y = ballPicObj_->GetPos().y + k,
										mc.x = ballPicObj_->GetPos().x - k * tan(fi_);
	else if (( fi_ > -PI  &&  fi_ <= -3/4*PI )  ||  ( fi_ >= 3/4*PI  &&  fi_ <= PI )) 	// v down
										mc.y = ballPicObj_->GetPos().y - k,
										mc.x = ballPicObj_->GetPos().x + k * tan(PI+fi_);
	else if (fi_ > PI/4  &&  fi_ < 3/4*PI) 	// > right
										mc.x = ballPicObj_->GetPos().x - k,
										mc.y = ballPicObj_->GetPos().y - k * CTG(PI-fi_);		//тут и в след.ветке юзаю ctg, т.к. ч/з tg считать опасно в этих областях углов
	else if (fi_ > -3/4*PI  &&  fi_ < -PI/4) 	// < left
										mc.x = ballPicObj_->GetPos().x + k,
										mc.y = ballPicObj_->GetPos().y - k * CTG(fi_);
#if DEBUG==1
	else throw MyExcptn(1);
#endif
	return mc;
}


void Ball::computeStartFi ()
{
#if DEBUG==1
	if (isAttachedToPaddle_!=this)
		throw MyExcptn(6);
	else
#endif
	{
		fi_ = (float)CRndGen::GetInst().Generate(static_cast<u32>(2*RAND_START_ANGLE*MULTIPL_FOR_GEN)) 
			/ MULTIPL_FOR_GEN - RAND_START_ANGLE;
	}
}


vector2 Ball::getNewMoveCenter (Direction dir)
{
#if DEBUG==1
	if (dir==NO) throw MyExcptn(5);
#endif

	if (isAttachedToPaddle_ != this)		
	{
		ballPicObj_->StopMoveTo();	///@@@ ск.вс. не нужно
		computeNewFi (dir);
	}
	else /*if (isAttachedToPaddle_ == this)*/ computeStartFi();
	return computeNewMoveCenter();
}


bool Ball::tryToInvertIsAttachedToPaddle() 
{ 
	if (isAttachedToPaddle_ == this)	{ isAttachedToPaddle_ = nullptr; return 1; }
	else if (!isAttachedToPaddle_)		{ isAttachedToPaddle_ = this; return 1; }
	else /*if other ball is attached*/	return 0; 
}


void Ball::locateAttachedToPadBall()
{
#if DEBUG==1
	if (isAttachedToPaddle_!=this) throw MyExcptn(9);
#endif
	ballPicObj_->SetPos ( Pad->GetCenter().x - ballPicObj_->GetSize().x/2,   
						  paddleY - ballPicObj_->GetSize().y-1 );
}


Direction Ball::isHit ()
{																		///@@@ !!! ОПТИМИЗИРОВАТЬ !!!
	if(isAttachedToPaddle_==this || gameState!=RUN) return NO;
	bool needToCheckBricks=1;	//Ввёл эту булку для отсечения проверки кирпичей: Т.к. кирпичи проверяются последними, то, если выявлено столкновение с Pad'ом, то проверка кирпичей бессмысленна

	std::vector<Direction> vHP;	//может после отладки сделать его сразу размерным (ск.вс. 2 достаточно бу) - НЕТ - он заполняется
//Поиск столкновения с <^> краями экрана
	if(hitPnts_[N].y + ballPicObj_->GetPos().y <= 0) vHP.push_back(N);
	if(hitPnts_[W].x + ballPicObj_->GetPos().x <= 0) vHP.push_back(W);
	else if(hitPnts_[E].x + ballPicObj_->GetPos().x >= width) vHP.push_back(E);

	short dlt=5;	//без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. успевает пролететь разу на несколько пикселей между итерациями просчёта оптимизирующих (см. "opt") проверок

//Поиск столкновения с Pad'ом
	if ( hitPnts_[S].y + ballPicObj_->GetPos().y  <  paddleY-dlt  ||	//Opt.: если шарик выше горизонтали Pad'а, или если улетел уже ниже (горизонтали Pad'a +10) то коллизию с Pad'ом искать не нужно - начинаем перебор акт.объектов начиная со 2го (Pad всегда 1ый)
		 hitPnts_[S].y + ballPicObj_->GetPos().y  >  paddleY+10  ||								//opt
		 hitPnts_[E].x + ballPicObj_->GetPos().x  <  Pad->GetPos().x -dlt  ||					//opt
		 hitPnts_[W].x + ballPicObj_->GetPos().x  >  Pad->GetPos().x + Pad->GetSize().x +dlt )	//opt
		 /*то ничего*/;
	else
	{
		for (u32 i=2; i<=6; ++i)	// диапазон [2, 6] == [E, W] (остальные точки проверять для Pad'а не нужно)
		{
			if ( Pad->Hit(hitPnts_[i]+vec3To2(ballPicObj_->GetPos())/*,0*/) )
			{
				vHP.push_back(static_cast<Direction>(i));
				needToCheckBricks=0;
				
				//изменение угла отражения в зависимости от удаления точки контакта с Pad'ом от середины последнего
				if (vHP.back() = S)	//не корректирую угол, если касание по углу или боков.поверхности
					{
						float delta = ballPicObj_->GetCenter().x - Pad->GetCenter().x;	//степень отклонения шарика от центра Pad'а при ударе
						float weightedDelta = delta/Pad->GetSize().x/2;		//delta взвешенная по отношению к 1/2 длины Pad'а (т.е. получается в пределах [0, 1]
						fi_Plus= (PI/2/*4*/)*weightedDelta*((fabs(fi_)-PI/2)/PI/2);	//максимальная корректировка к углу == PI/4, корректируется взвешенной delta'ой и разницей fi_ и PI/2, выраженной в долях PI/2 (т.е. тоже [0, 1]). Для fi_ ==почти PI/2 (3,1417/2) угол отлёта всё равно получается < PI/2. Т.е. корректировка безопасная - шарик не полетит вниз из-за неё.
					}
			}
		}
	}
	
//Поиск столкновения с кирпичами
	if(needToCheckBricks)	//см.пояснение выше - у описания этой переменной
	{
#if LOCKS==1
		while (Brick::bricksLockOn(1) != 1)	///In isHit()
		{	/*включаю замок на изменения в bricks. Если уже включён кем-то другим, жду*/
#if LOW_LVL_LOCKS==1		
			//Костыли: вставляю временно для проверки
			if (Brick::bricksLockOn(1) != 1)
				return NO;
			else
				break;
#endif//LOW_LVL_LOCKS
		}
#endif//LOCKS

		//for (auto it = bricks.begin(); it!=bricks.end(); ++it)	//была проблема с инвалидным итератором из-за многопоточности
		//for (auto it = bricks.rbegin(); it!=bricks.rend(); ++it)	///@@@ использую ITERAOR N
		//for (char ib = bricks.size()-1; ib>=0; --ib)
		for (char ib = 0; ib < bricks.size(); ++ib)	//Костыли
		{		
			/*КОСТЫЛЬ*/ CAniObject ao = bricks[ib]->getBrickAniObj();	//Костыли: пробую сохранять себе копию объекта - не помогает
			//CAniObject *ao = (*it)->getBrickAniObj();	//для лаконичности
			//CAniObject ao (bricks[ib]->getBrickAniObj());	//для лаконичности
				//if (hitPnts_[S].y + ballPicObj_->GetPos().y < ao->GetPos().y -dlt  ||	//Opt.: проверка пересечения описывающих прямоугольников
				//hitPnts_[N].y + ballPicObj_->GetPos().y > ao->GetPos().y + ao->GetSize().y +dlt ||	//opt
				//hitPnts_[E].x + ballPicObj_->GetPos().x < ao->GetPos().x -dlt  ||					//opt
				//hitPnts_[W].x + ballPicObj_->GetPos().x > ao->GetPos().x + ao->GetSize().x +dlt)	//opt
			if (hitPnts_[S].y + ballPicObj_->GetPos().y < ao.GetPos().y -dlt  ||	//Opt.: проверка пересечения описывающих прямоугольников
				hitPnts_[N].y + ballPicObj_->GetPos().y > ao.GetPos().y + ao.GetSize().y +dlt ||	//opt
				hitPnts_[E].x + ballPicObj_->GetPos().x < ao.GetPos().x -dlt  ||					//opt
				hitPnts_[W].x + ballPicObj_->GetPos().x > ao.GetPos().x + ao.GetSize().x +dlt)	//opt
				continue;																			//opt
	
			for (u32 i=0; i<8; ++i)
			{
				if ( /*ao->*/ao.Hit(hitPnts_[i] + vec3To2(ballPicObj_->GetPos())/*,0*/) )
				{
					vHP.push_back(static_cast<Direction>(i));
					if(/*(*it)*/ bricks[ib]/**/->state == Brick::ING)
						/*(*it)*/ bricks[ib]/**/->state=Brick::DSTR_ST;
				}
			}
		}
#if LOCKS==1
#if DEBUG==1
		if(Brick::bricksLockOff(1)==0) throw MyExcptn(16); /*ВЫключаю замок на изменения в bricks. Если уже включён кем-то другим, жду*/
#else
		Brick::bricksLockOff(1)
#endif//DEBUG
#endif//LOCKS
	}

//Корректировка и интерпретация результатов
	if (vHP.empty()) return NO;	//нет коллизий

	Direction dir;
	if(vHP.size()==2)
	{
		if(abs(vHP[1]-vHP[0])==2)	dir= static_cast<Direction>(vHP[0]+1);	// Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
		else if( (vHP[0]==N && vHP[1]==W) || (vHP[0]==W && vHP[1]==N) )		dir= NW;			//1й из 2х частн.случаев предыд.триггера
		else if( (vHP[0]==NE && vHP[1]==NW) /**/|| (vHP[0]==NW && vHP[1]==NE) /**/)	dir= N;		//2й из 2х частн.случаев предыд.триггера	//тут второй набор условия пока невозможен, но оставляю для возможных будущих нужд
#if DEBUG==1
		else throw MyExcptn(7);
#endif
	}

#if DEBUG==1
	else if (vHP.size()>2) throw MyExcptn(4);		//assert(vHP.size()<=2);
#endif
	else /**/if(!vHP.empty())/**/ dir= vHP[0];	//т.е. if единств.датчик сработал
	else throw MyExcptn(11);	//Не нужно - проверка на .empty() была в начале

	return hitReview(dir);
}

//Проверка на повторное определение уже обработанного контакта, пересчёт траектории по которому уже был произведён
Direction Ball::hitReview(Direction dir)
{
#if DEBUG==1
	if (fi_>PI || fi_<= -PI) throw MyExcptn(12);
#endif
	switch (dir)
	{
		case N:  if (fi_>= PI/2 || fi_<=-PI/2) dir=NO; break;
		case NE: if (fi_>= PI*3/4 || fi_<=-PI/4) dir=NO; break;
		case E:  if (fi_ == PI || fi_<= 0) dir=NO; break;
		case SE: if (fi_>= -PI*3/4 && fi_<= PI/4) dir=NO; break;
		case S:  if (fi_>= -PI/2 && fi_<= PI/2) dir=NO; break;
		case SW: if (fi_>= -PI/4 && fi_<= PI*3/4) dir=NO; break;
		case W:  if (fi_ >= 0) dir=NO; break;
		case NW: if (fi_>= PI/4 || fi_<= -PI*3/4) dir=NO; break;
		default: throw MyExcptn(13);
	}
	return dir;
}
