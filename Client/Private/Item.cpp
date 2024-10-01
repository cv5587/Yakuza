#include "..\Public\Item.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Collision_Manager.h"
#include "Transform.h"
#include "Player.h"
#include "Mesh.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pCollisionManager{ CCollision_Manager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
}

CItem::CItem(const CItem& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager },
	m_pCollisionManager{ rhs.m_pCollisionManager }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CItem::Initialize(void* pArg)
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

		if (nullptr != m_pNavigationCom)
			m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);

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
			//COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE
			if (CCollider::TYPE::COLLIDER_SPHERE == (CCollider::TYPE)gameobjDesc->pColliderDesc[i].iColliderType)
			{
				/*OBJCOLLIDER_DESC		objDesc = gameobjDesc->pColliderDesc[i];

				CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

				ColliderDesc.eType = CCollider::COLLIDER_SPHERE;
				ColliderDesc.fRadius = objDesc.vExtents.x;
				ColliderDesc.vCenter = objDesc.vCenter;

				CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

				m_vColliders.push_back(pCollider);*/
			}
			else if (CCollider::TYPE::COLLIDER_AABB == (CCollider::TYPE)gameobjDesc->pColliderDesc[i].iColliderType)
			{
				OBJCOLLIDER_DESC		objDesc = gameobjDesc->pColliderDesc[i];

				CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};
				ColliderDesc.eType = CCollider::COLLIDER_AABB;
				ColliderDesc.vExtents = objDesc.vExtents;
				ColliderDesc.vCenter = objDesc.vCenter;

				m_pColliderCom = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

				/*m_vColliders.push_back(pCollider);*/
			}
			else
			{
				OBJCOLLIDER_DESC		objDesc = gameobjDesc->pColliderDesc[i];

				CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

				ColliderDesc.eType = CCollider::COLLIDER_OBB;
				ColliderDesc.vExtents = objDesc.vExtents;
				ColliderDesc.vCenter = objDesc.vCenter;
				ColliderDesc.vRotation = objDesc.vQuaternion;

				m_pOBBColliderCom = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

				/*m_vColliders.push_back(pCollider);*/
			}


		}

		// 플레이어 matrix 추가
		m_pPlayerMatrix = gameobjDesc->vPlayerMatrix;

		// 잠깐 초기화해두기 -> 나중에 OFFSET으로 사용 (플레이어어와 충돌했을 경우 위치 조절)
		m_vOffsetMatrix = gameobjDesc->vOffsetMatrix;

		m_WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	}

	m_Casecade = { 0.f, 10.f, 24.f, 40.f };

	m_iLife = 3;
	m_eItemMode = ITEM_IDLE;

	return S_OK;
}

void CItem::Priority_Tick(const _float& fTimeDelta)
{
}

void CItem::Tick(const _float& fTimeDelta)
{
	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);

	// 잡을 수 있을때와 아닐때를 구별
	Set_Item_Mode();

	// Bright 처리
	bool		bBright = m_bBright;

	if (true == m_bDissovle)
	{
		m_fDissolveTime += fTimeDelta * 0.8;

		if(3 < m_fDissolveTime)
		{
			m_isDead = true;
		}
	}
	else if (0 != m_fBrightTime && ITEM_BRIGHT != m_eItemMode)
	{
		// bright 모드 아닌데 아직 깜빡거림이 안끝났을때
		m_bBright = true;

		m_fBrightTime += 0.04;

		if (2 < m_fBrightTime)
		{
			m_fBrightTime = 0;
			m_bBright = false;
		}
	}
	else if (ITEM_BRIGHT == m_eItemMode)
	{
		// bright 모드 
		m_bBright = true;

		m_fBrightTime += 0.02;

		if (2 < m_fBrightTime)
		{
			m_fBrightTime = 0;
		}
	}
	else if (0 == m_fBrightTime && ITEM_BRIGHT != m_eItemMode)
	{
		// bright 모드 아님
		m_bBright = false;
	}
	else if (true == m_bCurGrab)
	{
		// grab이면 바로 꺼짐
		m_bBright = false;
		m_fBrightTime = 0;
	}



	if (ITEM_GRAB == m_eItemMode)
	{
		// 잡고 있을때 
		_matrix		offsetMatrix = XMMatrixIdentity();

		_matrix PlayerMatrix, ParentMatrix;
		PlayerMatrix = ParentMatrix = XMMatrixIdentity();

		if (nullptr != m_pPlayerMatrix)
			PlayerMatrix = XMLoadFloat4x4(m_pPlayerMatrix);

		if (nullptr != m_vParentMatrix)
			ParentMatrix = XMLoadFloat4x4(m_vParentMatrix);

		ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
		ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
		ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);

		// 플레이어의 회전 행렬에서 Y축 회전(방향) 추출
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
		XMVECTOR playerForward = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		XMVECTOR playerRight = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT);
		XMVECTOR playerUp = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_UP);


		float forwardOffset = m_vOffsetMatrix._41;
		float rightOffset = m_vOffsetMatrix._43;
		float upOffset = m_vOffsetMatrix._42;

		XMFLOAT4X4 tempOffsetMatrix;
		XMStoreFloat4x4(&tempOffsetMatrix, offsetMatrix);

		// 플레이어의 방향에 맞춰 오프셋을 적용
		tempOffsetMatrix._41 += forwardOffset * XMVectorGetX(playerForward);
		tempOffsetMatrix._43 += forwardOffset * XMVectorGetZ(playerForward);
		tempOffsetMatrix._41 += rightOffset * XMVectorGetX(playerRight);
		tempOffsetMatrix._43 += rightOffset * XMVectorGetZ(playerRight);
		tempOffsetMatrix._42 += upOffset * XMVectorGetY(playerUp);

		offsetMatrix = XMLoadFloat4x4(&tempOffsetMatrix);

		//m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix)
		XMStoreFloat4x4(&m_WorldMatrix, ParentMatrix * PlayerMatrix * offsetMatrix);

		XMMATRIX worldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		m_pTransformCom->Set_WorldMatrix(worldMatrix);
	}
	else
	{
		//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));
	}

	if (m_isThrowing)
		Throwing(fTimeDelta);	

	_matrix		worldMatrix = m_pTransformCom->Get_WorldMatrix();

#ifdef _DEBUG
	//for (auto& iter : m_vColliders)
	//	iter->Tick(worldMatrix);


	if (nullptr != m_pColliderCom)
	{
		m_pColliderCom->Tick(worldMatrix);
	}

	if (nullptr != m_pOBBColliderCom)
	{
		m_pOBBColliderCom->Tick(worldMatrix);
	}
#endif

}

void CItem::Late_Tick(const _float& fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT_NONBLUR, this);

		for (auto& iter : m_vDecals)
			iter->Late_Tick(fTimeDelta);
	}

	// 네비문제 해결되고 주석해제해야 밀림
	//m_pCollisionManager->Add_ImpulseResolution(this);

	if (m_isAttacking && nullptr != m_pOBBColliderCom)
		m_pCollisionManager->Add_ItemCollider(m_pOBBColliderCom);

}

HRESULT CItem::Render()
{
#ifdef _DEBUG
	//for (auto& iter : m_vColliders)
	//	m_pGameInstance->Add_DebugComponent(iter);

	if (nullptr != m_pColliderCom)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}

	if (nullptr != m_pOBBColliderCom)
	{
		m_pGameInstance->Add_DebugComponent(m_pOBBColliderCom);
	}
#endif

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	int		iRenderState = m_pGameInstance->Get_RenderState();

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();

	if (iRenderState == CRenderer::RENDER_NONBLENDER)
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (false == m_bBright)
			{
				_bool fFar = m_pGameInstance->Get_CamFar();
				m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
					return E_FAIL;
				m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

				_bool isRS = true;
				_bool isRD = true;
				_bool isRM = true;
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

				m_pShaderCom->Begin(SHADER_DEFAULT);

				m_pModelCom->Render(i);
			}
			
		}
	}
	else if (iRenderState == CRenderer::RENDER_NONLIGHT_NONBLUR)
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (true != m_bDissovle)
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_bBright", &m_bBright, sizeof(_bool))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightTime", &m_fBrightTime, sizeof(_float))))
					return E_FAIL;

				m_pShaderCom->Begin(SHADER_DEFAULT_BRIGHT);
			}
			else
			{
				
				m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0);

				if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fDissolveTime", m_fDissolveTime)))
					return E_FAIL;

				m_pShaderCom->Begin(SHADER_DISSOLVE);
				
			}

			m_pModelCom->Render(i);
		}

	}
	
	

	return S_OK;
}

HRESULT CItem::Render_LightDepth()
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

void CItem::ImpulseResolution(CGameObject* pTargetObject, _float fDistance)
{
	if (nullptr == m_pColliderCom) return;

	_float3 vDir = m_pColliderCom->ImpulseResolution(pTargetObject->Get_Collider(), fDistance);

	if (!XMVector3Equal(XMLoadFloat3(&vDir), XMVectorZero()))
	{
		_vector vMovePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMLoadFloat3(&vDir));

		//CNavigation* pTargetNavi = dynamic_cast<CNavigation*>(pTargetObject->Get_Component(TEXT("Com_Navigation")));
		if (nullptr == m_pNavigationCom)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMovePos);
		else
		{
			// 네비 밖인지 아닌지 검사해야함.
			if (m_pNavigationCom->isMove(vMovePos))
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMovePos);
		}
	}
}

void CItem::Attack_Event(CGameObject* pHitObject, _bool isItem)
{

}

_bool CItem::Decrease_Life()
{
	m_iLife--;

	// 감소 이후 생명이 0보다 작아진다면 오브젝트 사망처리
	if (0 >= m_iLife)
	{
		Set_ObjectDead();
		//Dissolve 시작
		m_bDissovle = true;
		return false;
	}

	return true;
}

void CItem::Throw_On(THROW_INFO_DESC& Desc)
{
	if (m_isThrowing) return;

	m_eItemMode = ITEM_IDLE;

	m_isThrowing = true;
	m_ThrowInfo.fThrowSpeed = Desc.fThrowSpeed;
	m_ThrowInfo.vThrowDir = Desc.vThrowDir;

	m_pPlayerMatrix = nullptr;
	m_vParentMatrix = nullptr;

	// Dissolve 시작
	m_bDissovle = true;
	Set_ObjectDead();
}

void CItem::Throwing(const _float& fTimeDelta)
{
	m_fThrowTimer += fTimeDelta;
	if (THROW_TIME <= m_fThrowTimer)
	{
		m_fThrowTimer = 0.f;
		m_isThrowing = false;


		// TODO: 오브젝트를 죽음처리하면 디졸브를 실행해야한다.
		Set_ObjectDead();
	}

	m_pTransformCom->Go_Move_Custum(m_ThrowInfo.vThrowDir, m_ThrowInfo.fThrowSpeed, fTimeDelta);
	//m_pTransformCom->Go_Straight(fTimeDelta);
}

void CItem::Set_Item_Mode()
{
	if (nullptr == m_pPlayerMatrix) return;

	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

	float fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPlayerPosition));

	// 현재 잡고 있지 않음 + 근처에 있을때
	if (fDistance < BRIGHT_DISTANCE && false == m_bCurGrab)
	{	
		m_bBright = true;
		
		m_eItemMode = ITEM_BRIGHT;			
	}
	else if (false == m_bCurGrab)
	{
		// 잡고 있지도 않고 근처에 없을때
		m_eItemMode = ITEM_IDLE;

		m_bBright = false;
	}

}

HRESULT CItem::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_MeshItem"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;


	/* For.Com_SubTexture */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Texture_Dissolve_0"),
		TEXT("Com_SubTexture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
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

CItem* CItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem* pInstance = new CItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();

	for (auto& iter : m_vDecals)
		Safe_Release(iter);
	m_vDecals.clear();

	//for (auto& iter : m_vColliders)
	//	Safe_Release(iter);
	//m_vColliders.clear();


	Safe_Release(m_pDissolveTextureCom);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pOBBColliderCom);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pSystemManager);
	Safe_Release(m_pCollisionManager);
}
