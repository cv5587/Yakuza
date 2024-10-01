/*
* 24.06.28 혜원 추가
* 캐릭터 애니메이션 객체화 상위클래스
*/
#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CBehaviorAnimation : public CBase
{
protected:
	CBehaviorAnimation();
	virtual ~CBehaviorAnimation() = default;

public:
	virtual void Tick(const _float& fTimeDelta) = 0;
	virtual void Change_Animation() = 0;
	virtual _bool Get_AnimationEnd() { return false; };
	virtual void Stop() {};
	virtual _bool Stopping() { return false; };
	virtual void Reset() {};
	virtual void Combo_Count(_bool isFinAction = false) {};
	virtual void Setting_Value(void* pValue = nullptr) {};
	virtual string Get_AnimationName() { return string(); };
	virtual _uint Get_AnimationIndex() { return m_AnimationIndices[m_iCurrentIndex]; };
	virtual _bool Checked_Animation_Ratio(_float fRatio);
	virtual void Event(void* pValue = nullptr) {}

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	vector<_uint> m_AnimationIndices;
	_uint m_iCurrentIndex = { 0 };

public:
	// ADVENTURE: 0, KRS: 1, KRH: 2, KRC: 3
	static CBehaviorAnimation* Create(_uint iStyle, _uint iBehavior, class CPlayer* pPlayer);
	static CBehaviorAnimation* Create_Adventure_Behavior(_uint iBehavior, class CPlayer* pPlayer);
	static CBehaviorAnimation* Create_KRS_Behavior(_uint iBehavior, class CPlayer* pPlayer);
	static CBehaviorAnimation* Create_KRH_Behavior(_uint iBehavior, class CPlayer* pPlayer);
	static CBehaviorAnimation* Create_KRC_Behavior(_uint iBehavior, class CPlayer* pPlayer);

	virtual void Free() override;
};

END