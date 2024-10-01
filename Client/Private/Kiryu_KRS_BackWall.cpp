#include "GameInstance.h"
#include "Kiryu_KRS_BackWall.h"
#include "Player.h"

CKiryu_KRS_BackWall::CKiryu_KRS_BackWall()
	:CBehaviorAnimation{}
{
	m_AnimationIndices.push_back(567);	//[567]	p_krs_style_st[p_krs_style_st]
	m_AnimationIndices.push_back(565);	//[565]	p_krs_stand_btl_lp[p_krs_stand_btl_lp]
	m_AnimationIndices.push_back(564);	//[564]	p_krs_stand_btl_en[p_krs_stand_btl_en]
}

void CKiryu_KRS_BackWall::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRS_BackWall::Change_Animation()
{
}

_bool CKiryu_KRS_BackWall::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		return true;
	}

	return false;
}

void CKiryu_KRS_BackWall::Reset()
{
	m_fOffBattleTimer = 0.f;
}

CBehaviorAnimation* CKiryu_KRS_BackWall::Create(CPlayer* pPlayer)
{
	CKiryu_KRS_BackWall* pInstnace = new CKiryu_KRS_BackWall();

	if (nullptr == pInstnace) 
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRS_BattleStart");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRS_BackWall::Free()
{
	__super::Free();
}