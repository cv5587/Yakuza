#include "UICarchase.h"

#include"GameInstance.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"	

#include "CarChase_Monster.h"
#include "Highway_Taxi.h"   
#include "Highway_Kiryu.h"

#include "EffectManager.h"

CUICarchase::CUICarchase()
    :CUIScene{}
{
}

CUICarchase::CUICarchase(const CUICarchase& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUICarchase::Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP)
{
    CARCCHASE_UI_DESC* pTarget = Find_TargetUI(iIndex);
    if (nullptr == pTarget)
        return E_FAIL;

    _vector vTargetPos = TargetMatrix.r[3];
    pTarget->TargetingUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);

    if (pTarget->HPBarBackUI != nullptr && pTarget->HPBarUI != nullptr)
    {
        vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) - 60.f);

        pTarget->HPBarBackUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);
        pTarget->HPBarUI->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);

        _float LerpHp = m_pGameInstance->Lerp(-1.f, 0.f, iHP / 100.f);

        static_cast<CUI_Texture*>(pTarget->HPBarUI)->Change_Point(_float4(0.f, 0.f, LerpHp, 0.f), _float4(0.f, 0.f, LerpHp, 0.f));

        pTarget->HPBarUI->Get_TransformCom()->Set_Scale(100.f, 6.f, 0.f);
    }
    return S_OK;
}

HRESULT CUICarchase::Remove_Target(_uint iIndex)
{
    CARCCHASE_UI_DESC* pTarget = Find_TargetUI(iIndex);
    if (nullptr == pTarget)
        return E_FAIL;

    Safe_Release(pTarget->HPBarBackUI);
    Safe_Release(pTarget->HPBarUI);
    //Safe_Release(pTarget->pMonsterAddr);
    Safe_Release(pTarget->TargetingUI);

    // 무언가를 삭제할 때, 현재저장하고 있는 값이라면 댕글링포인터가 되지 않게 초기화해준다.
    if (&m_Targets.find(iIndex)->second == m_CurrentTarget)
    {
        m_CurrentTarget = nullptr;
    }

    m_Targets.erase(iIndex);

    return S_OK;
}

CCarChase_Monster* CUICarchase::Get_Target()
{
    return nullptr == m_CurrentTarget ? nullptr : m_CurrentTarget->pMonsterAddr;
}

HRESULT CUICarchase::Show_Scene()
{

    _int Index = 0;

    //8번9번 ui 리로드
    for (auto& iter : m_UI)
    {
        if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
        {
            if (Back == Index || TEXT == Index)
            {
                iter->Close_UI();
            }
            else
                iter->Show_UI();
        }
        else
            iter->Close_UI();

        Index++;
    }

    for (auto& iter : m_EventUI)
    {
        if (CUI_Object::TYPE_BTN != iter->Get_TypeIndex())
            iter->Show_UI();
        else
            iter->Close_UI();
    }
    m_isAnimFin = false;
    m_isClose = false;


    return S_OK;
}

HRESULT CUICarchase::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUICarchase::Tick(const _float& fTimeDelta)
{
    Update_Player();

    _float fGameTimeDelta = m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

    __super::Tick(fGameTimeDelta);

    m_EventUI[AIM]->Tick(fTimeDelta);
    m_EventUI[INFO]->Tick(fTimeDelta);

    Move_Aim(fGameTimeDelta);

    for (auto& pTarget : m_Targets)
    {
        if (pTarget.second.HPBarBackUI != nullptr && pTarget.second.HPBarUI != nullptr)
        {
            pTarget.second.HPBarBackUI->Tick(fGameTimeDelta);
            pTarget.second.HPBarUI->Tick(fGameTimeDelta);
        }

        pTarget.second.TargetingUI->Tick(fGameTimeDelta);
    }

    return S_OK;
}

HRESULT CUICarchase::Late_Tick(const _float& fTimeDelta)
{
    _float fGameTimeDelta = m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));
    
    __super::Late_Tick(fGameTimeDelta);

    m_EventUI[AIM]->Late_Tick(fTimeDelta);
    m_EventUI[INFO]->Late_Tick(fTimeDelta);

    for (auto& pTarget : m_Targets)
    {
        if (pTarget.second.HPBarBackUI != nullptr && pTarget.second.HPBarUI != nullptr)
        {
            pTarget.second.HPBarBackUI->Late_Tick(fGameTimeDelta);
            pTarget.second.HPBarUI->Late_Tick(fGameTimeDelta);
        }

        pTarget.second.TargetingUI->Late_Tick(fGameTimeDelta);
    }

    if (!m_isAnimFin)
        m_isAnimFin = true;

    Coll_Aim();

    return S_OK;
}

void CUICarchase::Action()
{
}

void CUICarchase::OverAction()
{
}

HRESULT CUICarchase::Add_TargetingRound(_uint iIndex, class CCarChase_Monster* pMonster, _bool isBullet)
{
    if (nullptr != Find_TargetUI(iIndex))
        return E_FAIL;

    CARCCHASE_UI_DESC Desc{};

    CUI_Object* pTargeting = dynamic_cast<CUI_Object*>(m_EventUI[TARGETING]->Clone(nullptr));
    if (nullptr == pTargeting)
        return E_FAIL;

    if (isBullet == false)
    {
        CUI_Object* pHpBarBack = dynamic_cast<CUI_Object*>(m_EventUI[HPBAR_BACK]->Clone(nullptr));
        if (nullptr == pHpBarBack)
            return E_FAIL;

        CUI_Object* pHpBar = dynamic_cast<CUI_Object*>(m_EventUI[HPBAR]->Clone(nullptr));
        if (nullptr == pHpBar)
            return E_FAIL;

        Desc.HPBarBackUI = pHpBarBack;
        Desc.HPBarUI = pHpBar;
    }
    else
    {
        Desc.HPBarBackUI = nullptr;
        Desc.HPBarUI = nullptr;
    }

    Desc.TargetingUI = pTargeting;
    Desc.pMonsterAddr = pMonster;

    m_Targets.emplace(iIndex, Desc);
    //Safe_AddRef(pMonster);
    
    return S_OK;
}

HRESULT CUICarchase::Update_Player()
{
    _uint Level = m_pGameInstance->Get_CurrentLevel();

    CHighway_Taxi* Taxi = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(Level, TEXT("Layer_Taxi"), 0));

    CHighway_Kiryu* pKiryu = static_cast<CHighway_Kiryu*>(Taxi->Get_Kiryu());

   _uint Type = pKiryu->Get_CurrentBehavior();

   if (CHighway_Kiryu::HIDE == Type)
   {
       if (KiryuType != Type)
       {
           m_UI[Back]->Show_UI();
           m_UI[TEXT]->Show_UI();
       }
           KiryuType = Type;
   }
   else
   {
       if (KiryuType != Type && CHighway_Kiryu::HIDE== KiryuType)
       {
           m_UI[Back]->Close_UI();
           m_UI[TEXT]->Close_UI();
       }
           KiryuType = Type;
   }

    //정보 갱신 + ui 업데이트
    //1 . 캐릭피 , 2히트아이 , 3자동차피 , 4총알
    vector<CUI_Object*>* pUI =  dynamic_cast<CGroup*>(m_EventUI[INFO])->Get_pPartObjects();

    _float Upoint = m_pGameInstance->Lerp(-1.f, 0.f, pKiryu->Get_HP() / pKiryu->Get_MaxHP());
    dynamic_cast<CUI_Texture*>((*pUI)[PLAYERHP])->Change_Point(_float4(0.f,0.f,Upoint,0.f),_float4(0.f,0.f,Upoint,0.f));

    Upoint = m_pGameInstance->Lerp(-1.f, 0.f, pKiryu->Get_HitEye() / pKiryu->Get_MaxHitEye());
    dynamic_cast<CUI_Texture*>((*pUI)[HITEYE])->Change_Point(_float4(0.f,0.f,Upoint,0.f),_float4(0.f,0.f,Upoint,0.f));

    Upoint = m_pGameInstance->Lerp(-1.f, 0.f, Taxi->Get_CarHp() / Taxi->Get_CarMaxHp());
    dynamic_cast<CUI_Texture*>((*pUI)[CARHP])->Change_Point(_float4(0.f, 0.f, Upoint, 0.f), _float4(0.f, 0.f, Upoint, 0.f));

    Upoint = m_pGameInstance->Lerp(-1.f, 0.f, Taxi->Get_CarHp() / Taxi->Get_CarMaxHp());

    wstring NumAmmo = to_wstring(pKiryu->Get_Ammo());
    dynamic_cast<CText*>((*pUI)[AMMO])->Set_Text(NumAmmo);

    return S_OK;
}

void CUICarchase::Move_Aim(const _float& fTimeDelta)
{
    _vector vPos = m_EventUI[AIM]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

    if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
    {
        _float fY = XMVectorGetY(vPos);
        fY += fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetY(vPos, fY);
    }
    if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
    {
        _float fX = XMVectorGetX(vPos);
        fX += fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetX(vPos, fX);
    }
    if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
    {
        _float fX = XMVectorGetX(vPos);
        fX -= fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetX(vPos, fX);
    }
    if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
    {
        _float fY = XMVectorGetY(vPos);
        fY -= fTimeDelta * m_fAimSpeed;
        vPos = XMVectorSetY(vPos, fY);
    }

    m_EventUI[AIM]->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
}

CUICarchase::CARCCHASE_UI_DESC* CUICarchase::Find_TargetUI(_uint iIndex)
{
    auto pTarget = m_Targets.find(iIndex);
    
    if (pTarget == m_Targets.end())
        return nullptr;

    return &(pTarget->second);
}

//충돌나면?
void CUICarchase::Coll_Aim()
{
    if (m_pGameInstance->GetMouseState(DIM_LB) == TAP)
    {

        _uint Level = m_pGameInstance->Get_CurrentLevel();

        CHighway_Taxi* Taxi = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(Level, TEXT("Layer_Taxi"), 0));

        CHighway_Kiryu* pKiryu = static_cast<CHighway_Kiryu*>(Taxi->Get_Kiryu());
        if(pKiryu->Shot())
        {
            m_pGameInstance->StopAndPlaySound(L"p_gunshot_def_0.wav", SOUND_ATTACK, 0.5f);
            if (nullptr != m_CurrentTarget)
            {
                m_CurrentTarget->TargetingUI->Show_UI();
                m_CurrentTarget->pMonsterAddr->Set_Coll();
            }
        }
    }

    _float4 vAimPos;
    XMStoreFloat4(&vAimPos, m_EventUI[AIM]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
    _float3 vAimScale = m_EventUI[AIM]->Get_TransformCom()->Get_Scaled();
    //에임이 정확하게 들어와 있어야 함.
    for (auto& pTarget : m_Targets)
    {
        _float4 vTargetPos;
        XMStoreFloat4(&vTargetPos, pTarget.second.TargetingUI->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

        _float3 vTargetScale;
        vTargetScale = pTarget.second.TargetingUI->Get_TransformCom()->Get_Scaled();

        _float fAimMinX = vAimPos.x - vAimScale.x * 0.5f;
        _float fAimMaxX = vAimPos.x + vAimScale.x * 0.5f;

        _float fAimMinY = vAimPos.y - vAimScale.y * 0.5f;
        _float fAimMaxY = vAimPos.y + vAimScale.y * 0.5f;

        _float fTargetMinX = vTargetPos.x - vTargetScale.x * 0.5f;
        _float fTargetMaxX = vTargetPos.x + vTargetScale.x * 0.5f;

        _float fTargetMinY = vTargetPos.y - vTargetScale.y * 0.5f;
        _float fTargetMaxY = vTargetPos.y + vTargetScale.y * 0.5f;

        _bool isColl = { false };

        if (fAimMinX > fTargetMinX && fAimMaxX < fTargetMaxX
            && fAimMinY > fTargetMinY && fAimMaxY < fTargetMaxY)
        {
            //충돌
            m_CurrentTarget = &pTarget.second;

            isColl = true;
            return;
        }

        // 아무것도 충돌하지 않았을 경우.
        if (!isColl)
            m_CurrentTarget = nullptr;
    }
}

CUICarchase* CUICarchase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUICarchase* pInstance = new CUICarchase();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        MSG_BOX("Make Failed :CUICarchase");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUICarchase::Free()
{
    __super::Free();

    for (auto& pTarget : m_Targets)
    {
        Safe_Release(pTarget.second.HPBarBackUI);
        Safe_Release(pTarget.second.HPBarUI);
        //Safe_Release(pTarget.second.pMonsterAddr);
        Safe_Release(pTarget.second.TargetingUI);
    }
    m_Targets.clear();
    
}
