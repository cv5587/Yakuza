/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRC_Attack : public CBehaviorAnimation
{
private:
	CKiryu_KRC_Attack();
	virtual ~CKiryu_KRC_Attack() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Reset();
	virtual void Combo_Count(_bool isFinAction = false) override;
	virtual void Setting_Value(void* pValue = nullptr) override;

	_bool IsFinishBlow();

public:
	_uint Get_ComboCount() {
		return m_iComboCount;
	}

private:
	_bool Changeable_Combo_Animation();

	void Shaking();

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

private:
	_int	m_iComboCount = { -1 };
	_bool	m_isFinished = { false };
	_bool	m_isBut = { false };
	_bool m_isShaked = { false };
};
END