#include "Karaoke_Glass.h"

#include "GameInstance.h"
#include "Shader.h"

CKaraoke_Glass::CKaraoke_Glass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSocketModel{ pDevice, pContext }
{
}

CKaraoke_Glass::CKaraoke_Glass(const CKaraoke_Glass& rhs)
	:CSocketModel{ rhs }
{
}

HRESULT CKaraoke_Glass::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKaraoke_Glass::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Change_Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0, 0, 0, 1));
	m_pTransformCom->Set_Scale(m_pTransformCom->Get_Scaled().x * 1.2f, m_pTransformCom->Get_Scaled().y * 1.2f, m_pTransformCom->Get_Scaled().z * 1.2f);

	return S_OK;
}

void CKaraoke_Glass::Priority_Tick(const _float& fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CKaraoke_Glass::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CKaraoke_Glass::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CKaraoke_Glass::Render()
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

HRESULT CKaraoke_Glass::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_karaoke_cup"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CKaraoke_Glass::Bind_ResourceData()
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

CKaraoke_Glass* CKaraoke_Glass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKaraoke_Glass* pInstance = new CKaraoke_Glass(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CKaraoke_Glass::Clone(void* pArg)
{
	CKaraoke_Glass* pInstance = new CKaraoke_Glass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CKaraoke_Glass::Free()
{
	__super::Free();
}
