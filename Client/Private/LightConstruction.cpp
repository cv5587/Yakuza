#include "..\Public\LightConstruction.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Transform.h"

#include "Mesh.h"

CLightConstruction::CLightConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CLightConstruction::CLightConstruction(const CLightConstruction& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CLightConstruction::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CLightConstruction::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
		m_iLayerNum = gameobjDesc->iLayer;
		m_wstrModelName = gameobjDesc->wstrModelName;
		m_iShaderPassNum = gameobjDesc->iShaderPass;
		m_iObjectType = gameobjDesc->iObjType;

		for (int i = 0; i < gameobjDesc->iDecalNum; i++)
		{
			DECAL_DESC  tDecal;
			CTexture* pTexture;
			XMMATRIX    vStartPos;

			CDecal::DECALOBJ_DESC		decalObjDesc{};
			decalObjDesc.iMaterialNum = gameobjDesc->pDecal[i].iMaterialNum;
			decalObjDesc.pTexture = m_pModelCom->Copy_DecalTexture(decalObjDesc.iMaterialNum);
			decalObjDesc.vStartPos = XMLoadFloat4x4(&gameobjDesc->pDecal[i].vTransform);

			CDecal* pDecal = dynamic_cast<CDecal*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Decal"), &decalObjDesc));

			m_vDecals.push_back(pDecal);
		}

		for (int i = 0; i < gameobjDesc->iColliderNum; i++)
		{
			OBJCOLLIDER_DESC		objDesc = gameobjDesc->pColliderDesc[i];

			CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

			ColliderDesc.eType = (CCollider::TYPE)objDesc.iColliderType;
			ColliderDesc.vExtents = objDesc.vExtents;
			ColliderDesc.vCenter = objDesc.vCenter;
			ColliderDesc.vRotation = objDesc.vQuaternion;

			CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

			m_vColliders.push_back(pCollider);

		}
	}

	m_Casecade = { 0.f, 10.f, 24.f, 40.f };

	return S_OK;
}

void CLightConstruction::Priority_Tick(const _float& fTimeDelta)
{
}

void CLightConstruction::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);

	if (2 == m_iShaderPassNum)
	{
		m_fWaterDeltaTime += fTimeDelta * 0.07;

		if (m_fWaterDeltaTime > 1)
			m_fWaterDeltaTime = 0;
	}


#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		iter->Tick(m_pTransformCom->Get_WorldMatrix());
#endif
}

void CLightConstruction::Late_Tick(const _float& fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		// 0 : 그냥간판 - 형광등같은
		// 1 : 형광등자르기 + 알파
		// 2 : rm 텍스처 적용 - 외부간판
		// 3 : Lamp
		// 4 : 형광등 + 투명 - 소화전
		// 5 : Emissive + 알파
		// 6 : Emissive + 알파x
		if (m_iShaderPassNum == 0 || m_iShaderPassNum == 2)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
		}
		else if (m_iShaderPassNum == 1)
		{
			//m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
		}
		else if (m_iShaderPassNum == 3)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
		}
		else if (m_iShaderPassNum == 4)
		{
			//m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
		}
		else if (m_iShaderPassNum == 5)
		{
			//m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
		}
		else if (m_iShaderPassNum == 6)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
		}
		else if (m_iShaderPassNum == 7)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
		}



		for (auto& iter : m_vDecals)
			iter->Late_Tick(fTimeDelta);
	}
	}
	

HRESULT CLightConstruction::Render()
{
#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		m_pGameInstance->Add_DebugComponent(iter);
#endif


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		// Lamp 일떄 - 3
		// rm 외부간판 - 7
		if (3 == m_iShaderPassNum || 7 == m_iShaderPassNum)
		{
			bool	bRMExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_METALNESS);
			// Normal texture가 있을 경우
			if (true == bRMExist)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS);

				if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
					return E_FAIL;
			}
		}


		m_pShaderCom->Begin(m_iShaderPassNum);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CLightConstruction::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		bool	bRMExist = m_pModelCom->Check_Exist_Material(i, aiTextureType_METALNESS);
		// Normal texture가 있을 경우
		if (true == bRMExist)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS);

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bExistRMTex", &bRMExist, sizeof(bool))))
				return E_FAIL;
		}

		if(m_iShaderPassNum != 7)
			m_pShaderCom->Begin(m_iShaderPassNum);
		else
			m_pShaderCom->Begin(8);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CLightConstruction::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxMeshLight"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightConstruction::Bind_ShaderResources()
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

CLightConstruction* CLightConstruction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightConstruction* pInstance = new CLightConstruction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightConstruction::Clone(void* pArg)
{
	CLightConstruction* pInstance = new CLightConstruction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightConstruction::Free()
{
	__super::Free();

	for (auto& iter : m_vDecals)
		Safe_Release(iter);
	m_vDecals.clear();

	for (auto& iter : m_vColliders)
		Safe_Release(iter);
	m_vColliders.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSystemManager);
}
