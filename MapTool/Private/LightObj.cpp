#include "..\Public\LightObj.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Transform.h"

CLightObj::CLightObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLightObj::CLightObj(const CLightObj& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CLightObj::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CLightObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		LIGHTOBJ_DESC*			lightDesc = (LIGHTOBJ_DESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(lightDesc->vStartPos);
		m_tLightDesc = lightDesc->tLightDesc;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;



	return S_OK;
}

void CLightObj::Priority_Tick(const _float& fTimeDelta)
{
}

void CLightObj::Tick(const _float& fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CLightObj::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CLightObj::Render()
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

void CLightObj::Set_LightDesc(LIGHT_DESC lightDesc)
{
	/* 구조체 변경 */
	m_tLightDesc = lightDesc;

	XMVECTOR	vPosition = XMLoadFloat4(&m_tLightDesc.vPosition);
	vPosition.m128_f32[3] = 1;

	/* 객체 위치도 바꾸기 */
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}


HRESULT CLightObj::Add_Components(void* pArg)
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

HRESULT CLightObj::Bind_ShaderResources()
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
	if (CImgui_Manager::IDWRIE::LIGHT == CImgui_Manager::GetInstance()->Get_Write())
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

CLightObj* CLightObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightObj* pInstance = new CLightObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLightObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightObj::Clone(void* pArg)
{
	CLightObj* pInstance = new CLightObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLightObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
