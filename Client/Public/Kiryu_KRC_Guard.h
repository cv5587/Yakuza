/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRC_Guard : public CBehaviorAnimation
{
public:
	enum ANIM_STATE
	{
		ANIM_START, ANIM_LOOP, ANIM_END
	};

	enum STATE_TYPE
	{
		GUARD, HIT, COUNTER_ATTACK, RUSH,
	};


private:
	CKiryu_KRC_Guard();
	virtual ~CKiryu_KRC_Guard() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Stop() override;
	virtual _bool Stopping() override;
	virtual void Reset() override;
	virtual void Setting_Value(void* pValue = nullptr) override;

private:
	void Shaking();

private:
	_uint m_iIndex = { 0 };
	STATE_TYPE m_eCurrentType = GUARD;

	ANIM_STATE m_eAnimState = { ANIM_START };
	_bool m_isStop = { false };

	_bool m_isShaked = { false };

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END