#include "RoadYOP.h"

#include "GameInstance.h"

#include "AI_RoadNML.h"

CRoadYOP::CRoadYOP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRoadWalker { pDevice, pContext }
{
}

CRoadYOP::CRoadYOP(const CRoadYOP& rhs)
	: CRoadWalker{ rhs }
{
}

HRESULT CRoadYOP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoadYOP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	return S_OK;
}

void CRoadYOP::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadYOP::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRoadYOP::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoadYOP::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

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

CRoadYOP* CRoadYOP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoadYOP* pInstance = new CRoadYOP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CRoadYOP::Clone(void* pArg)
{
	CGameObject* pInstance = new CRoadYOP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRoadYOP::Free()
{
	__super::Free();
}
