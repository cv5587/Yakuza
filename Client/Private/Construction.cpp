#include "..\Public\Construction.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Transform.h"

#include "Mesh.h"

CConstruction::CConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager { CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CConstruction::CConstruction(const CConstruction& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager}
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CConstruction::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CConstruction::Initialize(void* pArg)
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

	// 물웅덩이 noiseTexture
	//if (2 == m_iShaderPassNum)
	//{
	//	/* For.Com_Shader */
	//	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Texture_NoiseTexture"),
	//		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
	//		return E_FAIL;
	//}


	return S_OK;
}

void CConstruction::Priority_Tick(const _float& fTimeDelta)
{
}

void CConstruction::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);

	if (2 == m_iShaderPassNum)
	{
		m_fWaterDeltaTime += fTimeDelta*0.07;

		if (m_fWaterDeltaTime > 1)
			m_fWaterDeltaTime = 0;
	}


#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		iter->Tick(m_pTransformCom->Get_WorldMatrix());
#endif
}

void CConstruction::Late_Tick(const _float& fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f) && OBJECT_TYPE::LARGE_CONSTRUCTION != m_iObjectType)
	{
		if (0 == m_iShaderPassNum || 3 == m_iShaderPassNum)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		}
		else if (1 == m_iShaderPassNum)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_GLASS, this);
		}
		else if (2 == m_iShaderPassNum)
		{
			//m_pGameInstance->Add_Renderer(CRenderer::RENDER_PUDDLE, this);
		}
		else
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		}

		if (1 != m_iObjectType)
		{
			if (m_pGameInstance->isShadow())
			{
				// 처음 렌더를 돌 때만 그림자를 그려준다.
				m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this);
			}
		}

		for (auto& iter : m_vDecals)
			iter->Late_Tick(fTimeDelta);
	}
	else if(OBJECT_TYPE::LARGE_CONSTRUCTION == m_iObjectType)
	{
		// 컬링x
		if (0 == m_iShaderPassNum || 3 == m_iShaderPassNum)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		}
		else if (1 == m_iShaderPassNum)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_GLASS, this);
		}
		else if (2 == m_iShaderPassNum)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_PUDDLE, this);
		}
		else 
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
		}

		if (1 != m_iObjectType)
		{
			if (m_pGameInstance->isShadow())
			{
				// 처음 렌더를 돌 때만 그림자를 그려준다.
				m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this);
			}
		}

		for (auto& iter : m_vDecals)
			iter->Late_Tick(fTimeDelta);
	}
	
}

HRESULT CConstruction::Render()
{
#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		m_pGameInstance->Add_DebugComponent(iter);
#endif

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (nullptr != m_pMaterialCom)
			m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));
		else
		{
			_bool isUVShader = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
				return E_FAIL;
		}


		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

	
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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;
		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
			isRM = false;
		m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_isRT", &isRM, sizeof(_bool));
		

		


		if (1 == m_iShaderPassNum)
		{
			// 유리문 처리
			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDiffuse"), m_pShaderCom, "g_RefractionTexture")))
				return E_FAIL;
		}
		else if (2 == m_iShaderPassNum)
		{
			//m_pTexture->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0);

			// 웅덩이
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fWaterDeltaTime, sizeof(float))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDiffuse"), m_pShaderCom, "g_RefractionTexture")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonBlendDepth"), m_pShaderCom, "g_DepthTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_Matrix("g_ReflectViewMatrix", m_pGameInstance->Get_ReflectViewMatrix())))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBlurReverse"), m_pShaderCom, "g_BlurReverseTexture")))
				return E_FAIL;

		}


		m_pShaderCom->Begin(m_iShaderPassNum);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CConstruction::Render_LightDepth()
{
	const _float4x4* ViewMatrixArray = m_pGameInstance->Get_Shadow_Transform_View_Float4x4();
	const _float4x4* ProjMatrixArray = m_pGameInstance->Get_Shadow_Transform_Proj_Float4x4();

	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_ViewMatrixArray", ViewMatrixArray, 3)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_ProjMatrixArray", ProjMatrixArray, 3)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(SHADER_LIGHTDEPTH);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

int CConstruction::Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc)
{
	/* 바이너리화하기 위한 데이터 제공 */

	XMStoreFloat4x4(&objplaceDesc->vTransform, m_pTransformCom->Get_WorldMatrix());

	//objPlaceDesc.iLayer = m_iLayerNum;

	string strName = m_pGameInstance->WstringToString(m_wstrModelName);
	strcpy_s(objplaceDesc->strModelCom, strName.c_str());

	objplaceDesc->iShaderPassNum = m_iShaderPassNum;
	objplaceDesc->iObjType = m_iObjectType;

	/* layer는 return 형식으로 */
	return m_iLayerNum;
}



void CConstruction::Edit_GameObject_Information(MAPOBJ_DESC mapDesc)
{
	m_iLayerNum = mapDesc.iLayer;
	m_iShaderPassNum = mapDesc.iShaderPass;
	m_iObjectType = mapDesc.iObjType;
}

CConstruction::MAPOBJ_DESC CConstruction::Send_GameObject_Information()
{
	return MAPOBJ_DESC();
}

HRESULT CConstruction::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//모델 이름 추출
	string strModelName = m_pGameInstance->WstringToString(gameobjDesc->wstrModelName);
	string strRemoveName = "Prototype_Component_Model_";
	_int iPos = strModelName.find(strRemoveName);

	if (iPos == string::npos)
		return E_FAIL;

	strModelName = strModelName.erase(iPos, strRemoveName.size());

	wstring strMaterialName = TEXT("Prototype_Component_Material_") + m_pGameInstance->StringToWstring(strModelName);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, strMaterialName,
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		m_pMaterialCom = nullptr;

	return S_OK;
}

HRESULT CConstruction::Bind_ShaderResources()
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

CConstruction* CConstruction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CConstruction* pInstance = new CConstruction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CConstruction::Clone(void* pArg)
{
	CConstruction* pInstance = new CConstruction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CConstruction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConstruction::Free()
{
	__super::Free();

	for (auto& iter : m_vDecals)
		Safe_Release(iter);
	m_vDecals.clear();

	for (auto& iter : m_vColliders)
		Safe_Release(iter);
	m_vColliders.clear();

	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSystemManager);
}
