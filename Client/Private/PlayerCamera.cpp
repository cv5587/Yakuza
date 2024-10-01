#include "PlayerCamera.h"

#include "GameInstance.h"
#include "SystemManager.h"
#include "Collision_Manager.h"
#include "UIManager.h"

#include "Player.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext },
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pCollisionManager{ CCollision_Manager::GetInstance() },
	m_pUIManager{ CUIManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
	Safe_AddRef(m_pUIManager);
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& rhs)
	: CCamera{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager },
	m_pCollisionManager{ rhs.m_pCollisionManager },
	m_pUIManager{ rhs.m_pUIManager }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CPlayerCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
	// 고속도로맵에서는 플레이어 없어서 카메라 막기
	if (m_pSystemManager->Get_Camera() != CAMERA_CARCHASE)
	{
		PLAYER_CAMERA_DESC* pDesc = static_cast<PLAYER_CAMERA_DESC*>(pArg);

		if (nullptr != pDesc)
		{
			m_fSensor = pDesc->fSensor;
			m_pPlayerMatrix = pDesc->pPlayerMatrix;
			m_iCurLevel = pDesc->iCurLevel;
		}

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		if (FAILED(Add_Components()))
			return E_FAIL;

		// 시작점 조정
		Set_StartPos();

		_vector vPlayerPosition;
		memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

		m_fCamDistance = MAX_DISTANCE;

		_vector vCamPosition = XMVectorSet(
			m_fCamDistance * cosf(XMConvertToRadians(m_fCamAngleY)) * cosf(XMConvertToRadians(m_fCamAngleX)),
			m_fCamDistance * sinf(XMConvertToRadians(m_fCamAngleX)),
			m_fCamDistance * sinf(XMConvertToRadians(m_fCamAngleY)) * cosf(XMConvertToRadians(m_fCamAngleX)),
			1.f
		);

		vCamPosition += XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition), XMVectorGetZ(vPlayerPosition), 0);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);

	}

	
	return S_OK;
}

void CPlayerCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CPlayerCamera::Tick(const _float& fTimeDelta)
{
	if (LEVEL::LEVEL_CARCHASE != m_iCurrentLevel)
	{
		if (m_pSystemManager->Get_Camera() != CAMERA_PLAYER) return;

		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

		if(m_isReturn)
			Play_FovLerp(fTimeDelta);

		__super::Tick(fTimeDelta);
	}

	
}

void CPlayerCamera::Late_Tick(const _float& fTimeDelta)
{
	if (LEVEL::LEVEL_CARCHASE != m_iCurrentLevel)
	{
		if (m_pSystemManager->Get_Camera() != CAMERA_PLAYER) return;
		
		m_ePreCamState = m_eCurCamState;
		
		//Advencture 일떄와 아닐떄 구별해서 length
		if (CPlayer::ADVENTURE == dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0))->Get_BattleStyle())
		{
			m_eCurCamState = CAM_ADV;
		}
		else
		{
			m_eCurCamState = CAM_BATTLE;
		}

		// 카메라 state(adv, fight)에 따라서 거리조절
		if (m_ePreCamState != m_eCurCamState)
		{
			m_bFightLerp = true;
			m_fLerpDelta = 0;
			
			if (CAM_ADV == m_eCurCamState)
			{
				m_fMaxDistance = MAX_DISTANCE_ADV;
				m_fMinDistance = MIN_DISTANCE_ADV;
			}
			else
			{
				m_fMaxDistance = MAX_DISTANCE_BATTLE;
				m_fMinDistance = MIN_DISTANCE_BATTLE;
			}
		}

		// 모드 변경해줄때 lerp
		if (true == m_bFightLerp)
			Camera_FightMode_Change(fTimeDelta);
		else
			Compute_View_During_Collision(fTimeDelta);

	}

	
}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
}


void CPlayerCamera::Compute_View_During_Collision(const _float& fTimeDelta)
{
	// 플레이어 위치 가져오기
	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

	// 너무 멀리 떨어져있을경우 충돌됐어도 선형보간해서 따라가야함
	float		fDistance = XMVectorGetX(XMVector3Length(vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (MIN_LERP_DISTANCE < fDistance)
	{
		m_bBlock = false;
	}

	if (true == m_bBlock)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vLatestCollisionPos);

		_vector vLookAt = XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition) + 1.f, XMVectorGetZ(vPlayerPosition), 1);
		m_pTransformCom->LookAt(vLookAt);

		m_bLerp = false;
		m_bCamCollision = true;

		return;
	}
	else
	{
		// 마우스 입력을 이용한 카메라 회전
		_long MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMS_X);
		_long MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);

		float	fCamAngleY = m_fCamAngleY;
		float	fCamAngleX = m_fCamAngleX;

		// 회전 막기(외부에서) - 대화창용도
		if (false == m_bBlockRotation)
		{
			fCamAngleY -= fTimeDelta * m_fSensor * MouseMoveX;
			fCamAngleX += fTimeDelta * m_fSensor * MouseMoveY;

			// 카메라 각도 제한 (수직 각도 제한)
			if (fCamAngleX > 70.0f)  // 캐릭터를 아래서 보지 않도록 최대 각도를 45도로 제한
				fCamAngleX = 70.0f;
			if (fCamAngleX < 30) // 카메라가 수직 아래로 향하지 않도록 최소 각도를 -89도로 제한
				fCamAngleX = 30;
		}

		// 이전 카메라 포지션 저장
		_vector vPrevCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		// 카메라 포지션 계산
		_vector vCamPosition = XMVectorSet(
			m_fMaxDistance * cosf(XMConvertToRadians(fCamAngleY)) * cosf(XMConvertToRadians(fCamAngleX)),
			m_fMaxDistance * sinf(XMConvertToRadians(fCamAngleX)),
			m_fMaxDistance * sinf(XMConvertToRadians(fCamAngleY)) * cosf(XMConvertToRadians(fCamAngleX)),
			1.f
		);

		vCamPosition += XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition), XMVectorGetZ(vPlayerPosition), 0);


		XMVECTOR	vCollisionPos;
		// 충돌 됐을때 막기
		if (true == m_pCollisionManager->Check_Map_Collision_Using_Position(m_pColliderCom, vCamPosition, vCollisionPos))
		{
			if (false == m_bBlock && false == m_bCamCollision)
			{
				// 계속 충돌 위치 바뀌지 않게 지정해주기
				m_vLatestCollisionPos = vCamPosition;
			}
			m_bCamCollision = true;
			m_bBlock = true;
			return;
		}
		else
		{
			m_bBlock = false;
		}

		if (true == m_bCamCollision)
		{
			// 충돌 벗어남
			m_bCamCollision = false;
			m_bLerp = true;
			m_fLerpDelta = 0;
		}


		_vector vLerpedCamPosition = vCamPosition;

		if (true == m_bLerp)
		{
			m_fLerpDelta += fTimeDelta * 0.5;
			vLerpedCamPosition = XMVectorLerp(vPrevCamPosition, vCamPosition, m_fLerpDelta);

			float		fDistance = XMVectorGetX(XMVector3Length(vCamPosition - vLerpedCamPosition));


			if (fDistance < 0.1 || 1 < m_fLerpDelta)
			{
				m_bLerp = false;
			}

		}

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpedCamPosition);



		m_fCamAngleY = fCamAngleY;
		m_fCamAngleX = fCamAngleX;

		// 월드 매트릭스 업데이트
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());


		//LookAt 적용
		_vector vLookAt = XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition) + 1.f, XMVectorGetZ(vPlayerPosition), 1);

		// 카메라가 플레이어를 바라보도록 설정
		m_pTransformCom->LookAt(vLookAt);
	}

	

}

void CPlayerCamera::Camera_FightMode_Change(const _float& fTimeDelta)
{
	m_fLerpDelta += fTimeDelta * 0.5;

	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));


	// 이전 카메라 포지션 저장
	_vector vPrevCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 카메라 포지션 계산
	_vector vCamPosition = XMVectorSet(
		m_fMaxDistance * cosf(XMConvertToRadians(m_fCamAngleY)) * cosf(XMConvertToRadians(m_fCamAngleX)),
		m_fMaxDistance * sinf(XMConvertToRadians(m_fCamAngleX)),
		m_fMaxDistance * sinf(XMConvertToRadians(m_fCamAngleY)) * cosf(XMConvertToRadians(m_fCamAngleX)),
		1.f
	);

	vCamPosition += XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition), XMVectorGetZ(vPlayerPosition), 0);


	_vector vLerpedCamPosition = XMVectorLerp(vPrevCamPosition, vCamPosition, m_fLerpDelta);

	float		fDistance = XMVectorGetX(XMVector3Length(vCamPosition - vLerpedCamPosition));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpedCamPosition);


	if (fDistance < 0.1 || 1 < m_fLerpDelta)
	{
		m_bFightLerp = false;


		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);
	}

	_vector vLookAt = XMVectorSet(XMVectorGetX(vPlayerPosition), XMVectorGetY(vPlayerPosition) + 1.f, XMVectorGetZ(vPlayerPosition), 1);

	// 카메라가 플레이어를 바라보도록 설정
	m_pTransformCom->LookAt(vLookAt);

}

void CPlayerCamera::Set_StartPos()
{
	if (LEVEL::LEVEL_OFFICE_1F == m_iCurrentLevel)
	{
		m_fCamAngleY = -90;
		MIN_LERP_DISTANCE = 1.5f;

		MAX_DISTANCE_ADV = 2.5f;
		MIN_DISTANCE_ADV = 1.5f;

		MAX_DISTANCE_BATTLE = 3.f;
		MIN_DISTANCE_BATTLE = 2.f;
	}
	else if (LEVEL::LEVEL_OFFICE_2F == m_iCurrentLevel)
	{
		m_fCamAngleX = 20;
		m_fCamAngleY = -200;
		MIN_LERP_DISTANCE = 2.5f;
	}
	else if (LEVEL::LEVEL_OFFICE_BOSS == m_iCurrentLevel)
	{
		m_fCamAngleX = 45.f;
		m_fCamAngleY = 90;
		MIN_LERP_DISTANCE = 2.5f;
	}
	else if (LEVEL::LEVEL_DOGIMAZO == m_iCurrentLevel)
	{
		m_fCamAngleY = 0;
		m_fCamAngleX = 45.f;
		MIN_LERP_DISTANCE = 1.5f;

		MAX_DISTANCE_ADV = 2.5f;
		MIN_DISTANCE_ADV = 1.5f;

		MAX_DISTANCE_BATTLE = 3.f;
		MIN_DISTANCE_BATTLE = 2.f;
	}
	else if (LEVEL::LEVEL_DOGIMAZO_STAIRS == m_iCurrentLevel)
	{
		m_fCamAngleY = 0;
		m_fCamAngleX = 45.f;
		MIN_LERP_DISTANCE = 1.5f;
	}
	else if (LEVEL::LEVEL_DOGIMAZO_LOBBY == m_iCurrentLevel)
	{
		m_fCamAngleY = 0;
		m_fCamAngleX = 25.f;
		MIN_LERP_DISTANCE = 1.5f;
	}
	else if (LEVEL::LEVEL_DOGIMAZO_BOSS == m_iCurrentLevel)
	{
		m_fCamAngleX = 30.f;
		MIN_LERP_DISTANCE = 1.5f;
	}

	if (m_iCurrentLevel != LEVEL_LOADING && m_iCurrentLevel != LEVEL_LOGO)
	{
		SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f); // 마우스 좌표 적용해주기
		ShowCursor(false);
	}
}

void CPlayerCamera::Adjust_Camera_Angle()
{
	_vector vPlayerPosition;
	memcpy(&vPlayerPosition, m_pPlayerMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

	_vector vCamDirection = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPlayerPosition);

	// 새로운 각도 계산
	m_fCamAngleY = XMConvertToDegrees(atan2f(XMVectorGetZ(vCamDirection), XMVectorGetX(vCamDirection)));
	m_fCamAngleX = XMConvertToDegrees(asinf(XMVectorGetY(vCamDirection) / m_fCamDistance));

	// 각도 제한 적용
	if (m_fCamAngleX > 70.0f)
		m_fCamAngleX = 70.0f;
	if (m_fCamAngleX < 30.0f)
		m_fCamAngleX = 30.0f;
}

void CPlayerCamera::Return_PrevWorld(const _float& fTimeDelta)
{
	if (!m_isReturn) return;

	// 경과 시간 업데이트
	m_fElapsedTime += fTimeDelta;
	m_fLerpRatio = m_fElapsedTime / m_fTotalLerpTime;

	// 보간 비율이 1을 초과하지 않도록 제한
	if (m_fLerpRatio >= 1.0f)
	{
		m_fLerpRatio = 1.0f;
		m_isReturn = false; // 보간 완료
	}

	// 이전 행렬과 시작 행렬을 분해해서 저장
	_matrix PrevMat = XMLoadFloat4x4(&m_PrevMatrix);
	_vector vPrevScale, vPrevRot, vPrevPos;
	XMMatrixDecompose(&vPrevScale, &vPrevRot, &vPrevPos, PrevMat);

	_matrix StartMat = XMLoadFloat4x4(&m_StartMatrix);
	_vector vStartScale, vStartRot, vStartPos;
	XMMatrixDecompose(&vStartScale, &vStartRot, &vStartPos, StartMat);

	// 각 성분에 대해 선형 보간
	_vector vScaleLerp = XMVectorLerp(vPrevScale, vStartScale, m_fLerpRatio);
	_vector vTransLerp = XMVectorLerp(vPrevPos, vStartPos, m_fLerpRatio);
	_vector vRotLerp = XMQuaternionSlerp(vPrevRot, vStartRot, m_fLerpRatio); // 회전은 구면 선형 보간(Slerp)을 사용

	// 보간된 값을 결합해 행렬 생성
	_matrix M = XMMatrixScalingFromVector(vScaleLerp) *
		XMMatrixRotationQuaternion(vRotLerp) *
		XMMatrixTranslationFromVector(vTransLerp);

	XMStoreFloat4x4(&m_WorldMatrix, M);
	m_fFovY = LerpFloat(m_fStartFov, m_fDefaultFovY, m_fLerpRatio);

	// 여기서 false가 나온다는것은, 완료 이후라는 것으로 초기화해줘야한다.
	if (!m_isReturn)
	{
		Reset_ReturnVariables();
	}
}

void CPlayerCamera::Reset_ReturnVariables()
{
	m_isReturn = { false };
	m_fLerpRatio = { 0.f };    // 보간 비율
	m_fElapsedTime = 0.0f; // 경과 시간
	m_fTotalLerpTime = 0.5f; // 보간에 걸리는 총 시간 (초 단위)
	m_fStartFov = 0.0f; // 보간에 걸리는 총 시간 (초 단위)
}

void CPlayerCamera::Play_FovLerp(const _float& fTimeDelta)
{
	// 경과 시간 업데이트
	m_fElapsedTime += fTimeDelta;
	m_fLerpRatio = m_fElapsedTime / m_fTotalLerpTime;

	// 보간 비율이 1을 초과하지 않도록 제한
	if (m_fLerpRatio >= 1.0f)
	{
		m_fLerpRatio = 1.0f;
		m_isReturn = false; // 보간 완료
	}

	m_fFovY = LerpFloat(m_fStartFov, m_fDefaultFovY, m_fLerpRatio);
	// 여기서 false가 나온다는것은, 완료 이후라는 것으로 초기화해줘야한다.
	if (!m_isReturn)
	{
		Reset_ReturnVariables();
	}
}

HRESULT CPlayerCamera::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_SPHERE;
	ColliderDesc.fRadius = 0.001f;
	ColliderDesc.vCenter = _float3(0, 0, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerCamera* pInstance = new CPlayerCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerCamera::Clone(void* pArg)
{
	CPlayerCamera* pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSystemManager);
	Safe_Release(m_pCollisionManager);
	Safe_Release(m_pUIManager);
}
