#pragma once

#include "Base.h"
#include "CineCamera.h"
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CEffectManager final : public CBase
{
	DECLARE_SINGLETON(CEffectManager)

private:
	CEffectManager();
	virtual ~CEffectManager() = default;

    //피파티클
public:
    //사람이 총맞았을떄 이펙트
    void Blood_Effect(CEffect::EFFECT_DESC& EffectDesc);
    //자동차, 오토바이에 총맞을때 나오는 스파크
    void Car_HitSpark(CEffect::EFFECT_DESC& EffectDesc);
    //차 터지고 나서 불
    void Car_Fire(CEffect::EFFECT_DESC& EffectDesc);
    //차 터질때
    void Car_Explosion(CEffect::EFFECT_DESC& EffectDesc);
    //총쏠떄
    void Shot_Flash(CEffect::EFFECT_DESC& EffectDesc);
    //시네마 초반 코피 터지기
    void Cine_NoseBlood(CEffect::EFFECT_DESC& EffectDesc);
    //시네마 입에서 피토
    void Cine_MouseBlood(CEffect::EFFECT_DESC& EffectDesc);
    //자동차 뒷불(트레일버퍼)
    void Car_BackTrail(CEffect::EFFECT_DESC& EffectDesc);
    //자동차 창문 깨짐
    void Car_GlassBroke(CEffect::EFFECT_DESC& EffectDesc);
    //돈
    void Money(CEffect::EFFECT_DESC& EffectDesc);
    //헬리콥터 불
    void Heli_Fire(CEffect::EFFECT_DESC& EffectDesc);
    //헬리콥터 폭파
    void Heli_Exp(CEffect::EFFECT_DESC& EffectDesc);
    //미사일폭팍
    void Heli_BulletExp(CEffect::EFFECT_DESC& EffectDesc);
    //파괴자 2타 이펙트
    void KRC_Hand(CEffect::EFFECT_DESC& EffectDesc);

    void Player_Attack_Effect(CEffect::EFFECT_DESC& EffectDesc);
    void Enemy_Attack_Effect(CEffect::EFFECT_DESC& EffectDesc);

    void Cine_BloodEffect(CEffect::EFFECT_DESC& EffectDesc, _uint iEffectType);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void							Free() override;
};
END
