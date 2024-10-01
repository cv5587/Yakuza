#include "../Default/framework.h"
#include "Terrain.h"

#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject{ rhs }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		TERRAIN_DESC* terrainDesc = (TERRAIN_DESC*)pArg;
		m_pVIBufferCom->Change_LandScale(terrainDesc->vPlaneSize.x, terrainDesc->vPlaneSize.y);

		m_vTerrainScale.x = terrainDesc->vPlaneSize.x;
		m_vTerrainScale.y = terrainDesc->vPlaneSize.y;
	}


	return S_OK;
}

void CTerrain::Priority_Tick(const _float& fTimeDelta)
{
}

void CTerrain::Tick(const _float& fTimeDelta)
{
	//m_pVIBufferCom->Culling(m_pTransformCom->Get_WorldMatrix_Inverse());
}

void CTerrain::Late_Tick(const _float& fTimeDelta)
{
	m_pNavigationCom->Tick();

	//m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

#ifdef _DEBUG	
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Flat"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Diffuse */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
