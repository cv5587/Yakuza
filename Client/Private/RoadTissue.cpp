#include "RoadTissue.h"

#include "GameInstance.h"

#include "AI_RoadTissue.h"

CRoadTissue::CRoadTissue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CRoadStanding{ pDevice, pContext }
{
}

CRoadTissue::CRoadTissue(const CRoadTissue& rhs)
    : CRoadStanding{rhs }
{
}

HRESULT CRoadTissue::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRoadTissue::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fChangeInterval = 18.f;
    m_iGender = GENDER_M;
    return S_OK;
}

void CRoadTissue::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadTissue::Tick(const _float& fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CRoadTissue::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

    __super::Late_Tick(fTimeDelta);
}

HRESULT CRoadTissue::Add_Components()
{
    //if (FAILED(__super::Add_Components()))
    //    return E_FAIL;

    //행동트리 저장
    CAI_RoadTissue::AI_ADVENTURE_DESC AIDesc{};
    AIDesc.pAnimCom = m_pAnimCom;
    AIDesc.pState = &m_iState;
    AIDesc.pThis = this;
    AIDesc.pAStartCom = m_pAStartCom;

    m_pTree = dynamic_cast<CAI_RoadTissue*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_RoadTissue"), &AIDesc));
    if (nullptr == m_pTree)
        return E_FAIL;

    return S_OK;
}

void CRoadTissue::Change_Animation()
{
	__super::Change_Animation();

	switch (m_iState)
	{
	case ADVENTURE_IDLE:
	{
		m_strAnimName = "f_nml_act_tissue_kamae";
		m_isAnimLoop = true;
		break;
	}
	case ADVENTURE_TISSUE_ST:
	{
		m_strAnimName = "f_nml_act_tissue_st";
		break;
	}
    case ADVENTURE_TISSUE_LP:
    {
        m_strAnimName = "f_nml_act_tissue_lp";
        break;
    }
    case ADVENTURE_TISSUE_EN:
    {
        m_strAnimName = "f_nml_act_tissue_en";
        break;
    }
	default:
		break;
	}

	m_iAnim = m_pAnimCom->Get_AnimationIndex(m_strAnimName.c_str());

	if (m_iAnim == -1)
		return;

	if (FAILED(Setup_Animation()))
		return;
}

CRoadTissue* CRoadTissue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRoadTissue* pInstance = new CRoadTissue(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
        Safe_Release(pInstance);

    return pInstance;
}

CGameObject* CRoadTissue::Clone(void* pArg)
{
    CRoadTissue* pInstance = new CRoadTissue(*this);

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CRoadTissue::Free()
{
    __super::Free();
}
