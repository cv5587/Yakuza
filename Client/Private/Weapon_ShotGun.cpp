#include "Weapon_ShotGun.h"

#include "GameInstance.h"
#include "Shader.h"

CShotGun::CShotGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSocketModel{ pDevice, pContext }
{
}

CShotGun::CShotGun(const CShotGun& rhs)
	:CSocketModel{ rhs }
{
}

HRESULT CShotGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShotGun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Change_Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-90.f));
	m_pTransformCom->Change_Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.1, -0.07f, 1));

	return S_OK;
}

void CShotGun::Priority_Tick(const _float& fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CShotGun::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CShotGun::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CShotGun::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);		//µðÆúÆ®
		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

HRESULT CShotGun::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_ShotGun"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShotGun::Bind_ResourceData()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	//if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CShotGun* CShotGun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShotGun* pInstance = new CShotGun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CShotGun::Clone(void* pArg)
{
	CShotGun* pInstance = new CShotGun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CShotGun::Free()
{
	__super::Free();
}
