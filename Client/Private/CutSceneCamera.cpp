#include "CutSceneCamera.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"
#include "SystemManager.h"

#include "Player.h"

CCutSceneCamera::CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext },
	m_pSystemManager { CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CCutSceneCamera::CCutSceneCamera(const CCutSceneCamera& rhs)
	: CCamera{ rhs },
	m_pSystemManager { rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CCutSceneCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCutSceneCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCutSceneCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCutSceneCamera::Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_CUTSCENE) return;

	ShowCursor(false);

	// 플레이어와 같은 타이머를 써야한다
	__super::Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
}

void CCutSceneCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CCutSceneCamera::Render()
{
	return S_OK;
}

void CCutSceneCamera::Return_PrevWorld(const _float& fTimeDelta)
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

void CCutSceneCamera::Reset_ReturnVariables()
{
	m_isReturn = { false };
	m_fLerpRatio = { 0.f };
	m_fElapsedTime = 0.0f; 
	m_fTotalLerpTime = 0.5f; 
	m_fStartFov = 0.0f; 
	m_fFovY = m_fDefaultFovY;
}

void CCutSceneCamera::Play_FovLerp(const _float& fTimeDelta)
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

CCutSceneCamera* CCutSceneCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutSceneCamera* pInstance = new CCutSceneCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCutSceneCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCutSceneCamera::Clone(void* pArg)
{
	CCutSceneCamera* pInstance = new CCutSceneCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCutSceneCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCutSceneCamera::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
}
