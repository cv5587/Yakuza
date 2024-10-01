#include "UIKaraoke_Select.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"

#include "Karaoke_Kiryu.h"

CUIKaraoke_Select::CUIKaraoke_Select()
    :CUIScene{}
{
}

CUIKaraoke_Select::CUIKaraoke_Select(const CUIKaraoke_Select& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUIKaraoke_Select::Show_Scene()
{
    if (FAILED(__super::Show_Scene()))
        return E_FAIL;

    m_iCurButton = 0;
    m_isStart = false;
    m_iCurrentTime = 0.f;

    Action();
    return S_OK;
}

HRESULT CUIKaraoke_Select::Close_Scene()
{
    if (FAILED(__super::Close_Scene()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIKaraoke_Select::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;
    return S_OK;
}

HRESULT CUIKaraoke_Select::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{

    if (pUIObject->Get_Event())
    {
        if (nullptr == m_Disc)
            m_Disc = static_cast<CGroup*>(pUIObject);
        else if (m_UIGrade.size() < 3)
            m_UIGrade.push_back(pUIObject);
        else if (nullptr == m_Select)
            m_Select = static_cast<CGroup*>(pUIObject);
        else if (m_UIName.size() < 3)
            m_UIName.push_back(pUIObject);
        else
            m_MusicTitle = pUIObject;


        return S_OK;
    }
    else
    {
        m_UI.push_back(pUIObject);
    }

    if (CUI_Object::TYPE_BTN == pUIObject->Get_TypeIndex())
    {
        Safe_AddRef(pUIObject);
        m_Button.push_back(dynamic_cast<CBtn*>(pUIObject));
    }


    return S_OK;
}


HRESULT CUIKaraoke_Select::Tick(const _float& fTimeDelta)
{


    m_Disc->Tick(fTimeDelta);
    if(FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    for (auto& iter : m_EventUI)
        iter->Tick(fTimeDelta);
    for (auto& iter : m_UIGrade)
        iter->Tick(fTimeDelta);
    for (auto& iter : m_UIName)
        iter->Tick(fTimeDelta);


    m_Select->Tick(fTimeDelta);

    if (m_isStart)
    {
        m_MusicTitle->Tick(fTimeDelta);
        m_iCurrentTime += fTimeDelta;
    }

    return S_OK;
}

HRESULT CUIKaraoke_Select::Late_Tick(const _float& fTimeDelta)
{
    if (m_pGameInstance->GetKeyState(DIK_E) == TAP)
    {
        m_isStart = true;

        m_pGameInstance->StopSound(SOUND_UI);
        m_pGameInstance->PlaySound_W(L"menu_decide.wav", SOUND_UI, 0.5f);
    }

    m_Disc->Late_Tick(fTimeDelta);
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    for (auto& iter : m_EventUI)
        iter->Late_Tick(fTimeDelta);
    for (auto& iter : m_UIGrade)
        iter->Late_Tick(fTimeDelta);
    for (auto& iter : m_UIName)
        iter->Late_Tick(fTimeDelta);

    m_Select->Late_Tick(fTimeDelta);

    if (m_isStart)
        m_MusicTitle->Late_Tick(fTimeDelta);

    if (m_isStart && m_MusicTitle->Check_AnimFin()&&m_iCurrentTime>3.0f)
    {
        CUIManager::GetInstance()->Close_Scene(m_strSceneName);
        CUIManager::GetInstance()->Open_Scene(TEXT("Karaoke_Play"));

        m_pGameInstance->PlaySound_W(L"Bakamita.mp3", SOUND_BGM, 0.5f);
    }


    if (!m_isAnimFin)
        Check_AimFin();
    else
    {
        if (Over_InterSect())
        {
            OverAction();
        }
    }

    return S_OK;
}

_bool CUIKaraoke_Select::Click_InterSect()
{
    for (size_t i = 0; i < m_UIName.size(); i++)
    {
        if (m_UIName[i]->Click_Intersect())
        {
            m_iCurButton = i;
            if (m_iPrevButton != m_iCurButton)
            {
                Action();
                m_iPrevButton = m_iCurButton;
                return true;
            }
            else
                return false;
        }
    }
    return false;
}

_bool CUIKaraoke_Select::Over_InterSect()
{
  /*  for (size_t i = 0; i < m_UIName.size(); i++)
    {
        if (m_UIName[i]->Click_Intersect())
        {
            m_iCurButton = i;
            if (m_iPrevButton != m_iCurButton)
            {
                OverAction();
                m_iPrevButton = m_iCurButton;
                return true;
            }
            else
                return false;
        }
    }*/
    return false;
}

void CUIKaraoke_Select::Action()
{

    //이건 선책 창 변경
    CUI_Object* pUISelect = m_Select->Get_PartObject(SELECT_MUSIC);

    _vector MusicPosition = m_UIName[m_iCurButton]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

    _vector SelectPostiion = pUISelect->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

    SelectPostiion = XMVectorSetY(SelectPostiion, XMVectorGetY(MusicPosition));

    pUISelect->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, SelectPostiion);

    //이건 cd 변경
    vector<CUI_Object*> vecUIDisc = m_Disc->Get_PartObjects();

    CUI_Object* pUIDiscs = m_Disc->Get_PartObject(DISC0);

    pUIDiscs->Set_isPlay(true);
    pUIDiscs->Show_UI();


    for (size_t i = TAG0; i < DISC_END; i++)
    {
        if (m_iCurButton + 1 == i)
        {
            vecUIDisc[i]->Set_isPlay(true);
            vecUIDisc[i]->Show_UI();
        }
        else
        {
            vecUIDisc[i]->Set_isPlay(false);
            vecUIDisc[i]->Close_UI();
        }
    }



}

void CUIKaraoke_Select::OverAction()
{



}

CUIKaraoke_Select* CUIKaraoke_Select::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUIKaraoke_Select* pInstance = new CUIKaraoke_Select();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUIKaraoke_Select::Free()
{
    __super::Free();

    Safe_Release(m_MusicTitle);
    Safe_Release(m_Disc);
    Safe_Release(m_Select);


    for (auto& iter : m_UIGrade)
    {
        Safe_Release(iter);
    }
    m_UIGrade.clear();

    for (auto& iter : m_UIName)
        Safe_Release(iter);
    m_UIName.clear();
}
