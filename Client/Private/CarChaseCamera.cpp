#include "CarChaseCamera.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "UIManager.h"

#include "CarChase_CATBullet.h"
#include "CarChase_Reactor.h"

CCarChaseCamera::CCarChaseCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pUIManager {CUIManager::GetInstance()}
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pUIManager);
}

CCarChaseCamera::CCarChaseCamera(const CCarChaseCamera& rhs)
	: CCamera{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager },
	m_pUIManager { rhs.m_pUIManager }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CCarChaseCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChaseCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CARCHASE_CAMERA_DESC* pDesc = static_cast<CARCHASE_CAMERA_DESC*>(pArg);
	m_fSensor = pDesc->fSensor;
	m_pPlayerMatrix = pDesc->pPlayerMatrix;
	m_pPlayerBoneMatrix = pDesc->pPlayerBoneMatrix;

	return S_OK;
}

void CCarChaseCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChaseCamera::Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_CARCHASE) return;


	//Adjust_Camera_Angle();
	if (m_pUIManager->isTitleEnd())
		Targeting(fTimeDelta);

	_vector vPlayerPos = XMLoadFloat4x4(m_pPlayerMatrix).r[3];

	_float4 vTargetPosition;

	// 목표 위치 계산
	if (!m_isShoulderView)
	{
		vTargetPosition.x = XMVectorGetX(vPlayerPos) + cosf(XMConvertToRadians(m_fCamAngleX)) * cosf(XMConvertToRadians(m_fCamAngleY)) * m_fCamDistance;
		vTargetPosition.y = XMVectorGetY(vPlayerPos) + m_fCamDistance * sinf(XMConvertToRadians(m_fCamAngleY)) + m_fHeight;
		vTargetPosition.z = XMVectorGetZ(vPlayerPos) + sinf(XMConvertToRadians(m_fCamAngleX)) * cosf(XMConvertToRadians(m_fCamAngleY));
		vTargetPosition.w = 1.f;
	}
	else
	{
		vTargetPosition.x = XMVectorGetX(vPlayerPos) + cosf(XMConvertToRadians(m_fCamAngleX)) * cosf(XMConvertToRadians(m_fCamAngleY)) * m_fCamDistance_X;
		vTargetPosition.y = XMVectorGetY(vPlayerPos) + m_fCamDistance_Y * sinf(XMConvertToRadians(m_fCamAngleY)) + m_fHeight;
		vTargetPosition.z = XMVectorGetZ(vPlayerPos) + sinf(XMConvertToRadians(m_fCamAngleX)) * cosf(XMConvertToRadians(m_fCamAngleY)) * m_fCamDistance_Z;
		vTargetPosition.w = 1.f;
	}

	// 현재 카메라 위치
	_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 목표 위치로 선형 보간
	float lerpFactor = 0.6f; // 보간 속도
	_vector vNewCameraPosition = XMVectorLerp(vCurrentPosition, XMLoadFloat4(&vTargetPosition), lerpFactor);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewCameraPosition);

	
	__super::Tick(fTimeDelta);
}

void CCarChaseCamera::Late_Tick(const _float& fTimeDelta)
{
	
}

HRESULT CCarChaseCamera::Render()
{
	return S_OK;
}

void CCarChaseCamera::Targeting(const _float& fTimeDelta)
{
	vector<CGameObject*> Bullets = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Bullet"));
	vector<CGameObject*> Reactors = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_Reactor"));
	if (Reactors.size() >= 2)
		m_isTargetPlayer = false;

	_vector vTargetingPos;
	_vector vCamMoveDir = {};
	if (Reactors.empty() || m_isTargetPlayer)
	{
		//플레이어 타겟팅 일 경우
		m_fHeight = 4.f;
		XMStoreFloat4x4(&m_pPlayerWorldMatrix, XMLoadFloat4x4(m_pPlayerBoneMatrix) * XMLoadFloat4x4(m_pPlayerMatrix));

		vTargetingPos = XMLoadFloat4((_float4*)&m_pPlayerMatrix->m[3]);
		vCamMoveDir = XMLoadFloat4((_float4*)&m_pPlayerWorldMatrix.m[0]) * -1.f;

	}
	else if (!Bullets.empty())
	{
		//불렛 타겟팅 일 경우
		CGameObject* pTarget = Bullets.back();

		//불렛은 하나만 있다고 가정한다.
		if (pTarget->isDead())
		{
			// 포커싱 초기화
			m_isZooming = false;
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
			Reset_ZoomVariables();

			m_isTargetPlayer = true;
			return;
		}

		vTargetingPos = dynamic_cast<CCarChase_CATBullet*>(pTarget)->Get_BulletPos();
		vCamMoveDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vTargetingPos);

		Focusing_Bullet(pTarget);
	}
	else {
		//몬스터 타겟팅 일 경우
		CGameObject* pTarget = Reactors.back();

		if (pTarget->isObjectDead())
		{
			if (Reactors.size() <= 1)
			{
				if (pTarget->isDead())
					m_isTargetPlayer = true;
				return;
			}

			pTarget = Reactors[Reactors.size() - 2];
		}

		m_fHeight = 2.f;
		vTargetingPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		vCamMoveDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vTargetingPos);
	}

	//선형보간
	_vector vCamDirection = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vCamTargetDirection = XMVector3Normalize(vTargetingPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_vector vLerpDir = XMVectorLerp(vCamDirection, vCamTargetDirection, fTimeDelta * 2.f);

	//카메라 이동값 용
	m_fCamAngleX = XMConvertToDegrees(atan2f(XMVectorGetZ(vCamMoveDir), XMVectorGetX(vCamMoveDir)));
	m_fCamAngleY = XMConvertToDegrees(asinf(XMVectorGetY(vCamMoveDir) / m_fCamDistance));

	m_pTransformCom->LookAt(vLerpDir, true);
}

void CCarChaseCamera::Focusing_Bullet(CGameObject* pTarget)
{
	_vector vTargetingPos = dynamic_cast<CCarChase_CATBullet*>(pTarget)->Get_BulletPos();
	_vector vCamMoveDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vTargetingPos);

	CCarChase_Reactor* pHeli = dynamic_cast<CCarChase_Reactor*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Reactor"), 0));
	_matrix pMonsterMat = XMLoadFloat4x4(dynamic_cast<CCarChase_Monster*>(pHeli->Get_Monsters().front())->Get_ModelMatrix());

	_vector vMonsterPos = pMonsterMat.r[CTransform::STATE_POSITION];
	_vector vPlayerPos = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0)->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	// 플레이어와 몬스터간의 거리, Duration이 된다.
	_float vMonsterDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos));

	// 플레이어와 불렛간의 거리, 비율이 된다.
	_float vDistnace = XMVector3Length(vPlayerPos - vTargetingPos).m128_f32[0];

	_float fRatio = vDistnace / vMonsterDistance;

	m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), fRatio);

	if (vDistnace <= 3.f)
		fRatio = 0.f;

	if (fRatio < 0.1f)
	{
		fRatio = 0.f;
		m_isZooming = false;
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 1.f);
		Reset_ZoomVariables();
	}

	Set_CustomRatio(fRatio);
	Set_TargetFoV(XMConvertToRadians(20.f));
	Start_Zoom();
}

CCarChaseCamera* CCarChaseCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChaseCamera* pInstance = new CCarChaseCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChaseCamera::Clone(void* pArg)
{
	CCarChaseCamera* pInstance = new CCarChaseCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChaseCamera::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
	Safe_Release(m_pUIManager);
}