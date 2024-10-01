#include "SkyDome.h"
#include "GameInstance.h"

CSkyDome::CSkyDome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkyDome::CSkyDome(const CSkyDome& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyDome::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkyDome::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		SkyObj_Desc* gameobjDesc = (SkyObj_Desc*)pArg;
		m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);

		return S_OK;
	}
}

void CSkyDome::Priority_Tick(const _float& fTimeDelta)
{

}

void CSkyDome::Tick(const _float& fTimeDelta)
{

}

void CSkyDome::Late_Tick(const _float& fTimeDelta)
{
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance->Get_CamPosition());

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CSkyDome::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CSkyDome::Add_Components(void* pArg)
{
	SkyObj_Desc* gameobjDesc = (SkyObj_Desc*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxMeshSky"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyDome::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CSkyDome* CSkyDome::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkyDome* pInstance = new CSkyDome(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSkyDome");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkyDome::Clone(void* pArg)
{
	CSkyDome* pInstance = new CSkyDome(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSkyDome");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyDome::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
