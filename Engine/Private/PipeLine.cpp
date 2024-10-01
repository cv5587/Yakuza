#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

const _float4x4* CPipeLine::Get_Transform_Float4x4(D3DTRANSFORMSTATE eState)
{
	return &m_TransformStateMatrix[eState];
}

const _float4x4* CPipeLine::Get_Old_Transform_Float4x4(D3DTRANSFORMSTATE eState)
{
	return &m_TransformStateMatrix[eState];
}

_matrix CPipeLine::Get_Transform_Matrix(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_TransformStateMatrix[eState]);
}

const _float4x4* CPipeLine::Get_Transform_Inverse_Float4x4(D3DTRANSFORMSTATE eState)
{
	return &m_TransformStateInverseMatrix[eState];
}

_matrix CPipeLine::Get_Transform_Inverse_Matrix(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_TransformStateInverseMatrix[eState]);
}

const _float4x4* CPipeLine::Get_Shadow_Transform_View_Float4x4()
{
	return &m_ShadowTransformViewMatrix[0];
}

const _float4x4* CPipeLine::Get_Shadow_Transform_Proj_Float4x4()
{
	return &m_ShadowTransformProjMatrix[0];
}

const _float4* CPipeLine::Get_CamPosition_Float4()
{
	return &m_vCamPosition;
}

_vector CPipeLine::Get_CamPosition()
{
	return XMLoadFloat4(&m_vCamPosition);
}

const _float4* CPipeLine::Get_ComLook_Float4()
{
	return &m_vCamLook;
}

_vector CPipeLine::Get_CamLook()
{
	return XMLoadFloat4(&m_vCamLook);
}

_vector CPipeLine::Get_CamRight()
{
	return XMLoadFloat4(&m_vCamRight);
}

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < D3DTS_END; ++i)
	{
		XMStoreFloat4x4(&m_TransformStateMatrix[i], XMMatrixIdentity());
		m_TransformStateInverseMatrix[i] = m_TransformStateMatrix[i];
	}

	m_vCamPosition = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < D3DTS_END; ++i)
	{
		XMStoreFloat4x4(&m_TransformStateInverseMatrix[i], XMMatrixInverse(nullptr, Get_Transform_Matrix(D3DTRANSFORMSTATE(i))));
	}

	memcpy(&m_vCamRight, &m_TransformStateInverseMatrix[D3DTS_VIEW].m[0], sizeof(_float4));
	memcpy(&m_vCamLook, &m_TransformStateInverseMatrix[D3DTS_VIEW].m[2], sizeof(_float4));
	memcpy(&m_vCamPosition, &m_TransformStateInverseMatrix[D3DTS_VIEW].m[3], sizeof(_float4));

	Ready_ShadowFurstum();
}

void CPipeLine::Ready_ShadowFurstum()
{
	vector<_float> m_Casecade = { 0.f, 20.f, 50.f, 80.f };
	//케스케이드 그림자맵을 위한 절두체
	_float3 vFrustum[]{
		{-1.f, 1.f, 0.f},
		{1.f, 1.f, 0.f},
		{1.f, -1.f, 0.f},
		{-1.f, -1.f, 0.f},

		{-1.f, 1.f, 1.f},
		{1.f, 1.f, 1.f},
		{1.f, -1.f, 1.f},
		{-1.f, -1.f, 1.f}
	};

	// NDC좌표계 -> 월드 좌표계 변환 행렬
	_matrix ViewMatrixInverse = Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrixInverse = Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);

	_float4	vPoints[8] = {};
	_float4x4 ViewMatrixArray[3] = {};
	_float4x4 ProjMatrixArray[3] = {};

	for (size_t i = 0; i < 8; i++)
		XMStoreFloat4(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&vFrustum[i]), ProjMatrixInverse));

	for (size_t i = 0; i < 8; i++)
		XMStoreFloat3(&vFrustum[i], XMVector3Transform(XMLoadFloat4(&vPoints[i]), ViewMatrixInverse));
	
	for (size_t i = 0; i < m_Casecade.size() - 1; ++i)
	{
		//큐브의 정점을 시야절두체 구간으로 변경
		_float3 Frustum[8];
		for (size_t j = 0; j < 8; ++j)
			Frustum[j] = vFrustum[j];

		for (size_t j = 0; j < 4; ++j)
		{
			//앞에서 뒤쪽으로 향하는 벡터
			_vector vTemp = XMVector3Normalize(XMLoadFloat3(&Frustum[j + 4]) - XMLoadFloat3(&Frustum[j]));

			//구간 시작, 끝으로 만들어주는 벡터
			_vector n = vTemp * m_Casecade[i];
			_vector f = vTemp * m_Casecade[i + 1];

			//구간 시작, 끝으로 설정
			XMStoreFloat3(&Frustum[j + 4], XMLoadFloat3(&Frustum[j]) + f);
			XMStoreFloat3(&Frustum[j], XMLoadFloat3(&Frustum[j]) + n);
		}

		//해당 구간을 포함할 바운딩구의 중심을 계산
		_vector vCenter{};
		for (auto& v : Frustum)
		{
			//_matrix matCamInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
			//XMStoreFloat3(&v, XMVector3TransformCoord(XMLoadFloat3(&v), matCamInv));
			vCenter = vCenter + XMLoadFloat3(&v);
		}

		vCenter = vCenter / 8.f;
		vCenter = XMVectorSetW(vCenter, 1.f);

		//바운딩구의 반지름을 계산
		_float fRadius = 0;
		for (auto& v : Frustum)
			fRadius = max(fRadius, XMVectorGetX(XMVector3Length(XMLoadFloat3(&v) - vCenter)));

		fRadius = ceil(fRadius * 16.f) / 16.f;

		/* 광원 기준의 뷰 변환행렬. */
		_float4x4		ViewMatrix, ProjMatrix;

		_vector vLightDir = XMVector3Normalize(XMLoadFloat4(&m_vLightDir));
		//_vector vLightDir = XMVector3Normalize(XMVectorSet(0.f, 1.f, -1.f, 0.f));
		_vector  shadowLightPos = vCenter + (vLightDir * -fRadius);
		shadowLightPos = XMVectorSetW(shadowLightPos, 1.f);

		XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(shadowLightPos, vCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(fRadius * 2, fRadius * 2, 0.1f, 1000.f));



		_vector vShadowOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		vShadowOrigin = XMVector3TransformCoord(vShadowOrigin, XMLoadFloat4x4(&ViewMatrix) * XMLoadFloat4x4(&ProjMatrix));
		vShadowOrigin = vShadowOrigin * (1280.f / 2.f);

		_vector vRoundedOrigin = XMVectorRound(vShadowOrigin);
		_vector vRoundOffset = vRoundedOrigin - vShadowOrigin;

		vRoundOffset = vRoundOffset * 2.f / 1280.f;
		vRoundOffset = XMVectorSetZ(vRoundOffset, 0.f);
		vRoundOffset = XMVectorSetW(vRoundOffset, 0.f);

		XMStoreFloat4((_float4*)ProjMatrix.m[3], XMLoadFloat4((_float4*)&ProjMatrix.m[3]) + vRoundOffset);

		ViewMatrixArray[i] = ViewMatrix;
		ProjMatrixArray[i] = ProjMatrix;
	}

	Set_ShadowTransformProjMatrix(ProjMatrixArray);
	Set_ShadowTransformViewMatrix(ViewMatrixArray);
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CPipeLine");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CPipeLine::Free()
{
}