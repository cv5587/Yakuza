/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRC_Grab : public CBehaviorAnimation
{
public:
	struct KRC_Grab_DESC
	{
		_bool	isGrabed;
		_int	iDirection;
	};

	enum ANIM_STATE
	{
		SEIZE_TRY, ANIM_START, ANIM_LOOP, ANIM_ONCE, ANIM_END
	};

private:
	CKiryu_KRC_Grab();
	virtual ~CKiryu_KRC_Grab() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Reset();
	virtual void Combo_Count(_bool isFinAction = false) override;
	virtual void Stop() override;
	virtual void Setting_Value(void* pValue = nullptr) override;
	virtual void Event(void* pValue = nullptr) override;

private:
	_bool Changeable_Combo_Animation();
	void Shaking();
	void Play_Off();

	void Move_KeyInput(const _float& fTimeDelta);

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

private:
	ANIM_STATE	m_eAnimState = { SEIZE_TRY };
	_bool		m_isStop = { false };

	_int		m_iComboCount = { -1 };
	_int		m_iDirection = { -1 };		// F, B, L, R

	_bool		m_isGrabed = { false };
	_bool		m_isShaked = { false };
};
END