/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRS_Down : public CBehaviorAnimation
{
public:
	struct KRS_DOWN_DESC
	{
		_int iDownState;		// -1 안쓰겠다, 0 B(뒤), 1 F(앞)
		_int iDirection;		// 대상의 위치가 어디인지 (존재하는 반대로 굴러야함), -1 값이라면 히트용이 아니다
		string strAnimationName;
	};

	enum ANIM_STATE
	{
		ANIM_LOOP, ANIM_END, ANIM_ONCE
	};


private:
	CKiryu_KRS_Down();
	virtual ~CKiryu_KRS_Down() = default;

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
	_bool m_isFront = { false };

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END