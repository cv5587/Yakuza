#include "UITitle.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUITitle::CUITitle()
    :CUIScene{}
{
}

CUITitle::CUITitle(const CUITitle& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUITitle::Show_Scene()
{
    if (FAILED(__super::Show_Scene()))
        return E_FAIL;

    m_iCurrentTime = 0.f;
    m_isStart = false;
    return S_OK;
}

HRESULT CUITitle::Close_Scene()
{
    if (FAILED(__super::Close_Scene()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUITitle::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUITitle::Tick(const _float& fTimeDelta)
{
    if (m_isStart)
        m_iCurrentTime += fTimeDelta;

    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;
    for (auto& iter : m_EventUI)
    {
        iter->Tick(fTimeDelta);
    }


    return S_OK;
}

HRESULT CUITitle::Late_Tick(const _float& fTimeDelta)
{
    if (m_isStart && m_iCurrentTime > 2.0f)
    {
        //여기다가 시간 지나면 꺼질지 뭐할지 선택하면됨(나는 꺼질꺼임)
        CUIManager::GetInstance()->Close_Scene(m_strSceneName);
        m_isEnd = true;
        m_isStart = false;
    }

    if (m_isStart && m_iCurrentTime > 1.0f)
    {
        m_isBattleStart = true;
    }

    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;
    for (auto& iter : m_EventUI)
    {
        iter->Late_Tick(fTimeDelta);
    }


    return S_OK;
}

void CUITitle::Start_Title(_uint Index)
{
    m_EventUI[0]->Show_UI();
    m_EventUI[1]->Show_UI();

    dynamic_cast<CGroup*>(m_EventUI[1])->Show_Choice(Index);

    //밖에서 불러와서 몇번쨰 타이틀 띄울지 하자
    m_isStart = true;
    m_isEnd = false;
    m_isBattleStart = false;
}

CUITitle* CUITitle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUITitle* pInstance = new CUITitle();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUITitle::Free()
{
    __super::Free();
}
