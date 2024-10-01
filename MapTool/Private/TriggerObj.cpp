#include "..\Public\TriggerObj.h"
#include "Imgui_Manager.h"

#include "GameInstance.h"

CTriggerObj::CTriggerObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CTriggerObj::CTriggerObj(const CTriggerObj& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CTriggerObj::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTriggerObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		TRIGGEROBJ_DESC* triggerobj = (TRIGGEROBJ_DESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(triggerobj->vStartPos);
		m_tTriggerDesc = triggerobj->tTriggerDesc;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;



	return S_OK;
}

void CTriggerObj::Priority_Tick(const _float& fTimeDelta)
{
}

void CTriggerObj::Tick(const _float& fTimeDelta)
{
}

void CTriggerObj::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CTriggerObj::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		/*m_pShaderCom->Begin(m_iShaderPassNum);*/
		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
//
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif


	return S_OK;
}

void CTriggerObj::Set_TriggerDesc(TRIGGER_DESC triggerDesc)
{
	m_tTriggerDesc = triggerDesc;
}

TRIGGER_DESC CTriggerObj::Get_TriggerDesc()
{
	m_tTriggerDesc.vTransform = *m_pTransformCom->Get_WorldFloat4x4();

	return m_tTriggerDesc;
}

CTriggerObj* CTriggerObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriggerObj* pInstance = new CTriggerObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTriggerObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CTriggerObj::Add_Components()
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

HRESULT CTriggerObj::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fObjID", m_iObjectIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fFar", *m_pGameInstance->Get_CamFar())))
		return E_FAIL;


	bool	bWrite;
	if (CImgui_Manager::IDWRIE::TRIGGER == CImgui_Manager::GetInstance()->Get_Write())
	{
		bWrite = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bWriteID", &bWrite, sizeof(bool))))
			return E_FAIL;
	}
	else
	{
		bWrite = false;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bWriteID", &bWrite, sizeof(bool))))
			return E_FAIL;
	}

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}


CGameObject* CTriggerObj::Clone(void* pArg)
{
	CTriggerObj* pInstance = new CTriggerObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTriggerObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriggerObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}

