/*
* 24.07.02 혜원 추가
* 키류 불한당 애니메이션 아이들
*/
#pragma once
#include "Client_Defines.h"
#include "BehaviorAnimation.h"

BEGIN(Client)

class CKiryu_KRS_Hit : public CBehaviorAnimation
{
public:
	struct KRS_Hit_DESC
	{
		const _float3* pDir;			// 때리는 방향(주먹이나 발의 이동방향), 파티클에서 사용
		_float fDamage;
		string strAnimationName;		// 상대방의 애니메이션에 따른 히트모션을 실행하기 위함
		_uint iDirection;				// 때리는 상대방의 방향
	};

private:
	CKiryu_KRS_Hit();
	virtual ~CKiryu_KRS_Hit() = default;

public:
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Change_Animation() override;
	virtual _bool Get_AnimationEnd() override;
	virtual void Reset();
	virtual void Setting_Value(void* pValue = nullptr);
	virtual string Get_AnimationName();
	virtual _uint Get_AnimationIndex();

private:
	vector<string> m_AnimationNames;

public:
	static CBehaviorAnimation* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};
END