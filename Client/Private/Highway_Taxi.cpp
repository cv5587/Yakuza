#include "Highway_Taxi.h"

#include "GameInstance.h"
#include "Highway_Kiryu.h"

CHighway_Taxi::CHighway_Taxi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CHighway_Taxi::CHighway_Taxi(const CHighway_Taxi& rhs)
	: CLandObject{ rhs }
{
}

_uint CHighway_Taxi::Get_CurrentWaypointIndex()
{
	return m_pNavigationCom->Get_WaypointIndex();
}

_uint CHighway_Taxi::Get_WaypointSize()
{
	return m_pNavigationCom->Get_RouteIndexs(m_iNaviRouteNum).size();
}

const _float4x4* CHighway_Taxi::Get_KiryuBoneMatrix(const _char* pBoneName)
{
	return m_pKiryu->Get_BoneMatrix(pBoneName);
}

void CHighway_Taxi::Set_NavigationRouteIndex(_uint iLine)
{
	//라인이 기존과 다르다면 변경된 라인으로 이동 후 진행되도록 한다.

	m_iNaviRouteNum = iLine;
	m_pNavigationCom->Set_NavigationRouteIndex(iLine);
}

void CHighway_Taxi::Set_Dir(_uint iStageDir)
{
	m_iStageDir = iStageDir;
	m_pKiryu->Set_StageDir(iStageDir);
}

void CHighway_Taxi::Sit_Swap()
{
	m_pKiryu->Change_Behavior(CHighway_Kiryu::SWAP);
}

HRESULT CHighway_Taxi::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHighway_Taxi::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	HIGHWAY_TEXI_DESC* gameobjDesc = (HIGHWAY_TEXI_DESC*)pArg;
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Objects()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	return S_OK;
}

void CHighway_Taxi::Priority_Tick(const _float& fTimeDelta)
{
	m_pKiryu->Priority_Tick(fTimeDelta);

	//if (m_pGameInstance->GetKeyState(DIK_N) == HOLD)
	//	m_pTransformCom->Go_Straight(fTimeDelta);
}

void CHighway_Taxi::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (m_isObjectDead)
	//	m_pModelCom->Play_Animation(fTimeDelta);

#ifdef _DEBUG
	if (m_pGameInstance->GetKeyState(DIK_LCONTROL) == TAP)
		m_isStop = !m_isStop;
#endif // _DEBUG

	if (!m_isStop)
		Move_Waypoint(fTimeDelta * m_fBooster);

	m_pKiryu->Tick(fTimeDelta);
}

void CHighway_Taxi::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (nullptr != m_pNavigationCom)
	{
		float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);
		vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
	}
	m_pKiryu->Late_Tick(fTimeDelta);
}

HRESULT CHighway_Taxi::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);		//디폴트
		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

string CHighway_Taxi::Get_CurrentAnimationName()
{
	return m_pModelCom->Get_AnimationName(m_pModelCom->Get_CurrentAnimationIndex());
}

void CHighway_Taxi::Set_NavigationIndex(int iIndex)
{
	m_pNavigationCom->Set_Index(iIndex);
}

void CHighway_Taxi::OnHit(_float fDamage)
{
	// 키류가 차체에 숨어있는 상황이라면, 차량의 체력이 깎이고, 반대는 키류 체력이 깎인다.
	if (CHighway_Kiryu::HIDE == m_pKiryu->Get_CurrentBehavior())
	{
		m_fCarHp -= fDamage;
	}
	else
	{
		m_pKiryu->OnHit(fDamage);
	}
}

void CHighway_Taxi::Change_Animation()
{
}

void CHighway_Taxi::Move_Waypoint(const _float& fTimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//웨이포인트 
	_float4 vMovePos;
	_vector vDir = m_pNavigationCom->Compute_WayPointDir(vPosition, fTimeDelta);
	m_pTransformCom->LookAt_For_LandObject(vDir, true);

	m_pTransformCom->Go_Straight_CustumSpeed(m_fSpeed, fTimeDelta, m_pNavigationCom);
}

HRESULT CHighway_Taxi::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Taxi"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC Desc{};
	Desc.iCurrentLine = m_iNaviRouteNum;
	Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHighway_Taxi::Add_Objects()
{
	CHighway_Kiryu::CARCHASE_KIRYU_DESC Desc{ const_cast<_float4x4*>(m_pTransformCom->Get_WorldFloat4x4()) };
	m_pKiryu = dynamic_cast<CHighway_Kiryu*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CarChase_Kiryu"), &Desc));
	if (nullptr == m_pKiryu)
		return E_FAIL;

	return S_OK;
}

HRESULT CHighway_Taxi::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CHighway_Taxi* CHighway_Taxi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHighway_Taxi* pInstance = new CHighway_Taxi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CHighway_Taxi::Clone(void* pArg)
{
	CHighway_Taxi* pInstance = new CHighway_Taxi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHighway_Taxi::Free()
{
	__super::Free();

	Safe_Release(m_pKiryu);
	Safe_Release(m_pNavigationCom);
}
