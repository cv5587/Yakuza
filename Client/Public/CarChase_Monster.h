#pragma once
#include "Monster.h"

BEGIN(Client)
class CCarChase_Monster abstract:
    public CMonster
{
public:
    typedef struct tCarChaseMonsterDesc : public CMonster::MONSTER_IODESC {
        const _float4x4* pParentMatrix;
        const _float4x4* pBoneMatrix;
        _uint iWeaponType;
        _uint iLineDir; // 위, 옆, 뒤
        _uint iStageDir; // 위, 왼, 오, 앞
    } CARCHASE_MONSTER_DESC;

public:
    enum WEAPON_TYPE { DRV, RKT, GUN, WPJ, WPR, GAT, GUN_R, GUN_L, WEAPON_END };
    enum REACTOR_TYPE { REACTOR_VAN, REACTOR_SEDAN, REACTOR_BIKE, REACTOR_HELI, REACTOR_TYPE_END };
    enum CARCHASE_STATE {
        CARCHASE_CURVA_L,
        CARCHASE_CURVA_R,
        CARCHASE_SIT_DWN,
        CARCHASE_SIT_LP,
        CARCHASE_TENTOU_A,
        CARCHASE_TENTOU_B,
        CARCHASE_TENTOU_C,
        CARCHASE_AIM_F_LP,
        CARCHASE_AIM_L90_LP,
        CARCHASE_AIM_R90_LP,
        CARCHASE_AIM_EN,
        CARCHASE_AIM_L_LP,
        CARCHASE_AIM_LP,
        CARCHASE_AIM_R_LP,
        CARCHASE_AIM_SHOT,
        CARCHASE_AIM_SHOT_F,
        CARCHASE_AIM_SHOT_R,
        CARCHASE_AIM_SHOT_L,
        CARCHASE_AIM_SHOT_L90,
        CARCHASE_AIM_SHOT_R90,
        CARCHASE_AIM_ST,
        CARCHASE_DAM_LV_2,
        CARCHASE_DED,
        CARCHASE_DED_L,
        CARCHASE_DED_R,
        CARCHASE_RELOAD,
        CARCHASE_SHOT_F_EN,
        CARCHASE_SHOT_F_LP,
        CARCHASE_SHOT_F_ST,
        CARCHASE_SHOT_L_LP,
        CARCHASE_SHOT_LP,
        CARCHASE_SHOT_R_LP,
        CARCHASE_SHOT_ST,
        CARCHASE_BATTOU,
        CARCHASE_NOUTOU,
        CARCHASE_STAND_UP,

        //CAR
        CARCHASE_AIML_CURVE_L,
        CARCHASE_AIML_CURVE_R,
        CARCHASE_AIML_DAM,
        CARCHASE_AIML_DED,
        CARCHASE_AIML_DWN,
        CARCHASE_AIML_HI,
        CARCHASE_AIML_L45,
        CARCHASE_AIML_LP,
        CARCHASE_AIML_SHOT,
        CARCHASE_AIML_STD_LP,
        CARCHASE_AIML_BTOL,
        CARCHASE_AIML_FTOL,
        CARCHASE_AIML_LTOB,
        CARCHASE_AIML_LTOF,
        CARCHASE_AIMR_CURVE_L,
        CARCHASE_AIMR_CURVE_R,
        CARCHASE_AIMR_DAM,
        CARCHASE_AIMR_DED,
        CARCHASE_AIMR_DWN,
        CARCHASE_AIMR_HI,
        CARCHASE_AIMR_L45,
        CARCHASE_AIMR_LP,
        CARCHASE_AIMR_SHOT,
        CARCHASE_AIMR_STD_LP,
        CARCHASE_AIMR_BTOL,
        CARCHASE_AIMR_FTOL,
        CARCHASE_AIMR_LTOB,
        CARCHASE_AIMR_LTOF,
        CARCHASE_SITL_EN,
        CARCHASE_SITL_LP,
        CARCHASE_SITL_ST,
        CARCHASE_SITR_EN,
        CARCHASE_SITR_LP,
        CARCHASE_SITR_ST,

        CARCAHSE_SYNC1_AIMLTO_R,
        CARCAHSE_SYNC1_AIMLTO_L,
        CARCAHSE_SYNC_AIMLTO_L,
        CARCAHSE_SYNC_AIMLTO_R,
        CARCHASE_END
    };

public:
    _uint Get_LineDir() { return m_iLineDir; } //몬스터의 위치 (앞, 옆, 뒤)
    const _float4x4* Get_ModelMatrix() { return &m_ModelWorldMatrix; }

public:
    virtual void Set_Coll();

protected:
    CCarChase_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCarChase_Monster(const CCarChase_Monster& rhs);
    virtual ~CCarChase_Monster() = default;

public://몬스터가 들고잇는 무기 타입에 따라서 모션이 달라짐.
    _uint Get_WeaponType() { return m_iWeaponType; }
    _bool isReactorDead() { return m_isReactorDead; }

public:
    void Set_ReactorDead(_bool isReactorDead);

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

protected:
    _uint m_iWeaponType = { WEAPON_END };
    _uint m_iDir = { DIR_END }; // 몬스터가 바라봐야 하는 방향
    _uint m_iLineDir = { DIR_END };
    _uint m_iStageDir = { DIR_END };
    _uint m_iCurrentAnimType = { CLandObject::ANIM_TYPE_END };

protected:
    _bool m_isReactorDead = { false }; // 자동차가 전복되는 경우

protected:
    class CUIManager* m_pUIManager = { nullptr };
    class CAI_CarChase* m_pTree = { nullptr };

    const _float4x4* m_pParentMatrix = { nullptr }; 
    const _float4x4* m_pParentBoneMatrix = { nullptr }; // 부착할 부모의 루트뼈
    const _float4x4* m_pTargetBoneMatrix = { nullptr }; // UI 부착할 몬스터 뼈
    _float4x4 m_pWorldMatrix;

protected: 
    virtual void Change_Animation();
    virtual void Update_TargetingUI();
    void Get_LookDir();
    _uint DirFromPlayerPos();

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;
    virtual void Set_ParentMatrix(const _float& fTimeDelta);

public:
    virtual void Free();
};
END
