#include "CarChase_CATBullet.h"

#include "GameInstance.h"
#include "UIManager.h"

#include "Mesh.h"

#include "Highway_Kiryu.h"
#include "Highway_Taxi.h"
#include "EffectManager.h"


CCarChase_CATBullet::CCarChase_CATBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCarChase_Monster{ pDevice, pContext },
	m_pUIManager { CUIManager::GetInstance() }
{
	Safe_AddRef(m_pUIManager);
}

CCarChase_CATBullet::CCarChase_CATBullet(const CCarChase_CATBullet& rhs)
	: CCarChase_Monster{ rhs },
	m_pUIManager{ rhs.m_pUIManager}
{
	Safe_AddRef(m_pUIManager);
}

void CCarChase_CATBullet::Set_Coll()
{
	m_isColl = true;

	//충돌처리
	m_Info.fHp -= 10.f;

	if (m_Info.fHp <= 0.f)
	{
		m_Info.fHp = 0.f;
		m_isDead = true;

		m_pUIManager->Remove_Target(m_iObjectIndex);
		m_pGameInstance->PlaySound_W(TEXT("rocket_damage_1.wav"), SOUND_EFFECT, DEFAULT_EFFECT_VOLUME);
	}
}

_vector CCarChase_CATBullet::Get_BulletPos()
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[3]);
}

HRESULT CCarChase_CATBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_CATBullet::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);
	m_pParentMatrix = pDesc->pParentMatrix;

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix));

	m_pTarget = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Taxi"), 0))->Get_Kiryu();
	if (nullptr == m_pTarget)
		return E_FAIL;
	Safe_AddRef(m_pTarget);

	m_pUIManager->Add_Target(m_iObjectIndex, this, true);

	m_Info.fMaxHP = 1.f;

	m_Info.fHp = m_Info.fMaxHP;

	return S_OK;
}

void CCarChase_CATBullet::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_CATBullet::Tick(const _float& fTimeDelta)
{
	if (m_isObjectDead)
	{
		Set_Dead();
		//이펙트 생성
		CEffect::EFFECT_DESC EffectDesc;

		EffectDesc.pWorldMatrix = &m_WorldMatrix;
		CEffectManager::GetInstance()->Heli_BulletExp(EffectDesc);
	}

	_vector vPlayerPos = XMLoadFloat4((_float4*)&m_pTarget->Get_ModelMatrix()->m[3]);
	_vector vThisPos = XMLoadFloat4((_float4*)&m_WorldMatrix.m[3]);
	float fVerticalSpeed = CalculateInitialVerticalSpeed(vPlayerPos, vThisPos);

	//플레이어를 바라봄.
	//m_pTransformCom->LookAt_For_LandObject(XMLoadFloat4((_float4*)&m_pTarget->Get_ModelMatrix()->m[3]));
	_vector vLook = XMVector3Normalize(vPlayerPos - vThisPos);
	vLook = XMVectorSetW(vLook, 0.f);
	m_pTransformCom->LookAt(vLook, true);

	//플레이어를 따라감.
	// 시간에 따른 위치 업데이트
	_float4 vTransformPos;
	XMStoreFloat4(&vTransformPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//수직 위치
	fVerticalSpeed -= m_fGravite * fTimeDelta;
	_float fY = vTransformPos.y + fVerticalSpeed * fTimeDelta;

	XMStoreFloat4(&vTransformPos, XMLoadFloat4(&vTransformPos) + XMVector3Normalize(vLook) * m_fSpeed * fTimeDelta);
	vTransformPos.y = fY;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vTransformPos));

	_matrix ParentMatrix = XMMatrixIdentity();
	ParentMatrix.r[3] = XMVectorSetY(XMLoadFloat4x4(m_pParentMatrix).r[3], XMVectorGetY(XMLoadFloat4x4(m_pParentMatrix).r[3]) + 2.f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * ParentMatrix);

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

		m_pEffect->Tick(fTimeDelta);
}

void CCarChase_CATBullet::Late_Tick(const _float& fTimeDelta)
{
	if (Check_Coll())
		return;

	Update_TargetingUI();

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	m_pEffect->Late_Tick(fTimeDelta);
	m_isColl = false;
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCarChase_CATBullet::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		if (nullptr != m_pMaterialCom)
		{
			if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
				return E_FAIL;
		}

		_bool fFar = m_pGameInstance->Get_CamFar();
		m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;
		_bool isRM = true;
		_bool isRT = true;
		_bool isMulti = true;
		_float fRDCount = 1.f;

		if (m_pGameInstance->Find_String(pMesh->Get_Name(), "shirts") ||
			m_pGameInstance->Find_String(pMesh->Get_Name(), "jacket") ||
			m_pGameInstance->Find_String(pMesh->Get_Name(), "pants"))
			fRDCount = 50.f;
		m_pShaderCom->Bind_RawValue("g_RDCount", &fRDCount, sizeof(_float));


		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS)))
			isMulti = false;
		m_pShaderCom->Bind_RawValue("g_isMulti", &isMulti, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("IsY3Shader", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;
		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
			isRM = false;
		m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RTTexture", i, aiTextureType_EMISSIVE)))
			isRT = false;
		m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

		m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

HRESULT CCarChase_CATBullet::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_RcktGunBullet"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_RcktGunBullet"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.3, 0.3, 0.3);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	CEffect::EFFECT_DESC EffectDesc;
	EffectDesc.pWorldMatrix = &m_WorldMatrix;

	//연기
	m_pEffect = reinterpret_cast<CEffect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Particle_Point_HeliBullet_Smog"), &EffectDesc));

	return S_OK;
}

HRESULT CCarChase_CATBullet::Bind_ResourceData()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CCarChase_CATBullet::Update_TargetingUI()
{
	//뼈에 붙이기 (kubi_c_n)
	_matrix BoneMatrix = XMMatrixIdentity();

	//3D -> 2D 변환
	_matrix VPMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) * m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	_matrix vWorldMaitrx = XMLoadFloat4x4(&m_WorldMatrix);

	//틱마다 업데이트
	XMStoreFloat4x4(&m_pWorldMatrix, vWorldMaitrx);

	_matrix ResultMatrix = vWorldMaitrx * VPMatrix;
	_vector vPos = ResultMatrix.r[3];
	_float fX = XMVectorGetX(vPos) / XMVectorGetW(vPos);
	_float fY = XMVectorGetY(vPos) / XMVectorGetW(vPos);

	_float windowX = fX * (g_iWinSizeX / 2);
	_float windowY = fY * (g_iWinSizeY / 2);

	BoneMatrix.r[3] = XMVectorSet(windowX, windowY, 0.f, 1.f);
	m_pUIManager->Update_TargetMatrix(m_iObjectIndex, BoneMatrix, m_Info.fHp);
}

_float CCarChase_CATBullet::CalculateInitialVerticalSpeed(_vector vPlayerPos, _vector vPosition)
{
	// 플레이어까지의 거리 계산
	float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vPosition));


	// 도달하는 데 필요한 시간 계산
	float timeToTarget = fDistance / m_fSpeed;

	// 초기 수직 속도 계산 (kinematic equation: v = u + at -> u = (v - at))
	float verticalSpeed = (XMVectorGetY(vPlayerPos) - XMVectorGetY(vPosition) + 0.5f * m_fGravite * timeToTarget * timeToTarget) / timeToTarget;

	return verticalSpeed;
}

_bool CCarChase_CATBullet::Check_Coll()
{

	if (m_pColliderCom->Intersect(m_pTarget->Get_KiryuCollier(), 1.f))
	{
		//충돌이 일어남
		Set_Dead();
		m_pGameInstance->PlaySound_W(TEXT("rocket_damage_0.wav"), SOUND_EFFECT, DEFAULT_EFFECT_VOLUME);

		m_pUIManager->Remove_Target(m_iObjectIndex);

		m_pTarget = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Taxi"), 0))->Get_Kiryu();
		m_pTarget->OnHit(10.f);

		//이펙트 생성
		CEffect::EFFECT_DESC EffectDesc;

		EffectDesc.pWorldMatrix = &m_WorldMatrix;
		CEffectManager::GetInstance()->Heli_BulletExp(EffectDesc);
		return true;
	}
	return false;
}

CCarChase_CATBullet* CCarChase_CATBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChase_CATBullet* pInstance = new CCarChase_CATBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChase_CATBullet::Clone(void* pArg)
{
	CCarChase_CATBullet* pInstance = new CCarChase_CATBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase_CATBullet::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pEffect);

	Safe_Release(m_pTarget);

	Safe_Release(m_pUIManager);
}
