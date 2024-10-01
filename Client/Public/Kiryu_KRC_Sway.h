/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRC_Sway : public CBehaviorAnimation
{
private:
	CKiryu_KRC_Sway();
	virtual ~CKiryu_KRC_Sway() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Reset();
	virtual void Combo_Count(_bool isFinAction = false) override;

private:
	_bool Changeable_Combo_Animation();

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

private:
	_int m_iComboCount = { 0 };
	_uint m_iDirectionIndex = { 0 };
};
END