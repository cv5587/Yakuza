#include "RoadNML.h"

#include "GameInstance.h"

#include "AI_RoadNML.h"

CRoadNML::CRoadNML(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRoadWalker { pDevice, pContext }
{
}

CRoadNML::CRoadNML(const CRoadNML& rhs)
	: CRoadWalker{ rhs }
{
}

HRESULT CRoadNML::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoadNML::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CRoadNML::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadNML::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRoadNML::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoadNML::Add_Components()
{
	//if (FAILED(__super::Add_Components()))
	//	return E_FAIL;

	//행동트리 저장
	CAI_RoadNML::AI_ADVENTURE_DESC AIDesc{};
	AIDesc.pAnimCom= m_pAnimCom;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;
	AIDesc.pAStartCom = m_pAStartCom;
	AIDesc.pNavigation = m_pNavigationCom;

	m_pTree = dynamic_cast<CAI_RoadNML*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_RoadNML"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

CRoadNML* CRoadNML::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoadNML* pInstance = new CRoadNML(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CRoadNML::Clone(void* pArg)
{
	CGameObject* pInstance = new CRoadNML(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRoadNML::Free()
{
	__super::Free();
}
