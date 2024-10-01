#include "..\Public\Construction.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Imgui_Manager.h"
#include "ObjPlace_Manager.h"
#include "Mesh.h"
#include "Navigation_Manager.h"
#include "Collider.h"

CConstruction::CConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CConstruction::CConstruction(const CConstruction& rhs)
	: CGameObject{ rhs }
{
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
		m_iObjectPropertyType = gameobjDesc->iObjPropertyType;
		m_iNaviNum = gameobjDesc->iNaviNum;
		m_iRouteNum = gameobjDesc->iRouteNum;
		m_vOffsetMatrix = gameobjDesc->vOffsetMatrix;
		m_iNPCDirection = gameobjDesc->iNPCDirection;
		m_iGroupMonster = gameobjDesc->iGroupMonster;
		m_iGroupNum = gameobjDesc->iGroupNum;

		for (int i = 0; i < gameobjDesc->iDecalNum; i++)
		{
			DECAL_DESC  tDecal;
			CTexture*	pTexture;
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

			CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(LEVEL_RUNMAP, TEXT("Prototype_Component_Collider"), &ColliderDesc));

			m_vColliders.push_back(pCollider);

			m_vColliderDesc.push_back(objDesc);
		}
	}


	return S_OK;
}

void CConstruction::Priority_Tick(const _float& fTimeDelta)
{
}

void CConstruction::Tick(const _float& fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& iter : m_vDecals)
		iter->Tick(fTimeDelta);


#ifdef _DEBUG
	for (auto& iter : m_vColliders)
		iter->Tick(m_pTransformCom->Get_WorldMatrix());
#endif

}

void CConstruction::Late_Tick(const _float& fTimeDelta)
{
	bool bRender = false;
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	// vector 비워주고 시작
	m_vRenderMeshIndexes.clear();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_float4x4 vLocalMatrix = Meshes[i]->Get_LocalMatrix();

		XMVECTOR localPosition = XMVectorSet(vLocalMatrix._41, vLocalMatrix._42, vLocalMatrix._43, 1);

		if (true == m_pGameInstance->isIn_LocalFrustum(localPosition, 5.f))
		{
			m_vRenderMeshIndexes.push_back(i);
		}

	}

	/*for (auto& iter : m_vDecals)
		iter->Late_Tick(fTimeDelta);*/

	XMVECTOR worldPos = XMVectorZero();
	worldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//if (true == m_pGameInstance->isIn_WorldFrustum(worldPos, 20))
	//{
	//	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	//}

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

}

HRESULT CConstruction::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (!strcmp(Meshes[i]->Get_Name(), "box4783"))
		{
			int a = 10;
		}

		/*m_pShaderCom->Begin(m_iShaderPassNum);*/

		bool	bFindDecal = false;

		if (true == m_bFindDecalMesh)
		{
			for (int j = 0; j < m_iDecalMeshCnt; j++)
			{
				if (m_pDecalMeshIndex[j] == i)
				{
					bFindDecal = true;
				}
			}
		}

		if (true == bFindDecal)
		{

			/* decal이 포함된 메쉬는 빨강색으로 */
			m_pShaderCom->Begin(1);
		}
		else
		{
			m_pShaderCom->Begin(0);
		}

		m_pModelCom->Render(i);
	}


	//vector<CMesh*> Meshes = m_pModelCom->Get_Meshes();

	//for (size_t i = 0; i < m_vRenderMeshIndexes.size(); i++)
	//{
	//	int		iMeshIndex = m_vRenderMeshIndexes[i];

	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", iMeshIndex, aiTextureType_DIFFUSE)))
	//		return E_FAIL;

	//	if (!strcmp(Meshes[iMeshIndex]->Get_Name(), "box4783"))
	//	{
	//		int a = 10;
	//	}

	//	/*m_pShaderCom->Begin(m_iShaderPassNum);*/

	//	bool	bFindDecal = false;

	//	if (true == m_bFindDecalMesh)
	//	{
	//		for (int j = 0; j < m_iDecalMeshCnt; j++)
	//		{
	//			if (m_pDecalMeshIndex[j] == iMeshIndex)
	//			{
	//				bFindDecal = true;
	//			}
	//		}
	//	}

	//	if (true == bFindDecal)
	//	{

	//		/* decal이 포함된 메쉬는 빨강색으로 */
	//		m_pShaderCom->Begin(1);
	//	}
	//	else
	//	{
	//		m_pShaderCom->Begin(0);
	//	}

	//	m_pModelCom->Render(iMeshIndex);
	//}


#ifdef _DEBUG

	for (auto& iter : m_vColliders)
		m_pGameInstance->Add_DebugComponent(iter);
#endif

	return S_OK;
}

int CConstruction::Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc)
{
	/* 바이너리화하기 위한 데이터 제공 */
	
	XMStoreFloat4x4(&objplaceDesc->vTransform, m_pTransformCom->Get_WorldMatrix());

	//objPlaceDesc.iLayer = m_iLayerNum;
	
	string strName = m_pGameInstance->WstringToString(m_wstrModelName);
	strcpy(objplaceDesc->strModelCom, strName.c_str());

	objplaceDesc->iShaderPassNum = m_iShaderPassNum;
	objplaceDesc->iObjType = m_iObjectType;
	objplaceDesc->iObjPropertyType = m_iObjectPropertyType;
	objplaceDesc->iNPCDirection = m_iNPCDirection;
	objplaceDesc->iGroupMonster = m_iGroupMonster;
	objplaceDesc->iGroupNum = m_iGroupNum;

	/*if ((int)CObjPlace_Manager::OBJECT_TYPE::PLAYER == m_iObjectType || (int)CObjPlace_Manager::OBJECT_TYPE::MONSTER_KUZE == m_iObjectType
		|| (int)CObjPlace_Manager::OBJECT_TYPE::MONSTER_RUSH == m_iObjectType || (int)CObjPlace_Manager::OBJECT_TYPE::MONSTER_SHAKEDOWN == m_iObjectType
		|| (int)CObjPlace_Manager::OBJECT_TYPE::MONSTER_WPA == m_iObjectType || (int)CObjPlace_Manager::OBJECT_TYPE::MONSTER_YONEDA == m_iObjectType
		|| (int)CObjPlace_Manager::OBJECT_TYPE::ADTENTURE_SUIT == m_iObjectType || (int)CObjPlace_Manager::OBJECT_TYPE::ITEM == m_iObjectType)
	{
		CNavigation_Manager* pNaviMgr = CNavigation_Manager::GetInstance();
		Safe_AddRef(pNaviMgr);

		int		iNaviNum = pNaviMgr->Get_Player_Monster_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_iNaviNum = 0;

		if (-1 != iNaviNum)
		{
			m_iNaviNum = iNaviNum;
		}

		objplaceDesc->iNaviNum = m_iNaviNum;
		

		Safe_Release(pNaviMgr);
	}
	else
	{
		objplaceDesc->iNaviNum = 0;
	}*/

	CNavigation_Manager* pNaviMgr = CNavigation_Manager::GetInstance();
	Safe_AddRef(pNaviMgr);

	int		iNaviNum = pNaviMgr->Get_Player_Monster_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_iNaviNum = 0;

	if (-1 != iNaviNum)
	{
		m_iNaviNum = iNaviNum;
	}

	objplaceDesc->iNaviNum = m_iNaviNum;


	Safe_Release(pNaviMgr);
	
	objplaceDesc->iNaviRoute = m_iRouteNum;
	objplaceDesc->vOffsetTransform = m_vOffsetMatrix;
	

	/* Decal 추가 */
	objplaceDesc->iDecalNum = m_vDecals.size();
	
	if (0 < objplaceDesc->iDecalNum)
	{
		objplaceDesc->pDecals = new DECAL_DESC_IO[objplaceDesc->iDecalNum];

		for (int i = 0; i < objplaceDesc->iDecalNum ; i++)
		{
			m_vDecals[i]->Get_Decal_Desc_IO(&objplaceDesc->pDecals[i]);
		}
	}

	objplaceDesc->iColliderNum = m_vColliders.size();

	if (0 < objplaceDesc->iColliderNum)
	{
		objplaceDesc->pObjColliders = new OBJCOLLIDER_DESC[objplaceDesc->iColliderNum];

		for (int i = 0; i < objplaceDesc->iColliderNum ; i++)
		{
			objplaceDesc->pObjColliders[i] = m_vColliderDesc[i];
		}
	}


	/* layer는 return 형식으로 */
	return m_iLayerNum;
}

CConstruction::MAPOBJ_DESC CConstruction::Get_MapObjDesc_For_AddList()
{
	CConstruction::MAPOBJ_DESC		mapobjDesc;
	mapobjDesc.vStartPos = m_pTransformCom->Get_WorldMatrix();
	mapobjDesc.wstrModelName = m_wstrModelName;
	mapobjDesc.iShaderPass = m_iShaderPassNum;
	mapobjDesc.vOffsetMatrix = m_vOffsetMatrix;
	mapobjDesc.iObjPropertyType = m_iObjectPropertyType;
	mapobjDesc.iNPCDirection = m_iNPCDirection;
	mapobjDesc.iGroupMonster = m_iGroupMonster;
	mapobjDesc.iGroupNum = m_iGroupNum;

	return mapobjDesc;
}

void CConstruction::Edit_GameObject_Information(CConstruction::MAPOBJ_DESC mapDesc)
{
	m_iLayerNum = mapDesc.iLayer;
	m_iShaderPassNum = mapDesc.iShaderPass;
	m_iObjectType = mapDesc.iObjType;
	m_iObjectPropertyType = mapDesc.iObjPropertyType;
	m_iRouteNum = mapDesc.iRouteNum;
	m_vOffsetMatrix = mapDesc.vOffsetMatrix;
	m_iNPCDirection = mapDesc.iNPCDirection;
	m_iGroupMonster = mapDesc.iGroupMonster;
	m_iGroupNum = mapDesc.iGroupNum;
}

CConstruction::MAPOBJ_DESC CConstruction::Send_GameObject_Information()
{
	MAPOBJ_DESC		mapObjDesc;

	mapObjDesc.iLayer = m_iLayerNum;
	mapObjDesc.iShaderPass = m_iShaderPassNum;
	mapObjDesc.iObjType = m_iObjectType;
	mapObjDesc.iObjPropertyType = m_iObjectPropertyType;
	mapObjDesc.iRouteNum = m_iRouteNum;
	mapObjDesc.vOffsetMatrix = m_vOffsetMatrix;
	mapObjDesc.iNPCDirection = m_iNPCDirection;
	mapObjDesc.iGroupMonster = m_iGroupMonster;
	mapObjDesc.iGroupNum = m_iGroupNum;

	return mapObjDesc;
}

void CConstruction::On_Find_DecalMesh(int* iNumMesh, int iCnt)
{
	m_pDecalMeshIndex = iNumMesh;
	m_iDecalMeshCnt = iCnt;
	m_bFindDecalMesh = true;
}

void CConstruction::Off_Find_DecalMesh()
{
	m_bFindDecalMesh = false;
}

void CConstruction::Add_Decal(CDecal* pDecal)
{
	m_vDecals.push_back(pDecal);
	Safe_AddRef(pDecal);
}

void CConstruction::Delete_Decal(int iIndex)
{
	if (1 == m_vDecals.size())
	{
		vector<CDecal*>::iterator		iter = m_vDecals.begin();

		if (0 != iIndex)
		{
			for (int i = 0; i < iIndex; i++)
			{
				iter++;
			}
		}

		Safe_Release(*iter);
		m_vDecals.erase(iter);

		m_vDecals.clear();
	}
	else
	{
		vector<CDecal*>::iterator		iter = m_vDecals.begin();

		if (0 != iIndex)
		{
			for (int i = 0; i < iIndex; i++)
			{
				iter++;
			}
		}

		Safe_Release(*iter);
		m_vDecals.erase(iter);
	}


}

void CConstruction::Add_Collider(OBJCOLLIDER_DESC tCollider)
{
	//CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	//ColliderDesc.eType = (CCollider::TYPE)tCollider.iColliderType;
	//ColliderDesc.vExtents = tCollider.vExtents;
	//ColliderDesc.vCenter = tCollider.vCenter;
	//ColliderDesc.vRotation = tCollider.vQuaternion;

	//CCollider*	pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(LEVEL_RUNMAP, TEXT("Prototype_Component_Collider"), &ColliderDesc));

	//m_vColliders.push_back(pCollider);

	//m_vColliderDesc.push_back(tCollider);


	if (CCollider::TYPE::COLLIDER_SPHERE == (CCollider::TYPE)tCollider.iColliderType)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_SPHERE;
		ColliderDesc.fRadius = tCollider.vExtents.x;
		ColliderDesc.vCenter = tCollider.vCenter;

		CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

		m_vColliders.push_back(pCollider);
	}
	else if (CCollider::TYPE::COLLIDER_AABB == (CCollider::TYPE)tCollider.iColliderType)
	{
		CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};
		ColliderDesc.eType = CCollider::COLLIDER_AABB;
		ColliderDesc.vExtents = tCollider.vExtents;
		ColliderDesc.vCenter = tCollider.vCenter;

		CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

		m_vColliders.push_back(pCollider);
	}
	else
	{
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_OBB;
		ColliderDesc.vExtents = tCollider.vExtents;
		ColliderDesc.vCenter = tCollider.vCenter;
		ColliderDesc.vRotation = tCollider.vQuaternion;

		CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Add_Component_Clone(m_iCurrentLevel, TEXT("Prototype_Component_Collider"), &ColliderDesc));

		m_vColliders.push_back(pCollider);
	}

	m_vColliderDesc.push_back(tCollider);
}

void CConstruction::Rewrite_Collider(OBJCOLLIDER_DESC tCollider, int iIndex)
{

}

void CConstruction::Delete_Collider(int iIndex)
{
	if (1 == m_vColliders.size())
	{
		vector<CCollider*>::iterator		iter = m_vColliders.begin();
		vector<OBJCOLLIDER_DESC>::iterator iterDesc = m_vColliderDesc.begin();

		if (0 != iIndex)
		{
			for (int i = 0; i < iIndex; i++)
			{
				iter++;
				iterDesc++;
			}
		}

		Safe_Release(*iter);
		m_vColliders.erase(iter);

		m_vColliders.clear();


		m_vColliderDesc.erase(iterDesc);

		m_vColliderDesc.clear();
	}
	else
	{
		vector<CCollider*>::iterator		iter = m_vColliders.begin();
		vector<OBJCOLLIDER_DESC>::iterator iterDesc = m_vColliderDesc.begin();

		if (0 != iIndex)
		{
			for (int i = 0; i < iIndex; i++)
			{
				iter++;
				iterDesc++;
			}
		}

		Safe_Release(*iter);
		m_vColliders.erase(iter);

		m_vColliderDesc.erase(iterDesc);
	}
}

void CConstruction::Delete_AllCollider()
{
	for (auto& iter : m_vColliders)
		Safe_Release(iter);
	m_vColliders.clear();


	m_vColliderDesc.clear();
}



HRESULT CConstruction::Add_Components(void* pArg)
{
	MAPOBJ_DESC* gameobjDesc = (MAPOBJ_DESC*)pArg;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, gameobjDesc->wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	


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

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fObjID", m_iObjectIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fFar", *m_pGameInstance->Get_CamFar())))
		return E_FAIL;

	bool	bWrite;
	if (CImgui_Manager::IDWRIE::OBJPLACE == CImgui_Manager::GetInstance()->Get_Write())
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

	m_vColliderDesc.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);


	m_vRenderMeshIndexes.clear();
}
