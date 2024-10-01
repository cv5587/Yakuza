#include "..\Public\ColliderObj.h"
#include "Imgui_Manager.h"
#include "Bounding_OBB.h"

CColliderObj::CColliderObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CColliderObj::CColliderObj(const CColliderObj& rhs)
	: CGameObject{ rhs }
{

}


HRESULT CColliderObj::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CColliderObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		COLLIDEROBJ_DESC* colliderDesc = (COLLIDEROBJ_DESC*)pArg;
		/* For.Prototype_Component_Texture_Terrain */

		if (true == colliderDesc->bLoad)
		{
			// 로드 후 설치
			m_pTransformCom->Set_WorldMatrix(colliderDesc->vStartPos);
			m_WorldMatrix = XMMatrixIdentity();

			XMStoreFloat4x4(&m_ColliderWorldMatrix, m_WorldMatrix);


			float pitch = colliderDesc->vQuaternion.x;
			float yaw = colliderDesc->vQuaternion.y;
			float roll = colliderDesc->vQuaternion.z;

			XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
			XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);

			XMStoreFloat4x4(&m_ColliderWorldMatrix, rotationMatrix);

			m_ColliderWorldMatrix._41 = colliderDesc->vStartPos.r[3].m128_f32[0];
			m_ColliderWorldMatrix._42 = colliderDesc->vStartPos.r[3].m128_f32[1];
			m_ColliderWorldMatrix._43 = colliderDesc->vStartPos.r[3].m128_f32[2];
			m_ColliderWorldMatrix._44 = 1;

			m_ColliderWorldMatrix._11 = colliderDesc->vExtents.x;
			m_ColliderWorldMatrix._22 = colliderDesc->vExtents.y;
			m_ColliderWorldMatrix._33 = colliderDesc->vExtents.z;

		}
		else
		{
			// 설치
			m_pTransformCom->Set_WorldMatrix(colliderDesc->vStartPos);
			m_WorldMatrix = XMMatrixIdentity();

			XMStoreFloat4x4(&m_ColliderWorldMatrix, m_WorldMatrix);
			m_ColliderWorldMatrix._41 = colliderDesc->vStartPos.r[3].m128_f32[0];
			m_ColliderWorldMatrix._42 = colliderDesc->vStartPos.r[3].m128_f32[1];
			m_ColliderWorldMatrix._43 = colliderDesc->vStartPos.r[3].m128_f32[2];
			m_ColliderWorldMatrix._44 = 1;
		}
		
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_tColliderDesc.vQuaternion = _float3(0,0,0);

	return S_OK;
}

void CColliderObj::Priority_Tick(const _float& fTimeDelta)
{
}

void CColliderObj::Tick(const _float& fTimeDelta)
{
	m_pColliderCom->Tick(m_WorldMatrix);

	_float3 vCenter = _float3(m_ColliderWorldMatrix._41, m_ColliderWorldMatrix._42, m_ColliderWorldMatrix._43);

	m_pColliderCom->Set_Center(vCenter);
	m_tColliderDesc.vCenter = vCenter;
	
	XMMATRIX worldMat = XMLoadFloat4x4(&m_ColliderWorldMatrix);
	XMVECTOR scale, rotationQuat, translation;
	XMMatrixDecompose(&scale, &rotationQuat, &translation, worldMat);

	// Quaternion에서 롤, 피치, 요 각도 추출
	XMFLOAT3 angles;
	XMStoreFloat3(&angles, XMQuaternionRotationRollPitchYawFromVector(rotationQuat));
	float pitch = angles.x;
	float yaw = angles.y;
	float roll = angles.z;

	_float3 vRotation = _float3(pitch, yaw, roll);

	m_pColliderCom->Set_Rotation(vRotation);

	_float3 vScale = _float3(m_ColliderWorldMatrix._11, m_ColliderWorldMatrix._22, m_ColliderWorldMatrix._33);
	m_pColliderCom->Set_Value(&vScale);

	XMVECTOR vPosition = XMVectorSet(m_ColliderWorldMatrix._41, m_ColliderWorldMatrix._42, m_ColliderWorldMatrix._43, 1);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

	m_tColliderDesc.vExtents = vScale;

}

void CColliderObj::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CColliderObj::Render()
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

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif


	return S_OK;
}

void CColliderObj::Set_ColliderDesc(COLLIDER_DESC lightDesc)
{



}

COLLIDER_DESC CColliderObj::Get_ColliderDesc()
{
	XMMATRIX worldMat = XMLoadFloat4x4(&m_ColliderWorldMatrix);
	XMVECTOR scale, rotationQuat, translation;
	XMMatrixDecompose(&scale, &rotationQuat, &translation, worldMat);

	// Quaternion에서 롤, 피치, 요 각도 추출
	XMFLOAT3 angles;
	XMStoreFloat3(&angles, XMQuaternionRotationRollPitchYawFromVector(rotationQuat));
	float pitch = angles.x;
	float yaw = angles.y;
	float roll = angles.z;

	m_tColliderDesc.vQuaternion = _float3(pitch, yaw, roll);
	XMStoreFloat4x4(&m_tColliderDesc.vTransform, m_pTransformCom->Get_WorldMatrix());


	return m_tColliderDesc;
}



HRESULT CColliderObj::Add_Components(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Model_Bone_Sphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	XMVECTOR pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	COLLIDEROBJ_DESC* colliderDesc = (COLLIDEROBJ_DESC*)pArg;

	if (true == colliderDesc->bLoad)
	{
		// 로드
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_OBB;
		ColliderDesc.vExtents = colliderDesc->vExtents;
		ColliderDesc.vCenter = colliderDesc->vCenter;
		ColliderDesc.vRotation = colliderDesc->vQuaternion;

		if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
	}
	else
	{
		// 시작
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::COLLIDER_OBB;
		ColliderDesc.vExtents = _float3(1, 1, 1);
		ColliderDesc.vCenter = _float3(pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);
		ColliderDesc.vRotation = _float3(0, 0.f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
	}
	

    return S_OK;
}

HRESULT CColliderObj::Bind_ShaderResources()
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
	if (CImgui_Manager::IDWRIE::COLLIDER == CImgui_Manager::GetInstance()->Get_Write())
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

CColliderObj* CColliderObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColliderObj* pInstance = new CColliderObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CColliderObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CColliderObj::Clone(void* pArg)
{
	CColliderObj* pInstance = new CColliderObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CColliderObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColliderObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
