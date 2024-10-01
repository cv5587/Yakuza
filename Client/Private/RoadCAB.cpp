#include "RoadCAB.h"

#include "GameInstance.h"

#include "AI_RoadCAB.h"

CRoadCAB::CRoadCAB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRoadStanding{ pDevice, pContext }
{
}

CRoadCAB::CRoadCAB(const CRoadCAB& rhs)
	: CRoadStanding{ rhs }
{
}

HRESULT CRoadCAB::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoadCAB::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fChangeInterval = 10.f;

	return S_OK;
}

void CRoadCAB::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadCAB::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRoadCAB::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoadCAB::Add_Components()
{
	//if (FAILED(__super::Add_Components()))
	//	return E_FAIL;

	//행동트리 저장
	CAI_RoadCAB::AI_ADVENTURE_DESC AIDesc{};
	AIDesc.pAnimCom = m_pAnimCom;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;
	AIDesc.pAStartCom = m_pAStartCom;

	m_pTree = dynamic_cast<CAI_RoadCAB*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_RoadCAB"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CRoadCAB::Change_Animation()
{
	__super::Change_Animation();

	switch (m_iState)
	{
	case ADVENTURE_IDLE:
	{
		m_strAnimName = "f_cab_stand_tlk_edit";
		m_isAnimLoop = true;
		break;
	}
	case ADVENTURE_REACT_A:
	{
		m_strAnimName = "f_cab_stand_tlk_edit_react_a";
		break;
	}
	case ADVENTURE_REACT_B:
	{
		m_strAnimName = "f_cab_stand_tlk_edit_react_b";
		break;
	}
	case ADVENTURE_REACT_C:
	{
		m_strAnimName = "f_cab_stand_tlk_edit_react_c";
		break;
	}
	case ADVENTURE_REACT_D:
	{
		m_strAnimName = "f_cab_stand_tlk_edit_react_d";
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

CRoadCAB* CRoadCAB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoadCAB* pInstance = new CRoadCAB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CRoadCAB::Clone(void* pArg)
{
	CRoadCAB* pInstance = new CRoadCAB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRoadCAB::Free()
{
	__super::Free();
}
