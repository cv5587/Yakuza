#include "EffectManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffectManager)

CEffectManager::CEffectManager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CEffectManager::Blood_Effect(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit_Blood0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit_Blood1"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit_Blood2"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_HitSpark(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_CarHitSpark0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_CarHitSpark1"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_Fire(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_FireRoundC"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Car_Smog"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_Explosion(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_ExpA"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_FireRoundC"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Shot_Flash(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Shot_Exp"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Shot_Glow"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Cine_NoseBlood(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Luquid0"), TEXT("Layer_Particle"), &EffectDesc);
   // m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Blood1"), TEXT("Layer_Particle"), &EffectDesc);

}

void CEffectManager::Cine_MouseBlood(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Blood6"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_BackTrail(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Trail_CarBackLight"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Car_GlassBroke(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Mesh_glass0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Mesh_glass1"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Mesh_glass2"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Money(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Mesh_Money"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Coin"), TEXT("Layer_Particle"), &EffectDesc);

    m_pGameInstance->PlaySound_W(TEXT("Money.wav"), SOUND_EFFECT);
    m_pGameInstance->PlaySound_W(TEXT("Coin_Get.wav"), SOUND_EFFECT_SUB);
    m_pGameInstance->PlaySound_W(TEXT("Coin.wav"), SOUND_EFFECT_THIRD);
}

void CEffectManager::Heli_Fire(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_ExpA"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_HeliFireC"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Heli_Exp(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_Heli_ExpA"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Heli_BulletExp(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Aura_ExpA"), TEXT("Layer_Particle"), &EffectDesc);

}

void CEffectManager::KRC_Hand(CEffect::EFFECT_DESC& EffectDesc)
{
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_KuzeHand"), TEXT("Layer_Particle"), &EffectDesc);

}

void CEffectManager::Player_Attack_Effect(CEffect::EFFECT_DESC& EffectDesc)
{
    // 플레이어 타격 이펙트 (== 몬스터 피격 이펙트)
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Glow0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part2"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part3"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part4"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Distortion0"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Enemy_Attack_Effect(CEffect::EFFECT_DESC& EffectDesc)
{    
    // 몬스터 타격 이펙트 (== 플레이어 피격 이펙트)
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part0"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part1"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part2"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part3"), TEXT("Layer_Particle"), &EffectDesc);
    m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Glow0"), TEXT("Layer_Particle"), &EffectDesc);
}

void CEffectManager::Cine_BloodEffect(CEffect::EFFECT_DESC& EffectDesc, _uint iEffectType)
{
    // 그 외의 번호는 필요  시 작성해서 쓸 것
    switch(iEffectType)
    {
    case 0:         //입(단발)
        Cine_NoseBlood(EffectDesc);
        break;
    case 1:         //입(루프_)
        Cine_MouseBlood(EffectDesc);
        break;
    case 2:         //충격파
        m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_23070_Glow"), TEXT("Layer_Particle"), &EffectDesc);
        m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_23070_Distotion"), TEXT("Layer_Particle"), &EffectDesc);
        break;
    case 3://먼지
        m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Smog"), TEXT("Layer_Particle"), &EffectDesc);
        break;
    case 4:
        m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Splash"), TEXT("Layer_Particle"), &EffectDesc);
        break;

    }
}

void CEffectManager::Free()
{
    Safe_Release(m_pGameInstance);
}