
#include "bricksManager.h"
#include "brick.h"
#include "myExcptn.h"
#include "obj.h"
#include "controller.h"



//// C-tor ////
CArkanoidController::CBricksManager::CBricksManager(const u32 numOfBricksOnLvl)
    :m_uNUM_OF_BRICKS_ON_LVL(numOfBricksOnLvl)
    //#OUTDATED:
    //, papBricksAniObjects(nullptr)
{

}



//// AddBrick() ////
void CArkanoidController::CBricksManager::AddBrick(CAniObject* baseBrick, vector2 position, CScene* sc)
{
    if (!m_vpBricks.empty())	//���� ��� ������ ������, �� ������ �� ������ ����� ����������, � ���� ��� �������, ������� �������� �� xml
    {
        m_vpBricks.push_back(new CBrick(new CAniObject(*baseBrick), position));
        sc->AddAni(m_vpBricks./*front()*/back()->GetBrickAniObj()/**/, 1/**/);	//�������� ���-������ � ����� 
        ///@@@ ���^ ��������� ������� iscopy - �����������, ��� ���
    }
    else
        m_vpBricks.push_back(new CBrick(baseBrick, position));
}



//// DelBrick() ////
///@@@ #TODO: ��.������� ����� - ����������� ����������
void CArkanoidController::CBricksManager::DelBrick(u32 i, CScene* sc)
{

#if LOCKS==1
    //�������: ������ ���� �� ������� �����
    while (CBrick::BricksLockOn(2) != 1)	//������� �����, ����� ����� �� ����������� ���� ����������� � ���������
    {

#if LOW_LVL_LOCKS==1
        //if ( CBrick::getBricksLockVal() == 2) break;	//�������: ������� ��� ����� ������ �������
        if ( CBrick::BricksLockOn(2) != 1) 
            return;
        else
            break;
#endif//LOW_LVL_LOCKS

    }
#endif//LOCKS

#if DEBUG==1
    if (m_vpBricks.size()<=i) throw CMyExcptn(14);
#endif

    if (CBrick::DSTR_ST == m_vpBricks[i]->m_state)
    {
        m_vpBricks[i]->GetBrickAniObj()->StartMovement(MV_SC01_BRICK);
        m_vpBricks[i]->m_state = CBrick::DSTR_EN;


        //�������: ��� ��� ����� ����� ������ ���� BricksLockOn() �������� ���� ����� �����. ����� �������, ������ ���.
#if LOCKS==1
#if DEBUG==1
        if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
#else
        CBrick::BricksLockOff(2)
#endif//DEBUG
#endif//LOCKS

        return;
    }
    if (m_vpBricks[i]->m_state == CBrick::DSTR_EN  &&  m_vpBricks[i]->GetBrickAniObj()->IsStopped())
    {
        m_vpBricks[i]->m_state = CBrick::OFF;	/*�������*/
//#if LOCKS==1
//      while (CBrick::BricksLockOn() != 1)	//������� �����, ����� ����� �� ����������� ���� ����������� � ���������
//      {
//      }
//#endif

#if DONT_DELETE_ANI_BRICKS==0
        //���� ��� �� ������� ������
        if (i!=0)
        {
            sc->RemoveAni(m_vpBricks[i]->GetBrickAniObj());	//������� ���-������ �� �����
            delete m_vpBricks[i]->GetBrickAniObj();
        }

        //���� ��� ������ (�������) ������, �� ����������� �������� - ������ 2 ������ ��� ���� ��������� ������, (�.�. ��������� ��� ����� - ����� �� ��������� �������� ����� ��� ��������) �� � �������� ��� �� �
        else
#endif//DONT_DELETE_ANI_BRICKS
        {
            m_vpBricks[i]->GetBrickAniObj()->SetPos(-300,-300); //������ �� �������� ����
            m_vpBricks[i]->GetBrickAniObj()->SetStatic(/*ANI_SC01_BRICK*/ST_SC01_BRICK_BEGIN);
            m_vpBricks[i]->m_state = CBrick::OFF;
        }
		
        //������ ��� ����������� ��� ������ �������
        delete m_vpBricks[i];

        m_vpBricks.erase(m_vpBricks.begin()+i);	//������� - �������� ������ ���� ������ �������� ����� ��������� � ��������� ��� 1�� ��-��� - �������� �� ����
        ////�������: ����� ��������� ����� � ���������
        //if (m_vpBricks.size()>1)
        //{
        //  //CBrick *tmpB = *(m_vpBricks.begin()+i);
        //  //*(m_vpBricks.begin()+i)=*(m_vpBricks.end()-1);
        //  //*(m_vpBricks.end()-1) = tmpB;
        //  //m_vpBricks.erase(m_vpBricks.end()-1);
        //  CBrick *tmpB = *(m_vpBricks.begin()+i);
        //  *(m_vpBricks.begin()+i)=*(m_vpBricks.begin());
        //  *(m_vpBricks.begin()) = tmpB;
        //  m_vpBricks.erase(m_vpBricks.begin());
        //}

        m_uGameScore+=10;

        if (m_vpBricks.empty()  &&  m_GameState==RUN)
        {
            m_GameState=WON;
        }
//#if LOCKS==1
//#if DEBUG==1
//      if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//      CBrick::BricksLockOff(2)
//#endif
//#endif
    }

//�������: ������ ��� ������
#if LOCKS==1
#if DEBUG==1
        if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
#else
        CBrick::BricksLockOff(2)
#endif//DEBUG
#endif//LOCKS

}



//// InitOfBricksAniObjects() ////
void CArkanoidController::CBricksManager::InitOfBricksAniObjects()
{	
    //#OUTDATED: �.�. ���� .push_back() vector'�, �, �������, ����� ������ ��� m_vpBricks � vpBricksAniObjs ��� �� �����, �������� �� ���������
    //for (char i = 0; i < m_vpBricks.size(); ++i)
    //{
    //    vpBricksAniObjs[i] = m_vpBricks[i]->GetBrickAniObj();
    //}
    for (auto& it = m_vpBricks.begin(); it != m_vpBricks.end(); ++it)
    {
        vpBricksAniObjs.push_back( (*it)->GetBrickAniObj() );
    }
}

