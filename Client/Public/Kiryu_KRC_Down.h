/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRC_Down : public CBehaviorAnimation
{
public:
	enum BEHAVIOR_STATE
	{
		TURN, WALK, RUN, BUMP, BEHAVIOR_STATE_END
	};

	enum ANIM_STATE
	{
		ANIM_LOOP, ANIM_END
	};


private:
	CKiryu_KRC_Down();
	virtual ~CKiryu_KRC_Down() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Stop() override;
	virtual _bool Stopping() override;
	virtual void Reset() override;
	virtual void Setting_Value(void* pValue) override;

private:
	ANIM_STATE m_eAnimState = { ANIM_LOOP };
	_bool m_isStop = { false };

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END