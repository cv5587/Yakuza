/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 시작 모션
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRS_BattleStart : public CBehaviorAnimation
{
	enum ANIM_STATE
	{
		ANIM_START, ANIM_LOOP, ANIM_END
	};

private:
	const _float ANIM_CHANGE_TIME = 4.f;		// 몇 초 이상 움직이거나, 공격하지않으면 손을 내린다

private:
	CKiryu_KRS_BattleStart();
	virtual ~CKiryu_KRS_BattleStart() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Reset();

private:
	void Off_Battle(const _float& fTimeDelta);

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

	ANIM_STATE m_eAnimState = { ANIM_START };
	_bool m_isStop = { false };
	_float m_fOffBattleTimer = { 0.f };
};
END