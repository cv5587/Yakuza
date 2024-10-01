/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRS_Sway : public CBehaviorAnimation
{
private:
	CKiryu_KRS_Sway();
	virtual ~CKiryu_KRS_Sway() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END