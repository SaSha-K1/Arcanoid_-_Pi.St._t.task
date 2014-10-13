
#include "obj.h"            //call of Copy C-tor in AddBrick()
#include "scene.h"          //call of ->AddAni()
#include "controller.h" ///@@@try to get rid of this include. BUT it's included in .h file
#include "bricksManager.h"
#include "brick.h"          //call of C-tor in AddBrick()
#include "myExcptn.h"



//// C-tor ////
_BRICKS_MNGR::CBricksManager(const u32 numOfBricksOnLvl)
    :m_uNUM_OF_BRICKS_ON_LVL(numOfBricksOnLvl)
    //#OUTDATED:
    //, papBricksAniObjects(nullptr)
{

}


//// AddBrick() ////
void _BRICKS_MNGR::AddBrick(
        /**/const/**/ CAniObject* const baseBrick, 
        const vector2           position, 
        CScene* const           sc  //@@ ������� ��� `const`, ��� ���� ����� �������.������ AddAni()
    )
{
    std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(3);
    if (!(*pLavHndlr)->empty())	//#NOTE: ����-�� `*` � `->` ����������� �/ _LAV_HANDLER    //���� ��� ������ ������, �� ������ �� ������ ����� ����������, � ���� ��� �������, ������� �������� �� xml
    {
        (*pLavHndlr)->push_back(new _BRICK(new CAniObject(*baseBrick), position));
        sc->AddAni((*pLavHndlr)->back()->GetBrickAniObj()/**/, 1/**/);	//�������� ���-������ � ����� 
        ///@@@ ���^ ��������� ������� iscopy - �����������, ��� ���. ���� �������� �������� ����� � ���-��� "0"
    }
    else
        (*pLavHndlr)->push_back(new _BRICK(baseBrick, position));   ///@@@ � 1� ������ ���� � ����� �� ���������? �� ��� ��� ���������� �������� �� xml? ��� �� ��� ��� �� ������� � �����?? (90%,��� 1� ���.)
}



//// DelBrick() ////
///@@@ #TODO: ��.������� ����� - ����������� ����������
void _BRICKS_MNGR::DelBrick(
            const u32                       i, 
            CScene* const                   sc,
            CArkanoidController* const      pArkCntrllr,
            std::auto_ptr<_LAV_HANDLER>*    ppLavHndlr /*= nullptr*/     //���������������� ����-� by def., ������������� � ���������� ������.     //@@��� �� ������ `const`'�, �.�. � ��������� ������������� � ������� �������.������ ��� ��������������� ptr'�.
        )   ///@@@ #UGLY: �������� - ���� ����� �������� shared_ptr, ����� �� �������� ��� ������� ������� �������������. (���� auto_ptr, �������� �������� �/� ptr �� ���� auto_ptr, �.�. ������, ����������� `nullptr`, ����� �� ����� ������������, � ���� �������� � �-��� auto_ptr �� ��������, �� ����������� �������� ������. � ������� ����. ������� ����� ��-���� �� ������: ��� ����� ��������, �� ������� ��������� � ������ ��� ���� ������, � ������� ���������� ����-� ������-���������, ���� �� 0)
{
//#OUTDATED (���������� ������ �����, ��� �����-��� LOW_LVL_LOCKS �������� �����):
//#if LOCKS==1
//    //�������: ������� ���� �� ������� �����
//    while (CBrick::BricksLockOn(2) != 1)	//������� �����, ����� ����� �� ����������� ���� ����������� � ���������
//    {
//
//#if LOW_LVL_LOCKS==1
//        //if ( CBrick::getBricksLockVal() == 2) break;	//�������: ������� ��� ����� ������ �������
//        if ( CBrick::BricksLockOn(2) != 1) 
//            return;
//        else
//            break;
//#endif //LOW_LVL_LOCKS
//
//    }
//#endif //LOCKS
    if (nullptr == ppLavHndlr)
//        std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(4);
        ppLavHndlr = &(m_LavBricks.CreateAccessHandler(4));

#if DEBUG==1
    if ( (*(*ppLavHndlr))->size() <= i ) throw CMyExcptn(14);   //#NOTE: ����-�� `*` � `->` ����������� �/ _LAV_HANDLER
#endif

    if ( _BRICK::DSTR_ST == (*(*(*ppLavHndlr)))[i]->m_state )   ///@@@����� ��� ����� ���: "/*_BRICK::*/DSTR_ST ..."
    //#NOTE: 
    //���   1� (*..) - �� ptr'� �� `auto_ptr` ��������� � `auto_ptr`
    //      2� (*..) - �� `auto_ptr` ��������� � _LAV_HANDLER
    //      3� (*..) - �.�. � _LAV_HANDLER ����-�� `*` � `->` �����������, �� ��� �� _LAV_HANDLER ��������� ����� � std::vector<_BRICK*> ..
    //.. ptr, �� ������� ���� member data ������.
    //      �� � [i] - ��� ��������� � ��-�� ������� ���� _BRICK*
    ///.. �������, ��� �����!
    {
        (*(*(*ppLavHndlr)))[i]->GetBrickAniObj()->StartMovement(MV_SC01_BRICK);
        (*(*(*ppLavHndlr)))[i]->m_state = _BRICK::DSTR_EN;

//#OUTDATED (���������� ������ �����, throw excptn if != reserved key, ����� � D-tor):
//        //�������: ��� ��� ����� ����� ������ ���� BricksLockOn() �������� ���� ����� �����. ����� ��������, ������ ���.
//#if LOCKS==1
//#if DEBUG==1
//        if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//        CBrick::BricksLockOff(2)
//#endif //DEBUG
//#endif //LOCKS

        return;
    }
    if ( _BRICK::DSTR_EN == (*(*(*ppLavHndlr)))[i]->m_state  &&  (*(*(*ppLavHndlr)))[i]->GetBrickAniObj()->IsStopped() )
    {
        (*(*(*ppLavHndlr)))[i]->m_state = _BRICK::OFF;	/*�������*/
//#if LOCKS==1
//      while (CBrick::BricksLockOn() != 1)	//������� �����, ����� ����� �� ����������� ���� ����������� � ���������
//      {
//      }
//#endif

#if DONT_DELETE_ANI_BRICKS==0
        //���� ��� �� ������� ������
        if (i!=0)
        {
            sc->RemoveAni((*(*(*ppLavHndlr)))[i]->GetBrickAniObj());	//������� ���-������ �� �����
            delete (*(*(*ppLavHndlr)))[i]->GetBrickAniObj();
        }

        //���� ��� ������ (�������) ������, �� ����������� �������� - ������ 2 ������ ��� ���� ��������� ������, (�.�. ��������� ��� ����� - ����� �� ��������� �������� ����� ��� ��������) �� � �������� ��� �� �
        else
#endif //DONT_DELETE_ANI_BRICKS
        {
            (*(*(*ppLavHndlr)))[i]->GetBrickAniObj()->SetPos(-300,-300); //������ �� �������� ����
            (*(*(*ppLavHndlr)))[i]->GetBrickAniObj()->SetStatic(/*ANI_SC01_BRICK*/ST_SC01_BRICK_BEGIN);     //@@? ��� � ����� ������� ��� �� �������������� ����� �� ����������
            (*(*(*ppLavHndlr)))[i]->m_state = _BRICK::OFF;
        }
		
        //������ ��� ����������� ��� ������ �������
        delete (*(*(*ppLavHndlr)))[i];

        (*(*ppLavHndlr))->erase((*(*ppLavHndlr))->begin()+i);	//������� - �������� ������ ���� ������ �������� ����� ��������� � ��������� ��� 1�� ��-��� - �������� �� ����
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

        pArkCntrllr->m_uGameScore +=10;

        if ((*(*ppLavHndlr))->empty()  &&  pArkCntrllr->m_GameState==RUN)
        {
            pArkCntrllr->m_GameState=WON;
        }
//#if LOCKS==1
//#if DEBUG==1
//      if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//      CBrick::BricksLockOff(2)
//#endif
//#endif
    }

//#OUTDATED (���������� ������ �����, throw excptn if != reserved key, ����� � D-tor):
////�������: ������� ��� ������
//#if LOCKS==1
//#if DEBUG==1
//        if (CBrick::BricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//        CBrick::BricksLockOff(2)
//#endif //DEBUG
//#endif //LOCKS

}


//// CleanBricks() ////
void  _BRICKS_MNGR::CleanBricks(
        CScene* const                   sc,
        CArkanoidController* const      pArkCntrllr
    )
{
    //@@#BACKUP (if ���-�� �������� GetLavHndlr(), �� � ��� �������� ������ ���� -->):  std::auto_ptr<_LAV_HANDLER> pLavHndlr = GetLavHndlr(7);
    std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(7);
    for (char i=(*pLavHndlr)->size()-1;  i>=0;  --i)
    ///@@@ #TODO: ������ ����������� ���� "���-�� ���������� � ���������", ������� ���-�� ����� ��������� ������� ��� ���������� ��������, ����� �������� ��� �� ���� �������� ��� �� ������.
    ///@@@ #TODO: ����� ����� � ���-�� ����� ����� ���� ����� ���������� ������ �������, ������� ����� ��������� - ����� ��������� ��� �� ��������.     //����� �������� ������������ ��� ���� ����� ������� Observer
    
	if ( (*(*pLavHndlr))[i]->m_state == /*_BRICK*/CBrick::DSTR_ST || (*(*pLavHndlr))[i]->m_state == _BRICK/*CBrick*/::DSTR_EN )
		DelBrick(i, sc, pArkCntrllr, &pLavHndlr);
}


//// RetrieveBrick() ////
void  _BRICKS_MNGR::RetrieveBrick(/*std::vector<*/ const std::vector<vector2>* /*>*/ const  /*vpvBricksOnLvlsCoords*/pvBricksOnCurrLvlCoords)
{
    std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(1);
#if DEBUG==1
    if (nullptr == pLavHndlr.get()) throw CMyExcptn(21);
#endif //DEBUG
    for (u32 i = 0;  i < m_vpBricksAniObjs.size();  ++i)
        (*pLavHndlr)->push_back( /**/(/**/ new /*CBricksManager::CBrick*/_BRICK(m_vpBricksAniObjs[i] /**/)/**/, /*(*vpvBricksOnLvlsCoords[0])*/(*pvBricksOnCurrLvlCoords)[i]));   ///@@@ ������ ��� ptr �� CLavHandler. ������� �������� ���������� m_vpBricks.push_back(... ����� ����-� -> ������ ����-�� �����.
}   ///@@@ ����������� ������ ������ () ������ `new`. ����� ��� �� �����, �� ����-� �������� //���������: oper-r `new` � `delete` ����� > ���������, ��� ����-� "," (�� ��� "," - separator, � �� oper-r) 


//// GetLavHndlr() ////
std::auto_ptr<_LAV_HANDLER>  _BRICKS_MNGR::GetLavHndlr(const u32 key)
{
    return m_LavBricks.CreateAccessHandler(key);
}


#if DONT_DELETE_ANI_BRICKS==1
//// InitOfBricksAniObjects() ////
void _BRICKS_MNGR::InitOfBricksAniObjects()
{	
    //#OUTDATED: �.�. ���� .push_back() vector'�, �, �������, ����� ������ ��� m_vpBricks � m_vpBricksAniObjs ��� �� �����, �������� �� ���������
    //for (char i = 0; i < m_vpBricks.size(); ++i)
    //{
    //    m_vpBricksAniObjs[i] = m_vpBricks[i]->GetBrickAniObj();
    //}
    std::auto_ptr<_LAV_HANDLER> pLavHndlr = m_LavBricks.CreateAccessHandler(5);
    for (auto& it = (*pLavHndlr)->begin(); it != (*pLavHndlr)->end(); ++it)
    {
        m_vpBricksAniObjs.push_back( (*it)->GetBrickAniObj() );     ///@@@��� ������ �� ������ ���, ��� �������� �� ������������ ������������� ����-� `->`. �������� �������� ��� ���� ������ � ��������������
    }
}
#endif //DONT_DELETE_ANI_BRICKS


//// StartAddBricks() ////
void _BRICKS_MNGR::StartAddBricks(
        CScene*                 sc, 
        std::vector<vector2>*   pvBricksOnLvlCoords
    )
{
        AddBrick(sc->FindAniObject(ANI_SC01_BRICK), (*pvBricksOnLvlCoords)[0], sc);
        //#OUTDATED: ��������� ���� ��� ���������
        //for (u32 ibr=1; ibr < m_uNUM_OF_BRICKS_ON_LVL1; ++ibr)	//frm "1", �.�. 0� ������ ������ �������� - �� �������
        //{
        //    addBrick(m_vpBricks[0]->getBrickAniObj(), m_aBricksCoordsLvl1[ibr], sc);
        //}
        for (auto it = pvBricksOnLvlCoords->begin()+1;  it != pvBricksOnLvlCoords->end(); ++it)	//frm "1", �.�. 0� ������ ������ �������� - �� �������  //ITERATOR using
        {   ///@@@ ���^^^ ���.����� [0]. ����������!
            AddBrick(/*m_pBricksMngr->m_vpBricks[0]->GetBrickAniObj()*/sc->FindAniObject(ANI_SC01_BRICK),  *it,  sc);    ///@@@ #TODO: �����������, ������ �������� �� "ptr �� �������� ���" ���-�, � 1�� ���-��� `m_pBricksMngr->...`
        }
        /*����� ������� �������� ��� ������ �� ���.���-�� 12 � ��������� �����. ���������*/
        //������ �������� ��� ����� ��������� �������� � xml � ��������� �� ���� ��� ���������� ���������� �� ������.
}


///@@@ At the end see list "#TODO" in file "PipeStudio Test task refactoring list.txt"