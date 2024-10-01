#include "UIFade.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"
#include"GameInstance.h"
CUIFade::CUIFade()
    :CUIScene{}
{
}

CUIFade::CUIFade(const CUIFade& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUIFade::Show_Scene()
{
    if (FAILED(__super::Show_Scene()))
        return E_FAIL;
    return S_OK;
}

HRESULT CUIFade::Close_Scene()
{
    if (FAILED(__super::Close_Scene()))
        return E_FAIL;
    m_isFinished = true;
    return S_OK;
}

HRESULT CUIFade::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
    if (pUIObject->Get_Event())
    {
        m_EventUI.push_back(pUIObject);

    }
    else
    {
        m_UI.push_back(pUIObject);
    }

    return S_OK;
}

HRESULT CUIFade::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;
    return S_OK;
}

HRESULT CUIFade::Tick(const _float& fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    for (auto& iter : m_EventUI)
        iter->Tick(fTimeDelta);

    return S_OK;
}

HRESULT CUIFade::Late_Tick(const _float& fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    for (auto& iter : m_EventUI)
        iter->Late_Tick(fTimeDelta);

    if (m_EventUI[0]->Check_AnimFin())
    {
        m_isClose = true;
        m_isOpen = false;
        m_isFinished = true;
        CUIManager::GetInstance()->Close_Scene(TEXT("Fade"));
    }
        

    return S_OK;
}

void CUIFade::Fade_Out()
{
    m_isAnimFin = false;
    m_isFinished = false;
    m_isClose = false;
    m_isOpen = true;
    m_EventUI[0]->Close_UI();
}

void CUIFade::Fade_In()
{
    m_isAnimFin = false;
    m_isFinished = false;
    m_isClose = false;
    m_isOpen = true;
    m_EventUI[0]->Show_UI();
}

CUIFade* CUIFade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUIFade* pInstance = new CUIFade();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUIFade::Free()
{
    __super::Free();
}
