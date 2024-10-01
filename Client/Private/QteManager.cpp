#include "QteManager.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Animation.h"
#include "Yoneda.h"
#include "Player.h"

Client::CQteManager::CQteManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext },
    m_pUIManager{ CUIManager::GetInstance() }
{
    Safe_AddRef(m_pUIManager);
}

Client::CQteManager::CQteManager(const CQteManager& rhs)
    : CGameObject{ rhs },
    m_pUIManager{ rhs.m_pUIManager}
{
    Safe_AddRef(m_pUIManager);
}

HRESULT Client::CQteManager::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Client::CQteManager::Initialize(void* pArg)
{
    if (FAILED(Ready_SlowKeyFrame()))
        return E_FAIL;

    return S_OK;
}

void Client::CQteManager::Priority_Tick(const _float& fTimeDelta)
{
}

void Client::CQteManager::Tick(const _float& fTimeDelta)
{
    Check_QTE_Section();
    Slowing();
    Cancle_KeyFrame();

    if (m_isKuze)
        m_pTargetMonster->Set_QTEResult(m_iSuccess);
    else
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));

        if (nullptr != pMonster)
            pMonster->Set_QTEResult(m_iSuccess);
    }
}

void Client::CQteManager::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT Client::CQteManager::Render()
{
    return S_OK;
}

void Client::CQteManager::Set_Animation(CAnim* pAnim, string strAnimName)
{
    m_pPlayerAnimCom = pAnim;
    m_strAnimationName = strAnimName;
    m_strPlayingAnimName = m_strAnimationName;
    m_iSuccess = 0;
}

void CQteManager::Set_Kuze(CMonster* pMonster)
{
    m_isKuze = true;
    m_pTargetMonster = pMonster;
}

HRESULT Client::CQteManager::Ready_SlowKeyFrame()
{
    /* 요네다 관련 */
    QTE_DESC Desc{ };
    Desc.iAnimIndex = 3;
    Desc.iCamAnimIndex = 24;
    Desc.iStartKeyFrameIndex = 10; 
    Desc.iEndKeyFrameIndex = 22;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    Desc.iSuccesStartFrame = 23;
    Desc.iSuccesEndFrame= 181;
    Desc.iFailedStartFrame = 182;
    Desc.iFailedEndFrame = 291;

    m_QTEs.emplace("a60300", Desc);

    Desc.iAnimIndex = 5;
    Desc.iCamAnimIndex = 26;
    Desc.iStartKeyFrameIndex = 61;
    Desc.iEndKeyFrameIndex = 75;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    Desc.iSuccesStartFrame = 76;
    Desc.iSuccesEndFrame = 244;
    Desc.iFailedStartFrame = 245;
    Desc.iFailedEndFrame = 628;

    m_QTEs.emplace("a60330", Desc);

    Desc.iAnimIndex = 6;
    Desc.iCamAnimIndex = 27;
    Desc.iStartKeyFrameIndex = 120;
    Desc.iEndKeyFrameIndex = 140;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    Desc.iSuccesStartFrame = 286;
    Desc.iSuccesEndFrame = 350;
    Desc.iFailedStartFrame = 141;
    Desc.iFailedEndFrame = 285;

    m_QTEs.emplace("a60350", Desc);

    Desc.iAnimIndex = 20;
    Desc.iCamAnimIndex = 45;
    Desc.iStartKeyFrameIndex = 0;
    Desc.iEndKeyFrameIndex = 40;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    Desc.iSuccesStartFrame = 41;
    Desc.iSuccesEndFrame = 85;
    Desc.iFailedStartFrame = 278;
    Desc.iFailedEndFrame = 342;
    m_QTEs.emplace("h23250", Desc);

    // 쿠제의 두번째 QTE 정보
    Desc.iAnimIndex = 20;
    Desc.iCamAnimIndex = 45;
    Desc.iStartKeyFrameIndex = 86;
    Desc.iEndKeyFrameIndex = 112;
    Desc.iKeyFrames = Desc.iEndKeyFrameIndex - Desc.iStartKeyFrameIndex;
    Desc.iSuccesStartFrame = 113;
    Desc.iSuccesEndFrame = 277;
    Desc.iFailedStartFrame = 343;
    Desc.iFailedEndFrame = 436;
    m_KuzeSecondQTE = Desc;

    return S_OK;
}

void Client::CQteManager::ResetVariables()
{
    m_fSlowSpeed = QTE_TIME_SPEED_MAX;
    m_fOutTime = 0.f;
    m_fPassTime = 0.f;
    m_strAnimationName = "";
    m_isSlowing = false;
    //m_iQteCount = 0;
    //m_pTargetMonster = nullptr;

    m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 1);
    m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1);
}

void Client::CQteManager::Skip_KeyFrame(QTE_DESC& Desc)
{
    auto AnimList = m_pPlayerAnimCom->Get_Animations();

    switch (m_iSuccess)
    {
    case 1:         //성공
    {
        // 키류 모델 애니메이션의 키프레임을 옮겨준다.
        AnimList[Desc.iAnimIndex]->Set_CurrentPosition(_double(Desc.iSuccesStartFrame));

        // 키류 컷신 카메라 애니메이션의 키프레임을 옮겨준다.
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
        CModel* pPlayerCamModel = dynamic_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model_Cam")));
        auto CamModelAnimList = pPlayerCamModel->Get_Animations();
        CamModelAnimList[Desc.iCamAnimIndex]->Set_CurrentPosition(Desc.iSuccesStartFrame);

        // 몬스터의 애니메이션 키프레임을 옮겨준다.
        // 쿠제가 아니라면 요네다를 꺼내서 저장한다.
        // 쿠제라면 컷신 실행 시 Set해줘야한다.
        if (!m_isKuze)
            m_pTargetMonster = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));

        CAnimation* pMonsterAnimation = dynamic_cast<CAnim*>(m_pTargetMonster->Get_Component(TEXT("Com_SyncAnim")))->Get_CurrentAnimation();
        pMonsterAnimation->Set_CurrentPosition(_double(Desc.iSuccesStartFrame));
        break;
    }
    case 2:         //실패
    {
        AnimList[Desc.iAnimIndex]->Set_CurrentPosition(_double(Desc.iFailedStartFrame));

        // 키류 컷신 카메라 애니메이션의 키프레임을 옮겨준다.
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
        CModel* pPlayerCamModel = dynamic_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model_Cam")));
        auto CamModelAnimList = pPlayerCamModel->Get_Animations();
        CamModelAnimList[Desc.iCamAnimIndex]->Set_CurrentPosition(Desc.iFailedStartFrame);


        if (!m_isKuze)
            m_pTargetMonster = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));

        CAnimation* pMonsterAnimation = dynamic_cast<CAnim*>(m_pTargetMonster->Get_Component(TEXT("Com_SyncAnim")))->Get_CurrentAnimation();
        pMonsterAnimation->Set_CurrentPosition(_double(Desc.iFailedStartFrame));
        break;
    }
    }
}

void CQteManager::Cancle_KeyFrame()
{
    if ("" == m_strPlayingAnimName && !m_isKuze) return;

    auto iter = m_QTEs.find(m_strPlayingAnimName);
    if (iter == m_QTEs.end() && m_iQteCount < 1) return;        // m_iQteCount가 1보다작은데 

    QTE_DESC m_eCurrentQTE = m_iQteCount > 0 ? m_KuzeSecondQTE : iter->second;

    // 성공 키프레임과 실패 키프레임을 비교해서, 성공 키프레임이 앞쪽이라면 true, 실패 키프레임이 앞쪽이라면 false
    _bool isFront = m_eCurrentQTE.iSuccesStartFrame < m_eCurrentQTE.iFailedStartFrame;
    auto AnimList = m_pPlayerAnimCom->Get_Animations();

    switch (m_iSuccess)
    {
    case 1:
    {
        if (m_eCurrentQTE.iSuccesEndFrame <= *AnimList[m_eCurrentQTE.iAnimIndex]->Get_CurrentPosition())
        {
            m_strPlayingAnimName = "";

            if (!m_isKuze)
            {
                CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
                pPlayer->Reset_CutSceneEvent();

                CMonster* pYoneda = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));
                pYoneda->Off_Sync();
            }
            else
            {
                if (m_iQteCount > 0)
                {
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
                    pPlayer->Reset_CutSceneEvent();

                    m_pTargetMonster->Off_Sync();
                }
                else
                {
                    // 키류 모델 애니메이션의 키프레임을 옮겨준다.
                    AnimList[m_eCurrentQTE.iAnimIndex]->Set_CurrentPosition(_double(86));

                    // 키류 컷신 카메라 애니메이션의 키프레임을 옮겨준다.
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
                    CModel* pPlayerCamModel = dynamic_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model_Cam")));
                    auto CamModelAnimList = pPlayerCamModel->Get_Animations();
                    CamModelAnimList[m_eCurrentQTE.iCamAnimIndex]->Set_CurrentPosition(86);

                    CAnimation* pMonsterAnimation = dynamic_cast<CAnim*>(m_pTargetMonster->Get_Component(TEXT("Com_SyncAnim")))->Get_CurrentAnimation();
                    pMonsterAnimation->Set_CurrentPosition(_double(86));
                }

            }

        }
         
        break;
    }
    case 2:
{
        if (m_eCurrentQTE.iFailedEndFrame <= *AnimList[m_eCurrentQTE.iAnimIndex]->Get_CurrentPosition())
        {
            m_strPlayingAnimName = "";

            // 이 때 컷신 종료해줘야한다.
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
            pPlayer->Reset_CutSceneEvent();

            if (!m_isKuze)
            {
                CMonster* pYoneda = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Yoneda"), 0));
                pYoneda->Off_Sync();
            }
            else
            {
                if (m_iQteCount > 0)
                {
                    m_pTargetMonster->Off_Sync();
                }
                else
                {
                    // 키류 모델 애니메이션의 키프레임을 옮겨준다.
                    AnimList[m_eCurrentQTE.iAnimIndex]->Set_CurrentPosition(_double(86));

                    // 키류 컷신 카메라 애니메이션의 키프레임을 옮겨준다.
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
                    CModel* pPlayerCamModel = dynamic_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model_Cam")));
                    auto CamModelAnimList = pPlayerCamModel->Get_Animations();
                    CamModelAnimList[m_eCurrentQTE.iCamAnimIndex]->Set_CurrentPosition(86);

                    CAnimation* pMonsterAnimation = dynamic_cast<CAnim*>(m_pTargetMonster->Get_Component(TEXT("Com_SyncAnim")))->Get_CurrentAnimation();
                    pMonsterAnimation->Set_CurrentPosition(_double(86));
                }
                
            }
        }

        break;
    }
    }


}

void Client::CQteManager::Slowing()
{
    if (!m_isSlowing) return;

    m_fSlowSpeed -= m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));

    if (m_fSlowSpeed <= QTE_TIME_SPEED)
    {
        m_fSlowSpeed = QTE_TIME_SPEED;
    }

    m_pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), m_fSlowSpeed);
    m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fSlowSpeed);

    auto it = m_QTEs.find(m_strAnimationName);

    QTE_DESC m_eCurrentQTE = m_iQteCount > 0 ? m_KuzeSecondQTE : it->second;

    if (it != m_QTEs.end()) {
        _uint animIndex = m_eCurrentQTE.iAnimIndex;

        // 해당 애니메이션의 포지션을 가져와서 얼마나 경과되었는지 계산한다.
        // Get_AnimPosition 값이 iStartKeyFrameIndex 이후라는 확신이 있는 상황에서 Slowing를 실행시켜야한다.
        _double AnimPos = *m_pPlayerAnimCom->Get_AnimPosition(animIndex);

        _double ConvertAnimPos = AnimPos - m_eCurrentQTE.iKeyFrames;
        _double PassRatio = ConvertAnimPos / m_eCurrentQTE.iKeyFrames;

        if (Check_QTE())
        {
            m_iSuccess = 1;
            m_pUIManager->PressKey();
            Skip_KeyFrame(m_eCurrentQTE);

            ResetVariables();
        }
    }
}

_bool Client::CQteManager::Check_QTE()
{
    return (m_pGameInstance->GetKeyState(DIK_Q) == TAP);
}

void Client::CQteManager::Check_QTE_Section()
{
    if (nullptr == m_pPlayerAnimCom) return;
    
    auto it = m_QTEs.find(m_strAnimationName);

    if (it != m_QTEs.end()) 
    {
        if (!m_isQTEUI)
        {
            m_isQTEUI = true;
            m_pUIManager->Close_Scene();
            m_pUIManager->Open_Scene(TEXT("QTE"));
        }

        _uint animIndex = it->second.iAnimIndex;

        if(!m_isKuze)
        {
            if (!m_isSlowing && it->second.iStartKeyFrameIndex < *m_pPlayerAnimCom->Get_AnimPosition(animIndex))
            {
                m_isSlowing = true;
            }
            if (m_isSlowing && it->second.iEndKeyFrameIndex < *m_pPlayerAnimCom->Get_AnimPosition(animIndex))
            {
                m_iSuccess = 2;
                m_isSlowing = false;
                ResetVariables();

                Skip_KeyFrame(it->second);
            }
        }
        else
        {
            //현재 애니메이션의 첫번째 QTE 진행
            if (m_iQteCount < 1)
            {
                if (!m_isSlowing && it->second.iStartKeyFrameIndex < *m_pPlayerAnimCom->Get_AnimPosition(animIndex))
                {
                    m_iQteCount++;
                    m_isSlowing = true;
                }
                if (m_isSlowing && it->second.iEndKeyFrameIndex < *m_pPlayerAnimCom->Get_AnimPosition(animIndex))
                {
                    m_pUIManager->Close_Scene();
                    m_iSuccess = 2;
                    m_isSlowing = false;
                    //ResetVariables();

                    Skip_KeyFrame(it->second);
                }
            }
            else
            {
                if (!m_isSlowing && m_KuzeSecondQTE.iStartKeyFrameIndex < *m_pPlayerAnimCom->Get_AnimPosition(animIndex))
                {
                    m_isSlowing = true;
                }
                if (m_isSlowing && m_KuzeSecondQTE.iEndKeyFrameIndex < *m_pPlayerAnimCom->Get_AnimPosition(animIndex))
                {
                    m_iSuccess = 2;
                    m_isSlowing = false;
                    ResetVariables();

                    Skip_KeyFrame(m_KuzeSecondQTE);
                }
            }
        }
  
    }

}

Client::CQteManager* Client::CQteManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CQteManager* pInstance = new CQteManager(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CQteManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* Client::CQteManager::Clone(void* pArg)
{
    CQteManager* pInstance = new CQteManager(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CQteManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Client::CQteManager::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerAnimCom);
    Safe_Release(m_pUIManager);
}
