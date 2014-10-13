#ifndef BALL_H
#define BALL_H


#include "pictureobj.h"
#include <array>					//��� std::array	

class Ball
{
public:
	Ball (CPictureObject *ball, std::array<vector2,8> &hitPnts, bool attachToPad) 
		: ballPicObj_(ball), velocity_(200), fi_(0), hitPnts_(hitPnts) 
	{ 
		if(attachToPad)
		{
			isAttachedToPaddle_=this; 
			locateAttachedToPadBall();	//����, ���� ������� Pad, �� ����� ��������������� � ��� �����, �� ��� ���-��� 
		}
	}
	static float fi_Plus; //������������� � ��������������� ����� ����� fi_ (���� ���� ��� �� Pad'�)
	bool tryToInvertIsAttachedToPaddle(); 
	void locateAttachedToPadBall();
	Direction isHit ();

	//������������ �-���:
	vector2 getNewMoveCenter (Direction dir);
	static Ball* getIsAttachedToPaddle() { return isAttachedToPaddle_; }
	CPictureObject* getBallPicObj() { return ballPicObj_; }
	float getVel() { return velocity_; }
	std::array<vector2,8>& getHitPnts() { return hitPnts_; }
#if DEBUG ==1
	float getFi() { return fi_; }
#endif

private:
    CPictureObject *ballPicObj_;
	static Ball *isAttachedToPaddle_;
	float velocity_;
	float fi_;		//���.����. ���� ����������� ����.������ == (-PI ; PI]. 0 �����-�� ����������� ����������������.
	std::array<vector2,8> &hitPnts_;

	void computeNewFi (Direction dir);
	void computeStartFi ();
	void normalizeFi ();
	vector2 computeNewMoveCenter ();
	Direction hitReview(Direction dir);
};


#endif//BALL_H