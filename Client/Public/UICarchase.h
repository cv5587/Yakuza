#pragma once
#include "UIScene.h"

#include "Client_Defines.h"
BEGIN(Client)
class CUICarchase :
    public CUIScene
{
    enum TYPE { AIM, TARGETING, HPBAR_BACK, HPBAR, INFO,  TYPE_END };
    enum INFO {PLAYERHP , HITEYE , CARHP , AMMO , INFO_END};
    typedef struct tCarchaseUIDesc {
        class CUI_Object* TargetingUI;
        class CUI_Object* HPBarBackUI;
        class CUI_Object* HPBarUI;
        class CCarChase_Monster* pMonsterAddr;
    } CARCCHASE_UI_DESC;
    enum RELOAD { Back = 8, TEXT, RELOAD_END };
protected:
    CUICarchase();
    CUICarchase(const CUICarchase& rhs);
    virtual ~CUICarchase() = default;

public:
    HRESULT Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP);
    HRESULT Remove_Target(_uint iIndex);
    class CCarChase_Monster* Get_Target();

    virtual HRESULT Show_Scene()override;//ui 애님 준비(초기화/열때 정방향 진행)
public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;

public:
    HRESULT Add_TargetingRound(_uint iIndex, class CCarChase_Monster* pMonster, _bool isBullet);
    HRESULT Update_Player();
private:
    map<_uint, CARCCHASE_UI_DESC> m_Targets; // 타겟팅 UI 저장

    _float m_fAimSpeed = { 300.f };
    _uint KiryuType = { 0 };

    CARCCHASE_UI_DESC* m_CurrentTarget = { nullptr };

private:
    void Move_Aim(const _float& fTimeDelta);
    CARCCHASE_UI_DESC* Find_TargetUI(_uint iIndex);
    void Coll_Aim();

public:
    static CUICarchase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
