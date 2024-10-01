#include "Camera.h"

#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pCameraDesc->vFocus));

	m_fFovY = pCameraDesc->fFovY;
	m_fAspect = pCameraDesc->fAspect;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;

	m_fDefaultFovY = m_fFovY;
	m_fTargetFovY = m_fFovY;
	m_fDefaultNear = m_fNear;
	m_fDefaultFar = m_fFar;

	return S_OK;
}

void CCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCamera::Tick(const _float& fTimeDelta, _bool isAuto)
{
	Shaking(fTimeDelta);
	Zoom(fTimeDelta);

	if (isAuto)
		m_WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));
	m_pGameInstance->Set_CamFar(m_fFar);

	/* 반사벡터를 위한 뷰, 투영 행렬을 구함. */ 
	_vector vPos = XMLoadFloat4((_float4*)&m_WorldMatrix.m[3]);
	_vector	vPosition, vLookAt;

	vPosition = XMVectorSetY(vPos, -7.f);
	vLookAt = XMVectorSetY(vPos, 0.f);

	_matrix viewReflect = XMMatrixLookAtLH(vPosition, vLookAt, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pGameInstance->Set_ReflectViewMatrix(viewReflect);
}

void CCamera::Late_Tick(const _float& fTimeDelta)
{
}

void CCamera::Turn(_float4x4* OrbitMatrix, _fvector vAxis, const _float& fTimeDelta)
{
	_matrix SubMatrix = XMLoadFloat4x4(OrbitMatrix);
	_vector		vRight = SubMatrix.r[0];
	_vector		vUp = SubMatrix.r[1];
	_vector		vLook = SubMatrix.r[2];

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(45.f) * fTimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	SubMatrix.r[0] = vRight;
	SubMatrix.r[1] = vUp;
	SubMatrix.r[2] = vLook;

	XMStoreFloat4x4(OrbitMatrix, SubMatrix);
}

void CCamera::Rotation(_float4x4* OrbitMatrix, _fvector vAxis, _float fRadian)
{
	_matrix SubMatrix = XMLoadFloat4x4(OrbitMatrix);
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	SubMatrix.r[0] = vRight;
	SubMatrix.r[1] = vUp;
	SubMatrix.r[2] = vLook;

	XMStoreFloat4x4(OrbitMatrix, SubMatrix);
}

void CCamera::Zoom(const _float& fTimeDelta)
{
	if (!m_isZooming) return;

	// 커스텀 보간 비율이 0이 아니라면 커스텀된대로 보간해준다
	if (-1.f != m_fCustomLerpRatio)
	{
		m_fFovY = LerpFloat(m_fDefaultFovY, m_fTargetFovY, m_fCustomLerpRatio);
	}
	else
	{
		// 경과 시간 업데이트
		m_fElapsedTime += fTimeDelta;
		m_fLerpRatio = m_fElapsedTime / m_fTotalLerpTime;

		// 보간 비율이 1을 초과하지 않도록 제한
		if (m_fLerpRatio >= 1.0f)
		{
			m_fLerpRatio = 1.0f;
			m_isZooming = false; // 보간 완료
		}

		m_fFovY = LerpFloat(m_fDefaultFovY, m_fTargetFovY, m_fLerpRatio);
	}

	// 여기서 false가 나온다는것은, 완료 이후라는 것으로 초기화해줘야한다.
	if (!m_isZooming)
	{
		Reset_ZoomVariables();
	}
}

void CCamera::LookAt(_float4x4* OrbitMatrix, _fvector vTargetPos, _fvector vPosition)
{
	_vector vLook = vTargetPos - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	if (isnan(XMVector4Normalize(vRight).m128_f32[0]) || isnan(XMVector4Normalize(vUp).m128_f32[0]) || isnan(XMVector4Normalize(vLook).m128_f32[0]))
	{
		return;
	}

	XMStoreFloat4((_float4*)&OrbitMatrix->m[0], XMVector4Normalize(vRight));
	XMStoreFloat4((_float4*)&OrbitMatrix->m[1], XMVector4Normalize(vUp));
	XMStoreFloat4((_float4*)&OrbitMatrix->m[2], XMVector4Normalize(vLook));
}

void CCamera::Shaking(_float fTimeDelta)
{
	if (!m_isShaking)
		return;

	m_fShakeTime += fTimeDelta;

	if (m_fShakeTime <= m_fShakeDuration)
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		_float fOffset = ((rand() % 100) / 100.f - 0.5f) * m_fShakeMagnitude;
		_vector vShakeOffset = XMVectorSet(fOffset, fOffset, 0.f, 0.f);
		//vShakeOffset = XMVector3Normalize(vShakeOffset);

		vPos.x += vShakeOffset.m128_f32[0] * m_vShakeDir.x;
		vPos.y += vShakeOffset.m128_f32[1] * m_vShakeDir.y;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	}
	else
	{
		m_fShakeTime = 0.f;
		m_isShaking = false;
	}
}

void CCamera::Reset_ZoomVariables()
{
	m_isZooming = { false };
	m_fLerpRatio = { 0.f };    // 보간 비율
	m_fElapsedTime = 0.0f; // 경과 시간
	m_fTotalLerpTime = 0.5f; // 보간에 걸리는 총 시간 (초 단위)
	m_fCustomLerpRatio = -1.f;
	m_fTargetFovY = m_fDefaultFovY;
	m_fFovY = m_fDefaultFovY;
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}
