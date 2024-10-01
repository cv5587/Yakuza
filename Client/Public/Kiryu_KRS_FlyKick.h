/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRS_FlyKick : public CBehaviorAnimation
{
private:
	CKiryu_KRS_FlyKick();
	virtual ~CKiryu_KRS_FlyKick() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;

private:
	void Shaking();

	void Radial_Control();

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;

private:
	_bool m_isShaked = { false };
};
END