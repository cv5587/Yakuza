/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRC_PickUp : public CBehaviorAnimation
{
public:
	struct KRC_PICK_UP_HIT_DESC {
		_uint iHitAnimIndex;
	};

	struct KRC_PICK_UP_DESC {
		_bool isLeft;			// 아이템이 왼쪽에있는지, 오른쪽에 있는지
		_uint iComboCount;		// 몇타째에 집었는지
	};

	enum ANIM_STATE
	{
		ANIM_START, ANIM_LOOP, ANIM_ONCE, ANIM_END
	};

	enum ATTACK_STATE 
	{
		NONE_ATTAK, ATTACK
	};

private:
	CKiryu_KRC_PickUp();
	virtual ~CKiryu_KRC_PickUp() = default;

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
	void Play_Hit();

	void Play_PickupCombo(_bool isLeft);

	void Move_KeyInput(const _float& fTimeDelta);
	void Attack_KeyInput(const _float& fTimeDelta);

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

private:
	ANIM_STATE	m_eAnimState = { ANIM_START };
	ATTACK_STATE m_eBehaviorState = { NONE_ATTAK };

	_bool		m_isStop = { false };
	_bool		m_isHit = { false };

	_int		m_iComboCount = { 0 };
	_int		m_iDirection = { -1 };		// F, B, L, R

	_bool		m_isShaked = { false };
};
END