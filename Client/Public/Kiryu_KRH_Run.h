/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRH_Run : public CBehaviorAnimation
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
	CKiryu_KRH_Run();
	virtual ~CKiryu_KRH_Run() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Stop() override;
	virtual void Reset() override;
	virtual _bool Stopping() override;

private:
	_bool Changeable_Combo_Animation();

private:
	ANIM_STATE m_eAnimState = { ANIM_LOOP };
	_bool m_isStop = { false };

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END