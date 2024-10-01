/*
* 24.06.28 혜원 추가
* 키류 애니메이션 어드밴처 아이들
*/
#pragma once
#include "BehaviorAnimation.h"
#include "Client_Defines.h"

BEGIN(Client)

class CKiryu_Adventure_Walk : public CBehaviorAnimation
{
public:
	enum ANIM_STATE
	{
		ANIM_LOOP, ANIM_END		
	};

private:
	CKiryu_Adventure_Walk();
	virtual ~CKiryu_Adventure_Walk() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Stop() override;

private:
	ANIM_STATE m_eAnimState = { ANIM_LOOP };
	_bool m_isStop = { false };
	
public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END