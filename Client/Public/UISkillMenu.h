#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUISkillMenu :
    public CUIScene
{
protected:
    CUISkillMenu();
    CUISkillMenu(const CUISkillMenu& rhs);
    virtual ~CUISkillMenu() = default;
public:
    virtual HRESULT Show_Scene()override;//ui 애님 준비(초기화/열때 정방향 진행)
    virtual HRESULT Close_Scene()override;//ui 애님 준비(초기화/닫을떄 반대로진행)

    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;
public:
    static CUISkillMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};

END