#include "..\Public\CameraObj.h"

#include "GameInstance.h"
#include "Transform.h"

CCameraObj::CCameraObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCameraObj::CCameraObj(const CCameraObj& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCameraObj::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CCameraObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CAMERAOBJINPUT_DESC* inputDesc = (CAMERAOBJINPUT_DESC*)pArg;
		m_tCameraDesc = inputDesc->tObjDesc;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(0.5, 0.5, 0.5);

	return S_OK;
}

void CCameraObj::Priority_Tick(const _float& fTimeDelta)
{
}

void CCameraObj::Tick(const _float& fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CCameraObj::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CCameraObj::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		/*m_pShaderCom->Begin(m_iShaderPassNum);*/
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	return S_OK;
}


HRESULT CCameraObj::Add_Components(void* pArg)
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

HRESULT CCameraObj::Bind_ShaderResources()
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

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}

CCameraObj* CCameraObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraObj* pInstance = new CCameraObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCameraObj::Clone(void* pArg)
{
	CCameraObj* pInstance = new CCameraObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLightObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
