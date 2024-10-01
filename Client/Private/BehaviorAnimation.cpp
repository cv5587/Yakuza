#include "BehaviorAnimation.h"
#include "GameInstance.h"

#include "Player.h"

#pragma region Adventure
#include "Kiryu_Adventure_Idle.h"
#include "Kiryu_Adventure_Walk.h"
#include "Kiryu_Adventure_Run.h"
#pragma endregion

#pragma region KRS
#include "Kiryu_KRS_BattleStart.h"
#include "Kiryu_KRS_Idle.h"
#include "Kiryu_KRS_Walk.h"
#include "Kiryu_KRS_Run.h"
#include "Kiryu_KRS_Attack.h"
#include "Kiryu_KRS_FlyKick.h"
#include "Kiryu_KRS_KickCombo.h"
#include "Kiryu_KRS_Hit.h"
#include "Kiryu_KRS_Sway.h"
#include "Kiryu_KRS_Down.h"
#include "Kiryu_KRS_Grab.h"
#include "Kiryu_KRS_PickUp.h"
#pragma endregion

#pragma region KRH
#include "Kiryu_KRH_BattleStart.h"
#include "Kiryu_KRH_Idle.h"
#include "Kiryu_KRH_Walk.h"
#include "Kiryu_KRH_Run.h"
#include "Kiryu_KRH_Attack.h"
#include "Kiryu_KRH_Hit.h"
#include "Kiryu_KRH_Sway.h"
#include "Kiryu_KRH_Down.h"
#pragma endregion

#pragma region KRC
#include "Kiryu_KRC_BattleStart.h"
#include "Kiryu_KRC_Idle.h"
#include "Kiryu_KRC_Walk.h"
#include "Kiryu_KRC_Run.h"
#include "Kiryu_KRC_Attack.h"
#include "Kiryu_KRC_Hit.h"
#include "Kiryu_KRC_Sway.h"
#include "Kiryu_KRC_Down.h"
#include "Kiryu_KRC_Grab.h"
#include "Kiryu_KRC_Guard.h"
#include "Kiryu_KRC_PickUp.h"
#pragma endregion

CBehaviorAnimation::CBehaviorAnimation()
	: m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

CBehaviorAnimation* CBehaviorAnimation::Create_Adventure_Behavior(_uint iBehavior, class CPlayer* pPlayer)
{
	// IDLE, WALK, RUN, ADVENTURE_BEHAVIOR_END
	switch (iBehavior)
	{
	case 0:
		return CKiryu_Adventure_Idle::Create(pPlayer);
	case 1:
		return CKiryu_Adventure_Walk::Create(pPlayer);
	case 2:
		return CKiryu_Adventure_Run::Create(pPlayer);
	}

	return nullptr;
}

CBehaviorAnimation* CBehaviorAnimation::Create_KRS_Behavior(_uint iBehavior, class CPlayer* pPlayer)
{
/*
        BTL_START, IDLE, WALK, RUN, ATTACK, HIT, 
        SWAY, DOWN, 
		SKILL_FLY_KICK, SKILL_KICK_COMBO,
*/
	CPlayer::KRS_BEHAVIOR_STATE eBehavior = static_cast<CPlayer::KRS_BEHAVIOR_STATE>(iBehavior);

	switch (eBehavior)
	{
	case CPlayer::KRS_BEHAVIOR_STATE::BTL_START:
		return CKiryu_KRS_BattleStart::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::IDLE:
		return CKiryu_KRS_Idle::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::WALK:
		return CKiryu_KRS_Walk::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::RUN:
		return CKiryu_KRS_Run::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::ATTACK:
		return CKiryu_KRS_Attack::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::HIT:
		return CKiryu_KRS_Hit::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::SWAY:
		return CKiryu_KRS_Sway::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::DOWN:
		return CKiryu_KRS_Down::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::GRAB:
		return CKiryu_KRS_Grab::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::PICK_UP:
		return CKiryu_KRS_PickUp::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::SKILL_FLY_KICK:
		return CKiryu_KRS_FlyKick::Create(pPlayer);
	case CPlayer::KRS_BEHAVIOR_STATE::SKILL_KICK_COMBO:
		return CKiryu_KRS_KickCombo::Create(pPlayer);
	}

	return nullptr;
}

CBehaviorAnimation* CBehaviorAnimation::Create_KRH_Behavior(_uint iBehavior, class CPlayer* pPlayer)
{
	/*
		BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
		SWAY, DOWN, KRS_BEHAVIOR_END
	*/

	CPlayer::KRH_BEHAVIOR_STATE eBehavior = static_cast<CPlayer::KRH_BEHAVIOR_STATE>(iBehavior);

	switch (eBehavior)
	{
	case CPlayer::KRH_BEHAVIOR_STATE::BTL_START:
		return CKiryu_KRH_BattleStart::Create(pPlayer);
	case CPlayer::KRH_BEHAVIOR_STATE::IDLE:
		return CKiryu_KRH_Idle::Create(pPlayer);
	case CPlayer::KRH_BEHAVIOR_STATE::WALK:
		return CKiryu_KRH_Walk::Create(pPlayer);
	case CPlayer::KRH_BEHAVIOR_STATE::RUN:
		return CKiryu_KRH_Run::Create(pPlayer);
	case CPlayer::KRH_BEHAVIOR_STATE::ATTACK:
		return CKiryu_KRH_Attack::Create(pPlayer);
	case CPlayer::KRH_BEHAVIOR_STATE::HIT:
		return CKiryu_KRH_Hit::Create(pPlayer);
	case CPlayer::KRH_BEHAVIOR_STATE::SWAY:
		return CKiryu_KRH_Sway::Create(pPlayer);
	case CPlayer::KRH_BEHAVIOR_STATE::DOWN:
		return CKiryu_KRH_Down::Create(pPlayer);
	}
	
	return nullptr;
}

CBehaviorAnimation* CBehaviorAnimation::Create_KRC_Behavior(_uint iBehavior, class CPlayer* pPlayer)
{
	/*
		BTL_START, IDLE, WALK, RUN, ATTACK, HIT,
		SWAY, DOWN, KRS_BEHAVIOR_END
	*/

	CPlayer::KRC_BEHAVIOR_STATE eBehavior = static_cast<CPlayer::KRC_BEHAVIOR_STATE>(iBehavior);

	switch (eBehavior)
	{
	case CPlayer::KRC_BEHAVIOR_STATE::BTL_START:
		return CKiryu_KRC_BattleStart::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::IDLE:
		return CKiryu_KRC_Idle::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::WALK:
		return CKiryu_KRC_Walk::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::RUN:
		return CKiryu_KRC_Run::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::ATTACK:
		return CKiryu_KRC_Attack::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::HIT:
		return CKiryu_KRC_Hit::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::SWAY:
		return CKiryu_KRC_Sway::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::DOWN:
		return CKiryu_KRC_Down::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::GRAB:
		return CKiryu_KRC_Grab::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::PICK_UP:
		return CKiryu_KRC_PickUp::Create(pPlayer);
	case CPlayer::KRC_BEHAVIOR_STATE::GUARD:
		return CKiryu_KRC_Guard::Create(pPlayer);
	}

	return nullptr;
}

_bool CBehaviorAnimation::Checked_Animation_Ratio(_float fRatio)
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (fRatio < *pModelCom->Get_AnimationCurrentPosition() / *pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

CBehaviorAnimation* CBehaviorAnimation::Create(_uint iStyle, _uint iBehavior, class CPlayer* pPlayer)
{
	CBehaviorAnimation* pInstance = { nullptr };
	
	// ADVENTURE: 0, KRS: 1, KRH: 2, KRC: 3
	switch (iStyle)
	{
	case 0:
		pInstance = Create_Adventure_Behavior(iBehavior, pPlayer);
		break;
	case 1:
		pInstance = Create_KRS_Behavior(iBehavior, pPlayer);
		break;
	case 2:
		pInstance = Create_KRH_Behavior(iBehavior, pPlayer);
		break;
	case 3:
		pInstance = Create_KRC_Behavior(iBehavior, pPlayer);
		break;
	}

	return pInstance;
}

void CBehaviorAnimation::Free()
{
	Safe_Release(m_pGameInstance);
}
