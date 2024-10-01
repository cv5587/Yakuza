#include "Adventure_Reactor.h"

#include "GameInstance.h"

#include "Mesh.h"

CAdventure_Reactor::CAdventure_Reactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CAdventure_Reactor::CAdventure_Reactor(const CAdventure_Reactor& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CAdventure_Reactor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAdventure_Reactor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ADVENTURE_REACTOR_IODESC* gameobjDesc = static_cast<ADVENTURE_REACTOR_IODESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_wstrModelName = gameobjDesc->wstrModelName;
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);
	m_pModelCom->Set_AnimationIndex(0);

	return S_OK;
}

void CAdventure_Reactor::Priority_Tick(const _float& fTimeDelta)
{
}

void CAdventure_Reactor::Tick(const _float& fTimeDelta)
{
	Move(fTimeDelta);
}

void CAdventure_Reactor::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);

	vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
}

HRESULT CAdventure_Reactor::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		if (nullptr != m_pMaterialCom)
		{
			if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
				return E_FAIL;
		}

		_bool fFar = m_pGameInstance->Get_CamFar();
		m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;
		_bool isRM = true;
		_bool isRT = true;
		_bool isMulti = true;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS)))
			isMulti = false;
		m_pShaderCom->Bind_RawValue("g_isMulti", &isMulti, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("IsY3Shader", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;
		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
			isRM = false;
		m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RTTexture", i, aiTextureType_EMISSIVE)))
			isRT = false;
		m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //블랜드
		else
			m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

string CAdventure_Reactor::Get_CurrentAnimationName()
{
	return string();
}

HRESULT CAdventure_Reactor::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC Desc{};
	Desc.iCurrentLine = m_iNaviRouteNum;
	Desc.iCurrentRouteDir = DIR_F;
	Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAdventure_Reactor::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CAdventure_Reactor::Move(const _float& fTimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//웨이포인트 
	_vector vDir = m_pNavigationCom->Compute_WayPointDir(vPosition, fTimeDelta);
	m_pTransformCom->LookAt_For_LandObject(vDir, true);

	m_pTransformCom->Go_Straight_CustumSpeed(m_fSpeed, fTimeDelta, m_pNavigationCom);
}

CAdventure_Reactor* CAdventure_Reactor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAdventure_Reactor* pInstance = new CAdventure_Reactor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CAdventure_Reactor::Clone(void* pArg)
{
	CAdventure_Reactor* pInstance = new CAdventure_Reactor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAdventure_Reactor::Free()
{
	__super::Free();
}
