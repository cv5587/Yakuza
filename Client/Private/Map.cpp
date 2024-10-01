#include "..\Public\Map.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Transform.h"
#include "Player.h"
#include "Mesh.h"

CMap::CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CMap::CMap(const CMap& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CMap::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CMap::Initialize(void* pArg)
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
		m_bCull = gameobjDesc->bCull;

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

		m_pPlayerMatrix = gameobjDesc->vPlayerMatrix;
	}

	m_Casecade = { 0.f, 10.f, 24.f, 40.f };

	D3D11_QUERY_DESC queryDesc;
	queryDesc.Query = D3D11_QUERY_OCCLUSION;
	queryDesc.MiscFlags = 0;
	m_pDevice->CreateQuery(&queryDesc, &m_pOcclusionQuery);

	float		fRandom = m_pGameInstance->Get_Random(0, 1);
	m_fTimer = fRandom;

	return S_OK;
}

void CMap::Priority_Tick(const _float& fTimeDelta)
{
}

void CMap::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);

	if (2 == m_iShaderPassNum)
	{
		m_fWaterDeltaTime += fTimeDelta * 0.07;

		if (m_fWaterDeltaTime > 1)
			m_fWaterDeltaTime = 0;
	}

	m_fTimer += fTimeDelta * 2;



	if (1 < m_fTimer)
	{
		m_fTimer = 0;
		m_fDynamicTime = 0;
		if (true == m_bPositive)
			m_fDynamicTime += 0.25;
		else
			m_fDynamicTime -= 0.25;

		m_bPositive = m_bPositive ? false : true;
	}
	
#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		iter->Tick(m_pTransformCom->Get_WorldMatrix());
#endif


	_vector vCubePos = m_CubeWorldMatrix.r[3];
	m_fCamDistance = XMVectorGetX(XMVector3Length(vCubePos - m_pGameInstance->Get_CamPosition()));

	
	if ((m_pGameInstance->Get_CurrentLevel() == LEVEL_TUTORIAL ||
		m_pGameInstance->Get_CurrentLevel() == LEVEL_NISHIKIWALK ||
		m_pGameInstance->Get_CurrentLevel() == LEVEL_TOKOSTREET || 
		m_pGameInstance->Get_CurrentLevel() == LEVEL_TEST) && true == m_bOcculusionCulling)
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_OCCULUSION, this);
}

void CMap::Late_Tick(const _float& fTimeDelta)
{
	// Renderer에 추가되는 mesh index 비워주고 시작
	m_vRenderDefaulMeshIndex.clear();
	m_vRenderGlassMeshIndex.clear();
	m_vMaskMeshIndex.clear();
	m_vDecalLightMeshIndex.clear();
	m_vSignMeshIndex.clear();
	m_vLampMeshIndex.clear();
	m_vDecalBlendMeshIndex.clear();
	m_vBloomIndex.clear();
	m_vMaskSignIndex.clear();
	m_vDynamicSignIndex.clear();
	m_vStrongBloomIndex.clear();
	m_vCompulsoryDecalBlendMeshIndex.clear();


	if (m_pGameInstance->Get_CurrentLevel() == LEVEL_TUTORIAL ||
		m_pGameInstance->Get_CurrentLevel() == LEVEL_NISHIKIWALK ||
		m_pGameInstance->Get_CurrentLevel() == LEVEL_TOKOSTREET || m_pGameInstance->Get_CurrentLevel() == LEVEL_TEST)
	{
		XMVECTOR vPlayerPos = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0))->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		XMVECTOR vThisPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fDistance = XMVector3Length(vPlayerPos - vThisPos).m128_f32[0]; // 거리 측정	

		m_isFar = false;

		if (fDistance > 700)
			m_isFar = true;

		if (true == m_bOcculusionCulling && isOcculusionDepth())
		{
			if (m_isFar == true)
			{
				Add_Renderer_Far(fTimeDelta);
			}
			else
			{
				Add_Renderer(fTimeDelta);
			}
		}
		else if (false == m_bOcculusionCulling)
		{
			if (m_isFar == true)
			{
				Add_Renderer_Far(fTimeDelta);
			}
			else
			{
				Add_Renderer(fTimeDelta);
			}
		}
	}
	else
	{
		Add_Renderer(fTimeDelta);
	}
	
}

HRESULT CMap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();

	int		iRenderState = m_pGameInstance->Get_RenderState();

	//SSAO 변수 전달
	m_pShaderCom->Bind_RawValue("g_ExcludeSSAO", &m_bExcludeSSAO, sizeof(_float));

	//NonBlend - 일반메시, 

#pragma region Render_DefaultMeshGroup
	if (m_isFar)
		Far_Render(iRenderState);
	else
		Near_Render(iRenderState);
#pragma endregion


	//Render Effect
#pragma region Render_EffectMeshGroup
	if (iRenderState == CRenderer::RENDER_EFFECT)
	{
		for (size_t k = 0; k < m_vDecalLightMeshIndex.size(); k++)
		{
			int		i = m_vDecalLightMeshIndex[k];
			
			bool	bLightCut = true;

			if (LEVEL_OFFICE_1F == m_iCurrentLevel || LEVEL_OFFICE_2F == m_iCurrentLevel)
				bLightCut = false;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightCut", &bLightCut, sizeof(bool))))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_DECAL_LIGHT);

			m_pModelCom->Render(i);
		}

	}
#pragma endregion


#pragma region RenderGlass
	if (iRenderState == CRenderer::RENDER_GLASS)
	{
		for (size_t k = 0; k < m_vRenderGlassMeshIndex.size(); k++)
		{
			int		i = m_vRenderGlassMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			// 유리문 처리
			if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShaderCom, "g_RefractionTexture")))
				return E_FAIL;

			// 색상값 넣어주기
			m_pShaderCom->Begin(SHADER_GLASS_COLOR);

			m_pModelCom->Render(i);
		}
	}

#pragma endregion




	//Render Decal
#pragma region Render_DecalMesh

	if (iRenderState == CRenderer::RENDER_DECAL)
	{
		// 일반 마스크
		//- 6번
		for (size_t k = 0; k < m_vDecalBlendMeshIndex.size(); k++)
		{
			int		i = m_vDecalBlendMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_DECAL_BLEND);

			m_pModelCom->Render(i);
		}


		for (size_t k = 0; k < m_vCompulsoryDecalBlendMeshIndex.size(); k++)
		{
			int		i = m_vCompulsoryDecalBlendMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_COMPULSORY_DECALBLEND);

			m_pModelCom->Render(i);
		}

	}
	
#pragma endregion

	//Render Bloom - nonlight
#pragma region Render_Bloom
	if (iRenderState == CRenderer::RENDER_NONLIGHT)
	{
		// 일반 마스크
		// VtxMesh - Defulat_Pass - 2번
		for (size_t k = 0; k < m_vBloomIndex.size(); k++)
		{
			int		i = m_vBloomIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			//White 부분만 되게끔
			m_pShaderCom->Begin(SHADER_BLOOM_WHITE);

			m_pModelCom->Render(i);
		}

		for (size_t k = 0; k < m_vSignMeshIndex.size(); k++)
		{
			int		i = m_vSignMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			//White 부분만 되게끔
			m_pShaderCom->Begin(SHADER_BLOOM_WHITE);

			m_pModelCom->Render(i);
		}


		for (size_t k = 0; k < m_vLampMeshIndex.size(); k++)
		{
			int		i = m_vLampMeshIndex[k];

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

			m_pShaderCom->Begin(SHADER_LAMP);

			m_pModelCom->Render(i);
		}

		for (size_t k = 0; k < m_vStrongBloomIndex.size(); k++)
		{
			int		i = m_vStrongBloomIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_STRONGBLOOM);

			m_pModelCom->Render(i);
		}

		for (size_t k = 0; k < m_vDynamicSignIndex.size(); k++)
		{
			int		i = m_vDynamicSignIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;


			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fDynamicTime, sizeof(float))))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_DYNAMIC_SIGN_BLOOM);

			m_pModelCom->Render(i);
		}

	}
#pragma endregion

	// Render NonlightNonBlur
#pragma region Render_NonLightNonBlur
	if (iRenderState == CRenderer::RENDER_NONLIGHT_NONBLUR)
	{
		// 일반 마스크
		// VtxMesh - Defulat_Pass - 2번
		for (size_t k = 0; k < m_vSignMeshIndex.size(); k++)
		{
			int		i = m_vSignMeshIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_SIGN);

			m_pModelCom->Render(i);
		}


		for (size_t k = 0; k < m_vMaskSignIndex.size(); k++)
		{
			int		i = m_vMaskSignIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_SIGN_MASK);

			m_pModelCom->Render(i);
		}


		for (size_t k = 0; k < m_vDynamicSignIndex.size(); k++)
		{
			int		i = m_vDynamicSignIndex[k];

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fDynamicTime, sizeof(float))))
				return E_FAIL;

			m_pShaderCom->Begin(SHADER_DYNAMIC_SIGN_SMALL);

			m_pModelCom->Render(i);
		}

	}
#pragma endregion

	

	return S_OK;
}

HRESULT CMap::Far_Render(_uint iRenderState)
{
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();

	if (iRenderState == CRenderer::RENDER_NONBLENDER)
	{
		// 램프 - 일반 mesh 출력, 그리고 부분 nonlight로 들어가서 bloom
		// 부분 bloom - 전부 출력 후 밝은 부분만 nonlight로 들어가서 bloom


#pragma region 일반
		for (size_t k = 0; k < m_vRenderDefaulMeshIndex.size(); k++)
		{
			int		i = m_vRenderDefaulMeshIndex[k];

			_bool isUVShader = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
				return E_FAIL;

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_FAR);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region 램프
		for (size_t k = 0; k < m_vLampMeshIndex.size(); k++)
		{
			int		i = m_vLampMeshIndex[k];

			_bool isUVShader = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
				return E_FAIL;

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_FAR);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region 부분bloom
		for (size_t k = 0; k < m_vBloomIndex.size(); k++)
		{
			int		i = m_vBloomIndex[k];

			_bool isUVShader = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
				return E_FAIL;

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_FAR);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region 강력한bloom
		for (size_t k = 0; k < m_vStrongBloomIndex.size(); k++)
		{
			int		i = m_vStrongBloomIndex[k];

			_bool isUVShader = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
				return E_FAIL;

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_FAR);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region GLASS
		for (size_t k = 0; k < m_vRenderGlassMeshIndex.size(); k++)
		{
			// GLASS NORMAL, DEPTH 등의 정보 넣기
			int		i = m_vRenderGlassMeshIndex[k];

			_bool isUVShader = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
				return E_FAIL;

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_FAR);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region Mask_구Decal
		for (size_t k = 0; k < m_vMaskMeshIndex.size(); k++)
		{
			int		i = m_vMaskMeshIndex[k];

			_bool isUVShader = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
				return E_FAIL;

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_FAR);

			m_pModelCom->Render(i);
		}
#pragma endregion

	}

	return S_OK;
}

HRESULT CMap::Render_LightDepth()
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

HRESULT CMap::Render_OcculusionDepth()
{
	m_pContext->Begin(m_pOcclusionQuery);

	_float4x4 CubeWorldMatrix;
	XMStoreFloat4x4(&CubeWorldMatrix, m_CubeWorldMatrix);
	if (FAILED(m_pCubeShaderCom->Bind_Matrix("g_WorldMatrix", &CubeWorldMatrix)))
		return E_FAIL;
	//if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pCubeShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pCubeShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pCubeShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	m_pCubeShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	m_pContext->End(m_pOcclusionQuery);

	return S_OK;
}

HRESULT CMap::Check_OcculusionCulling()
{
	XMMATRIX worldProjView = m_pTransformCom->Get_WorldMatrix() * m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) * m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
	worldProjView = XMMatrixTranspose(worldProjView);
	CMap::ObjectData objectData = { worldProjView, *m_pGameInstance->Get_CamFar() };
	m_pContext->UpdateSubresource(m_pObjectDataBuffer, 0, nullptr, &objectData, 0, 0);
	
	m_pContext->CSSetConstantBuffers(0, 1, &m_pObjectDataBuffer);

	// dmd
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_OcculusionDepth"), 1);
	m_pVIBufferCom->Bind_Compute(m_pComputeShaderCom);
	

	int pixelCount = 0;
	m_pVIBufferCom->Copy_ResultResources(m_pOutputBufferStaging);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_pContext->Map(m_pOutputBufferStaging, 0, D3D11_MAP_READ, 0, &mappedResource);

	// 정점 수 만큼 루프를 돌며 가시성 정보를 합산
	int* visibilityData = (int*)mappedResource.pData;
	for (int i = 0; i < m_pVIBufferCom->Get_NumVertex(); ++i)
	{
		pixelCount += visibilityData[i];
	}

	m_pContext->Unmap(m_pOutputBufferStaging, 0);

	// 만약 하나 이상의 정점이 가시성이 있다면 렌더링 플래그를 true로 설정
	if (pixelCount > 0)
	{
		m_bRender = true;
	}
	else
	{
		m_bRender = false;
	}

	return S_OK;
}

_bool CMap::isOcculusionDepth()
{
	UINT64 occlusionResult = 0;
	while (m_pContext->GetData(m_pOcclusionQuery, &occlusionResult, sizeof(UINT64), 0) == S_FALSE)
	{
		// 결과를 기다리는 중
	}

	return occlusionResult > 0;
}

HRESULT CMap::Near_Render(_uint iRenderState)
{
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();

	if (iRenderState == CRenderer::RENDER_NONBLENDER)
	{
		// 램프 - 일반 mesh 출력, 그리고 부분 nonlight로 들어가서 bloom
		// 부분 bloom - 전부 출력 후 밝은 부분만 nonlight로 들어가서 bloom


#pragma region 일반
		for (size_t k = 0; k < m_vRenderDefaulMeshIndex.size(); k++)
		{
			int		i = m_vRenderDefaulMeshIndex[k];

			if (nullptr != m_pMaterialCom)
				m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));
			else
			{
				_bool isUVShader = false;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
					return E_FAIL;
			}


			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			
			_bool isNormal = true;
			_bool isRS = true;
			_bool isRD = true;
			_bool isRM = true;
			_bool isRT = false;
			_bool isMulti = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

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
			m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region 일반
		if (LEVEL_OFFICE_1F == m_iCurrentLevel)
		{
			for (size_t k = 0; k < m_vSignMeshIndex.size(); k++)
			{
				int		i = m_vSignMeshIndex[k];

				if (nullptr != m_pMaterialCom)
					m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));
				else
				{
					_bool isUVShader = false;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
						return E_FAIL;
				}


				_float fFar = *m_pGameInstance->Get_CamFar();
				m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
					return E_FAIL;

				_bool isNormal = true;
				_bool isRS = true;
				_bool isRD = true;
				_bool isRM = true;
				_bool isRT = false;
				_bool isMulti = true;

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
					isNormal = false;
				m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

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
				m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

				m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

				m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

				m_pModelCom->Render(i);
			}
		}
#pragma endregion

#pragma region 램프
		for (size_t k = 0; k < m_vLampMeshIndex.size(); k++)
		{
			int		i = m_vLampMeshIndex[k];

			if (nullptr != m_pMaterialCom)
				m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));
			else
			{
				_bool isUVShader = false;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
					return E_FAIL;
			}

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;
			_bool isRS = true;
			_bool isRD = true;
			_bool isRM = true;
			_bool isRT = false;
			_bool isMulti = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

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
			m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region 부분bloom
		for (size_t k = 0; k < m_vBloomIndex.size(); k++)
		{
			int		i = m_vBloomIndex[k];

			if (nullptr != m_pMaterialCom)
				m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));
			else
			{
				_bool isUVShader = false;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
					return E_FAIL;
			}

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;
			_bool isRS = true;
			_bool isRD = true;
			_bool isRM = true;
			_bool isRT = false;
			_bool isMulti = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

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
			m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region 강력한bloom
		for (size_t k = 0; k < m_vStrongBloomIndex.size(); k++)
		{
			int		i = m_vStrongBloomIndex[k];

			if (nullptr != m_pMaterialCom)
				m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));
			else
			{
				_bool isUVShader = false;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
					return E_FAIL;
			}

			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;
			_bool isRS = true;
			_bool isRD = true;
			_bool isRM = true;
			_bool isRT = false;
			_bool isMulti = true;


			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

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
			m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

			m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

			m_pShaderCom->Begin(SHADER_DEFAULT_MAP);

			m_pModelCom->Render(i);
		}
#pragma endregion

#pragma region GLASS
		//for (size_t k = 0; k < m_vRenderGlassMeshIndex.size(); k++)
		//{
		//	// GLASS NORMAL, DEPTH 등의 정보 넣기
		//	int		i = m_vRenderGlassMeshIndex[k];

		//	if (nullptr != m_pMaterialCom)
		//		m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));
		//	else
		//	{
		//		_bool isUVShader = false;
		//		if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
		//			return E_FAIL;
		//	}

		//	_float fFar = *m_pGameInstance->Get_CamFar();
		//	m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
		//		return E_FAIL;

		//	_bool isNormal = true;
		//	_bool isRS = true;
		//	_bool isRD = true;
		//	_bool isRM = true;
		//	_bool isRT = false;
		//	_bool isMulti = true;
		//	

		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
		//		isNormal = false;
		//	m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS)))
		//		isMulti = false;
		//	m_pShaderCom->Bind_RawValue("g_isMulti", &isMulti, sizeof(_bool));

		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
		//		isRS = false;
		//	m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
		//		isRD = false;
		//	m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
		//		isRM = false;
		//	m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool));
		//	m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

		//	m_pShaderCom->Bind_RawValue("g_bCompulsoryAlpha", &m_bCompulsoryAlpha, sizeof(_bool));

		//	m_pShaderCom->Begin(SHADER_GLASS_DEFAULT);

		//	m_pModelCom->Render(i);
		//}
#pragma endregion

#pragma region Mask_구Decal
		for (size_t k = 0; k < m_vMaskMeshIndex.size(); k++)
		{
			int		i = m_vMaskMeshIndex[k];

			if (nullptr != m_pMaterialCom)
				m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(Meshes[i]->Get_MaterialIndex()));
			else
			{
				_bool isUVShader = false;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
					return E_FAIL;
			}


			_float fFar = *m_pGameInstance->Get_CamFar();
			m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			_bool isNormal = true;
			_bool isRS = true;
			_bool isRD = true;
			_bool isRM = true;
			_bool isRT = false;
			_bool isMulti = true;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				isNormal = false;
			m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));

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
			m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));


			m_pShaderCom->Begin(SHADER_DECAL_MASK);

			m_pModelCom->Render(i);
		}
#pragma endregion

	}

	return S_OK;
}

int CMap::Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc)
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

CMap::MAPOBJ_DESC CMap::Get_MapObjDesc_For_AddList()
{
	CMap::MAPOBJ_DESC		mapobjDesc;
	mapobjDesc.vStartPos = m_pTransformCom->Get_WorldMatrix();
	mapobjDesc.wstrModelName = m_wstrModelName;
	mapobjDesc.iShaderPass = m_iShaderPassNum;

	return mapobjDesc;
}

void CMap::Edit_GameObject_Information(MAPOBJ_DESC mapDesc)
{
	m_iLayerNum = mapDesc.iLayer;
	m_iShaderPassNum = mapDesc.iShaderPass;
	m_iObjectType = mapDesc.iObjType;
}

CMap::MAPOBJ_DESC CMap::Send_GameObject_Information()
{
	return MAPOBJ_DESC();
}

void CMap::Add_Renderer(const _float& fTimeDelta)
{
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();


	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_float4x4 vLocalMatrix = Meshes[i]->Get_LocalMatrix();

		XMVECTOR localPos = XMVectorSet(vLocalMatrix._41, vLocalMatrix._42, vLocalMatrix._43, 1);
		XMVECTOR worldPos = XMVectorZero();
		worldPos = XMVector3Transform(localPos, m_pTransformCom->Get_WorldMatrix());

		string meshName = Meshes[i]->Get_Name();

		// 점 위치 찾기
		size_t dotPos = meshName.find('.');

		// 점이 있으면 점 이전 부분을 추출하고, 없으면 전체 문자열을 사용
		string baseName = (dotPos != string::npos) ? meshName.substr(0, dotPos) : meshName;

		const char* baseNameCStr = baseName.c_str();

		if (false == m_bCull)
		{
			// 컬링안함
			if (0 == strcmp(baseNameCStr, "DEFAULTMESH"))
			{
				m_vRenderDefaulMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "GLASSMESH"))
			{
				m_vRenderGlassMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "DECALMESH"))
			{
				m_vMaskMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "DECALLIGHTMESH"))
			{
				m_vDecalLightMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "SIGNMESH"))
			{
				m_vSignMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "LAMPMESH"))
			{
				// Nonblend + effect 둘 다 호출
				m_vLampMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "DECALBLENDMESH"))
			{
				m_vDecalBlendMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "BLOOMMESH"))
			{
				m_vBloomIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "MASKSIGNMESH"))
			{
				m_vMaskSignIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "STRONGBLOOM"))
			{
				m_vStrongBloomIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "COMBLENDDECAL"))
			{
				m_vCompulsoryDecalBlendMeshIndex.push_back(i);
			}
			else if (strstr(baseNameCStr, "DYNAMICSIGN") != nullptr)
			{
				m_vDynamicSignIndex.push_back(i);
			}
			else
			{
				// mesh 안합쳐놓으면 그냥 default로 처리
				m_vRenderDefaulMeshIndex.push_back(i);
			}
		}
		else
		{
			float		fScale = Meshes[i]->Get_MeshScale();

			if (fScale < 15)
				fScale = 25;

			// 컬링
			if (true == m_pGameInstance->isIn_WorldFrustum(worldPos, fScale * 1.5))
			{
				// 컬링안함
				if (0 == strcmp(baseNameCStr, "DEFAULTMESH"))
				{
					m_vRenderDefaulMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "GLASSMESH"))
				{
					m_vRenderGlassMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "DECALMESH"))
				{
					m_vMaskMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "DECALLIGHTMESH"))
				{
					m_vDecalLightMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "SIGNMESH"))
				{
					m_vSignMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "LAMPMESH"))
				{
					// Nonblend + effect 둘 다 호출
					m_vLampMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "DECALBLENDMESH"))
				{
					m_vDecalBlendMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "BLOOMMESH"))
				{
					m_vBloomIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "MASKSIGNMESH"))
				{
					m_vMaskSignIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "STRONGBLOOM"))
				{
					m_vStrongBloomIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "COMBLENDDECAL"))
				{
					m_vCompulsoryDecalBlendMeshIndex.push_back(i);
				}
				else if (strstr(baseNameCStr, "DYNAMICSIGN") != nullptr)
				{
					m_vDynamicSignIndex.push_back(i);
				}
				else
				{
					// mesh 안합쳐놓으면 그냥 default로 처리
					m_vRenderDefaulMeshIndex.push_back(i);
				}
			}
		}

	}

	// RENDER_NONBLEND 돼야하는 그룹
	if (0 < m_vRenderDefaulMeshIndex.size() || 0 < m_vSignMeshIndex.size() || 0 < m_vLampMeshIndex.size() 
		|| 0 < m_vBloomIndex.size() || 0 < m_vMaskMeshIndex.size() || 0 < m_vStrongBloomIndex.size() || 0 < m_vRenderGlassMeshIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	// RENDER_EFFECT 돼야하는 그룹
	if (0 < m_vDecalLightMeshIndex.size() || 0 < m_vLampMeshIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);

	// GLASS, DECAL 같이 처리 - COLOR 값만 넣는애들
	if (0 < m_vDecalBlendMeshIndex.size() || 0 < m_vCompulsoryDecalBlendMeshIndex.size())
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_DECAL, this);
	}

	if (0 < m_vRenderGlassMeshIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_GLASS, this);

	// 빛 영향 안받고 원색값 유지
	if (0 < m_vSignMeshIndex.size() || 0 < m_vMaskSignIndex.size() || 0 < m_vDynamicSignIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT_NONBLUR, this);

	//Bloom
	if (0 < m_vBloomIndex.size() || 0 < m_vSignMeshIndex.size() || 0 < m_vLampMeshIndex.size() || 0 < m_vStrongBloomIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);


	for (auto& iter : m_vDecals)
		iter->Late_Tick(fTimeDelta);

}

void CMap::Add_Renderer_Far(const _float& fTimeDelta)
{
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();





	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_float4x4 vLocalMatrix = Meshes[i]->Get_LocalMatrix();

		XMVECTOR localPos = XMVectorSet(vLocalMatrix._41, vLocalMatrix._42, vLocalMatrix._43, 1);
		XMVECTOR worldPos = XMVectorZero();
		worldPos = XMVector3Transform(localPos, m_pTransformCom->Get_WorldMatrix());

		string meshName = Meshes[i]->Get_Name();

		// 점 위치 찾기
		size_t dotPos = meshName.find('.');

		// 점이 있으면 점 이전 부분을 추출하고, 없으면 전체 문자열을 사용
		string baseName = (dotPos != string::npos) ? meshName.substr(0, dotPos) : meshName;

		const char* baseNameCStr = baseName.c_str();

		if (false == m_bCull)
		{
			// 컬링안함
			if (0 == strcmp(baseNameCStr, "DEFAULTMESH"))
			{
				m_vRenderDefaulMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "GLASSMESH") || 0 == strcmp(baseNameCStr, "DECALMESH")
				|| 0 == strcmp(baseNameCStr, "DECALBLENDMESH") || 0 == strcmp(baseNameCStr, "COMBLENDDECAL"))
			{
			}
			else if (0 == strcmp(baseNameCStr, "DECALLIGHTMESH"))
			{
				m_vDecalLightMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "SIGNMESH"))
			{
				m_vSignMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "LAMPMESH"))
			{
				// Nonblend + effect 둘 다 호출
				m_vLampMeshIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "BLOOMMESH"))
			{
				m_vBloomIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "MASKSIGNMESH"))
			{
				m_vMaskSignIndex.push_back(i);
			}
			else if (0 == strcmp(baseNameCStr, "STRONGBLOOM"))
			{
				m_vStrongBloomIndex.push_back(i);
			}
			else if (strstr(baseNameCStr, "DYNAMICSIGN") != nullptr)
			{
				m_vDynamicSignIndex.push_back(i);
			}
			else
			{
				// mesh 안합쳐놓으면 그냥 default로 처리
				m_vRenderDefaulMeshIndex.push_back(i);
			}
		}
		else
		{
			float		fScale = Meshes[i]->Get_MeshScale();

			if (fScale < 15)
				fScale = 25;

			// 컬링
			if (true == m_pGameInstance->isIn_WorldFrustum(worldPos, fScale * 1.5))
			{
				if (0 == strcmp(baseNameCStr, "DEFAULTMESH"))
				{
					m_vRenderDefaulMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "GLASSMESH") || 0 == strcmp(baseNameCStr, "DECALMESH")
					|| 0 == strcmp(baseNameCStr, "DECALBLENDMESH") || 0 == strcmp(baseNameCStr, "COMBLENDDECAL"))
				{
				}
				else if (0 == strcmp(baseNameCStr, "SIGNMESH"))
				{
					m_vSignMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "DECALLIGHTMESH"))
				{
					m_vDecalLightMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "LAMPMESH"))
				{
					// Nonblend + effect 둘 다 호출
					m_vLampMeshIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "BLOOMMESH"))
				{
					m_vBloomIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "MASKSIGNMESH"))
				{
					m_vMaskSignIndex.push_back(i);
				}
				else if (0 == strcmp(baseNameCStr, "STRONGBLOOM"))
				{
					m_vStrongBloomIndex.push_back(i);
				}
				else if (strstr(baseNameCStr, "DYNAMICSIGN") != nullptr)
				{
					m_vDynamicSignIndex.push_back(i);
				}
				else
				{
					// mesh 안합쳐놓으면 그냥 default로 처리
					m_vRenderDefaulMeshIndex.push_back(i);
				}
			}
		}

	}

	// RENDER_NONBLEND 돼야하는 그룹
	if (0 < m_vRenderDefaulMeshIndex.size() || 0 < m_vSignMeshIndex.size() || 0 < m_vLampMeshIndex.size()
		|| 0 < m_vBloomIndex.size() || 0 < m_vMaskMeshIndex.size() || 0 < m_vStrongBloomIndex.size() || 0 < m_vRenderGlassMeshIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	// RENDER_EFFECT 돼야하는 그룹
	if (0 < m_vDecalLightMeshIndex.size() || 0 < m_vLampMeshIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);

	// GLASS, DECAL 같이 처리 - COLOR 값만 넣는애들
	if (0 < m_vDecalBlendMeshIndex.size() || 0 < m_vCompulsoryDecalBlendMeshIndex.size())
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_DECAL, this);
	}


	// 빛 영향 안받고 원색값 유지
	if (0 < m_vSignMeshIndex.size() || 0 < m_vMaskSignIndex.size() || 0 < m_vDynamicSignIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT_NONBLUR, this);

	//Bloom
	if (0 < m_vBloomIndex.size() || 0 < m_vSignMeshIndex.size() || 0 < m_vLampMeshIndex.size() || 0 < m_vStrongBloomIndex.size())
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);


	for (auto& iter : m_vDecals)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CMap::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//모델 이름 추출
	string strModelName = m_pGameInstance->WstringToString(gameobjDesc->wstrModelName);
	string strRemoveName = "Prototype_Component_Model_";
	_int iPos = strModelName.find(strRemoveName);
	
	if (iPos == string::npos)
		return E_FAIL;

	strModelName = strModelName.erase(iPos, strRemoveName.size());

	const char* gnrName = strModelName.c_str();
	
	if (strModelName == "doujima_gaikan")
	{
		m_bCompulsoryAlpha = { true };
	}
	else if (strstr(gnrName, "gnr_") != nullptr)
	{
		m_bExcludeSSAO = true;
	}
	else if (strstr(gnrName, "gnd") != nullptr)
	{
		m_bExcludeSSAO = true;
	}
	else if (strstr(gnrName, "road") != nullptr)
	{
		m_bExcludeSSAO = true;
	}

	wstring strMaterialName = TEXT("Prototype_Component_Material_") + m_pGameInstance->StringToWstring(strModelName);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, strMaterialName,
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		m_pMaterialCom = nullptr;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_MeshMap"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Occulusion Culling을 위한 scale 파악
	if (0 < gameobjDesc->iColliderNum)
	{

		m_CubeWorldMatrix = XMMatrixIdentity();
		m_CubeWorldMatrix.r[3].m128_f32[0] = gameobjDesc->vStartPos.r[3].m128_f32[0] + gameobjDesc->pColliderDesc[0].vCenter.x;
		m_CubeWorldMatrix.r[3].m128_f32[1] = gameobjDesc->vStartPos.r[3].m128_f32[1] + gameobjDesc->pColliderDesc[0].vCenter.y;
		m_CubeWorldMatrix.r[3].m128_f32[2] = gameobjDesc->vStartPos.r[3].m128_f32[2] + gameobjDesc->pColliderDesc[0].vCenter.z;

		m_CubeWorldMatrix.r[0].m128_f32[0] = gameobjDesc->pColliderDesc[0].vExtents.x * 2;
		m_CubeWorldMatrix.r[1].m128_f32[1] = gameobjDesc->pColliderDesc[0].vExtents.y * 2;
		m_CubeWorldMatrix.r[2].m128_f32[2] = gameobjDesc->pColliderDesc[0].vExtents.z * 2;
		
		m_bOcculusionCulling = true;
	}
	else
	{
		m_bOcculusionCulling = false;
	}

	
	// Occulusion Culling을 위한
	/* For.Com_SubModel */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_AABBCube"),
		TEXT("Com_SubModel"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_SubShader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxCube_Occulusion"),
		TEXT("Com_SubShader"), reinterpret_cast<CComponent**>(&m_pCubeShaderCom))))
		return E_FAIL;

	/* For.Com_SubShader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_OcculusionCulling"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
		return E_FAIL;


	XMMATRIX worldProjView = m_CubeWorldMatrix * m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) * m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
	CMap::ObjectData objectData = { worldProjView, *m_pGameInstance->Get_CamFar()};

	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.ByteWidth = sizeof(ObjectData);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &objectData;

	if (FAILED(m_pDevice->CreateBuffer(&cbDesc, &initData, &m_pObjectDataBuffer)))
	{
		return E_FAIL;
	}


	D3D11_BUFFER_DESC stagingDesc = {};
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	int num = m_pVIBufferCom->Get_NumVertex();
	stagingDesc.ByteWidth = sizeof(int) * num;  // g_OutputBuffer의 크기에 맞춰야 함
	stagingDesc.BindFlags = 0;  // Staging 버퍼는 바인딩 플래그가 필요 없음
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateBuffer(&stagingDesc, nullptr, &m_pOutputBufferStaging)))
	{
		// 오류 처리
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMap::Bind_ShaderResources()
{ 
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



	return S_OK;
}

HRESULT CMap::Reset_Bind()
{
	_bool isRS = false;
	_bool isRD = false;
	_bool isRM = false;
	_bool isRT = false;
	_bool isMulti = false;
	_bool isUVShader = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMulti", &isMulti, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isUVShader", &isUVShader, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CMap* CMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMap* pInstance = new CMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMap::Clone(void* pArg)
{
	CMap* pInstance = new CMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap::Free()
{
	__super::Free();

	for (auto& iter : m_vDecals)
		Safe_Release(iter);
	m_vDecals.clear();

	for (auto& iter : m_vColliders)
		Safe_Release(iter);
	m_vColliders.clear();

	m_vRenderDefaulMeshIndex.clear();
	m_vRenderGlassMeshIndex.clear();
	m_vMaskMeshIndex.clear();
	m_vDecalLightMeshIndex.clear();
	m_vSignMeshIndex.clear();
	m_vLampMeshIndex.clear();
	m_vDecalBlendMeshIndex.clear();
	m_vBloomIndex.clear();
	m_vMaskSignIndex.clear();
	m_vDynamicSignIndex.clear();
	m_vStrongBloomIndex.clear();
	m_vCompulsoryDecalBlendMeshIndex.clear();

	Safe_Release(m_pComputeShaderCom);
	Safe_Release(m_pCubeShaderCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pObjectDataBuffer);
	Safe_Release(m_pOutputBufferStaging);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pSystemManager);

	Safe_Release(m_pOcclusionQuery);
}
