#include "..\Public\CameraInstallObj.h"

#include "GameInstance.h"
#include "Transform.h"

CCameraInstallObj::CCameraInstallObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCameraInstallObj::CCameraInstallObj(const CCameraInstallObj& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCameraInstallObj::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CCameraInstallObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CAMERAINSTALL_DESC* camInstallDesc = (CAMERAINSTALL_DESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(camInstallDesc->vStartPos);
		m_eInstallDesc = camInstallDesc->eInstallDesc;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(0.5, 0.5, 0.5);

	return S_OK;
}

void CCameraInstallObj::Priority_Tick(const _float& fTimeDelta)
{
}

void CCameraInstallObj::Tick(const _float& fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CCameraInstallObj::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CCameraInstallObj::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (CAM_EYE == m_eInstallDesc)
		{
			m_pShaderCom->Begin(0);
		}
		else if (CAM_FOCUS == m_eInstallDesc)
		{
			m_pShaderCom->Begin(0);
		}

		m_pModelCom->Render(i);
	}


	return S_OK;
}


HRESULT CCameraInstallObj::Add_Components(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Model_Bone_Sphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraInstallObj::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fObjID", m_iObjectIndex)))
		return E_FAIL;

	bool bWrite = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bWriteID", &bWrite, sizeof(bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fFar", *m_pGameInstance->Get_CamFar())))
		return E_FAIL;

	return S_OK;
}

CCameraInstallObj* CCameraInstallObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraInstallObj* pInstance = new CCameraInstallObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCameraInstallObj::Clone(void* pArg)
{
	CCameraInstallObj* pInstance = new CCameraInstallObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLightObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraInstallObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
