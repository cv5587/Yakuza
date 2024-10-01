/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRS_Attack : public CBehaviorAnimation
{
private:
	CKiryu_KRS_Attack();
	virtual ~CKiryu_KRS_Attack() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Reset();
	virtual void Combo_Count(_bool isFinAction = false) override;

	_bool IsFinishBlow();

private:
	_bool Changeable_Combo_Animation();
	void Shaking();

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

private:
	_int m_iComboCount = { -1 };
	_bool m_isShaked = { false };

	_bool m_isFreezeEventOn = { false };
};
END