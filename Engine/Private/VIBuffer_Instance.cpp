#include "VIBuffer_Instance.h"
#include "ComputeShader.h"
#include "GameInstance.h"
CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
	: CVIBuffer{ rhs }
{
	//m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance);
	//m_pContext->CopyResource(m_pVBInstance, rhs.m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);

		m_InstanceDesc = pDesc;
		
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{

	ID3D11Buffer* pVertices[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint pStartVertices[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_InstanceDesc->iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Compute_Render()
{
	CComputeShader::COMPUTE_DESC ComputeDesc{};	
	ComputeDesc.OutputBuffer = mOutputUAV;
	ComputeDesc.NumElement = m_InstanceDesc->iNumInstance;

	//m_pComputeShader->Render(&ComputeDesc);

	m_pContext->Flush();




	COMPUTEMATRIX* pCopyBlends = new COMPUTEMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pCopyBlends, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	D3D11_BUFFER_DESC	InstanceBufferDesc = {};

	InstanceBufferDesc.ByteWidth = sizeof(COMPUTEMATRIX) * m_InstanceDesc->iNumInstance;
	InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InstanceBufferDesc.MiscFlags = 0;
	InstanceBufferDesc.StructureByteStride = sizeof(COMPUTEMATRIX);

	ID3D11Buffer* Instance = { nullptr };


	m_InitialData.pSysMem = pCopyBlends;

	if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &m_InitialData, &Instance)))
		return E_FAIL;

	Safe_Delete_Array(pCopyBlends);	

	m_pContext->CopyResource(Instance, m_pOutBuffer);




	//이곳에서 m_pBlendSortBuffer이 버퍼를 m_pVBInstance 버퍼로 바꿔준
	D3D11_MAPPED_SUBRESOURCE		SubResourceOutput{};
	D3D11_MAPPED_SUBRESOURCE		SubResourceRender{};

	m_pContext->Map(Instance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOutput);
	m_pContext->Map(m_pRenderBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceRender);

	COMPUTEMATRIX* pOutputVertices = (COMPUTEMATRIX*)SubResourceOutput.pData;		
	VTXMATRIX* pRenderVertices = (VTXMATRIX*)SubResourceRender.pData;

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		//두매트릭스는 같은데 맨뒤에 값이 추가됫냐 안됫냐 차이
		memcpy(&pRenderVertices[i], &pOutputVertices[i], sizeof(VTXMATRIX));
	}

	m_pContext->Unmap(Instance, 0);
	m_pContext->Unmap(m_pRenderBuffer, 0);	

	Safe_Release(Instance);

	ID3D11Buffer* pVertices[] = {
		m_pVB,
		m_pRenderBuffer//컴퓨트 셰이드 거치면 값이 바뀜(원본은 계산용, 이건 렌더용)
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint pStartVertices[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_InstanceDesc->iNumInstance, 0, 0, 0);


	return S_OK;
}

void CVIBuffer_Instance::Spread(_float fTimeDelta)
{

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		pVertices[i].vLifeTime.y += fTimeDelta;
		//x가 최종,y 가 current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];


		_vector			vDir = XMLoadFloat4(&pVertices[i].vDirection);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);

				XMStoreFloat4(&pVertices[i].vDirection, XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));

				if (!m_InstanceDesc->isAttach)//항상 붙여다닐꺼?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
				}

				pVertices[i].vLifeTime.y = 0.f;

				pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);	//크기
				pVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);//회전
			}
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::RotSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		pVertices[i].vLifeTime.y += fTimeDelta;

		// 메쉬 속력 로드
		_vector MeshSpeed = XMLoadFloat3(&m_pMeshSpeed[i]);

		// 중력 설정
		_vector Gravity = XMVectorSet(0.f, -9.81f * m_InstanceDesc->GravityScale, 0.f, 0.f);

		// 속도 크기 계산
		float speedMagnitude = XMVectorGetX(XMVector3Length(MeshSpeed));

		// 각 축에 대한 속도 성분 로드
		float speedX = XMVectorGetX(MeshSpeed);
		float speedY = XMVectorGetY(MeshSpeed);
		float speedZ = XMVectorGetZ(MeshSpeed);

		// 각 축에 대한 저항 계산 (공기 저항 계수 * 속도의 제곱)
		float airResistanceX = 0.5f * m_InstanceDesc->CrossArea * speedX * speedX / m_pWeight[i];
		float airResistanceY = 0.5f * m_InstanceDesc->CrossArea * speedY * speedY / m_pWeight[i];
		float airResistanceZ = 0.5f * m_InstanceDesc->CrossArea * speedZ * speedZ / m_pWeight[i];

		// 저항 벡터 생성
		_vector airResistance = XMVectorSet(
			speedX > 0 ? -airResistanceX : airResistanceX,
			speedY > 0 ? -airResistanceY : airResistanceY,
			speedZ > 0 ? -airResistanceZ : airResistanceZ,
			0.f
		);

		// 공기 저항이 y축의 중력보다 크지 않도록 조정
		float gravityMagnitude = fabsf(XMVectorGetY(Gravity)) * m_pWeight[i];
		float airResistanceYMagnitude = fabsf(XMVectorGetY(airResistance));

		if (airResistanceYMagnitude > gravityMagnitude) {
			float scale = gravityMagnitude / airResistanceYMagnitude;
			airResistance = XMVectorScale(airResistance, scale);
		}

		// 가속도 계산 (중력 + 공기 저항 방향으로 작용)
		_vector VerAcc = XMVectorAdd(Gravity, airResistance);

		// 새로운 속력 계산
		_vector NewMeshSpeed = XMVectorAdd(MeshSpeed, XMVectorScale(VerAcc, fTimeDelta));

		// 새로운 메쉬 속력 저장
		XMStoreFloat3(&m_pMeshSpeed[i], NewMeshSpeed);

		// 좌우 앞뒤 흔들림을 위한 사인 함수 추가
		float oscillationX = 0.0f;
		float oscillationZ = 0.0f;

		// 속도가 줄어들며 물체가 내려가는 방향이 되면 좌우 앞뒤 흔들림 적용
		if (XMVectorGetY(NewMeshSpeed) < 0.f) {
			oscillationX = m_pAmplitude[i].x * sinf(m_pFrequency[i].x * pVertices[i].vLifeTime.y);
			oscillationZ = m_pAmplitude[i].z * cosf(m_pFrequency[i].z * pVertices[i].vLifeTime.y);
			NewMeshSpeed = XMVectorAdd(NewMeshSpeed, XMVectorSet(oscillationX, 0.f, oscillationZ, 0.f));
		}

		// 수직 및 수평 거리 계산
		_vector Distance = XMVectorScale(NewMeshSpeed, fTimeDelta);

		// 원래 위치와 운동 거리의 합으로 새로운 위치 계산
		_vector newTranslation = XMVectorAdd(XMLoadFloat4(&pVertices[i].vTranslation), Distance);
		XMStoreFloat4(&pVertices[i].vTranslation, newTranslation);





		//회전. 준비
		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook= XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationXMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f ,0.f , 0.f), m_pOriginalAngleVelocity[i].x *fTimeDelta);
		_matrix		RotationYMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f ,1.f , 0.f), m_pOriginalAngleVelocity[i].y * fTimeDelta);
		_matrix		RotationZMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f ,0.f , 0.f), m_pOriginalAngleVelocity[i].z * fTimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationXMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationYMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationZMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook));


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{

				_vector WorldPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

				//m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x, m_InstanceDesc->vOffsetPos.y, m_InstanceDesc->vOffsetPos.z); // Loop를 위해 저장해준다.
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f);
				XMStoreFloat4(&pVertices[i].vDirection, vDir);

				_float MeshSpeedX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.x, m_InstanceDesc->vMaxSpeed.x);
				_float MeshSpeedY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.y, m_InstanceDesc->vMaxSpeed.y);
				_float MeshSpeedZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.z, m_InstanceDesc->vMaxSpeed.z);

				m_pMeshSpeed[i] = _float3(MeshSpeedX* pVertices[i].vDirection.x, MeshSpeedY* pVertices[i].vDirection.y, MeshSpeedZ* pVertices[i].vDirection.z);



				if (!m_InstanceDesc->isAttach)//항상 붙여다닐꺼?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorldPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorldPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorldPosition);
				}

				_float StartRotX =XMConvertToRadians (m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
				_float StartRotY = XMConvertToRadians( m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
				_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

				_matrix StartRot = XMMatrixRotationX(StartRotX)* XMMatrixRotationY(StartRotY)* XMMatrixRotationZ(StartRotZ);

				XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
				XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
				XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

				_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
				_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
				_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

				m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);



				m_pWeight[i] = m_pGameInstance->Get_Random(m_InstanceDesc->fWeight.x, m_InstanceDesc->fWeight.y);

				_float FrequencyX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.x, m_InstanceDesc->vMaxFrequency.x);
				_float FrequencyY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.y, m_InstanceDesc->vMaxFrequency.y);
				_float FrequencyZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.z, m_InstanceDesc->vMaxFrequency.z);

				m_pFrequency[i] = _float3(FrequencyX, FrequencyY, FrequencyZ);

				_float AmplitudeX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.x, m_InstanceDesc->vMaxAmplitude.x);
				_float AmplitudeY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.y, m_InstanceDesc->vMaxAmplitude.y);
				_float AmplitudeZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.z, m_InstanceDesc->vMaxAmplitude.z);

				m_pAmplitude[i] = _float3(AmplitudeX, AmplitudeY, AmplitudeZ);

			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);


}

void CVIBuffer_Instance::FallSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		pVertices[i].vRectSize.y = 1.f;	

		pVertices[i].vLifeTime.y += fTimeDelta;
		//x가 최종,y 가 current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];
		
		_vector			vDir = (XMLoadFloat4(&pVertices[i].vDirection));

		_vector Hor = XMVectorSetY(vDir, 0.f);
		//수평가속도 = -1/2 × 수평 속도²(크기는 1 고정)
		_vector HorAcc = -0.5f * m_InstanceDesc->CrossArea * XMVectorMultiply(Hor, Hor);

		_vector HorVel = Hor * m_pSpeeds[i] + HorAcc * fTimeDelta;
		if (XMVectorGetX(vDir) > 0.f)
		{
			if (XMVectorGetX(HorVel) < 0.f)
				HorVel = XMVectorSetX(HorVel, 0.f);
		}
		else
		{
			if (XMVectorGetX(HorVel) >= 0.f)
				HorVel = XMVectorSetX(HorVel, 0.f);
		}
		if (XMVectorGetZ(vDir) > 0.f)
		{
			if (XMVectorGetZ(HorVel) < 0.f)
				HorVel = XMVectorSetZ(HorVel, 0.f);
		}
		else
		{
			if (XMVectorGetZ(HorVel) >= 0.f)
				HorVel = XMVectorSetZ(HorVel, 0.f);
		}

		//수평 운동 거리 = 초기 수평 속도 × 시간 + 1 / 2 × 가속도 × 시간²
		_vector HorDistance = HorVel * fTimeDelta;

		_vector Ver = XMVectorSet(0.f, XMVectorGetY(vDir), 0.f, 0.f);

		//수직가속도 = 중력 - 공기 저항 = 9.81 - 1/2 × 수직 속도²
		_vector Gravity = XMVectorSet(0.f, -9.81f, 0.f, 0.f);
		_vector VerAcc = Gravity + (-0.5f * XMVectorMultiply(Ver, Ver));

		//수직 운동 거리 = 초기 수직 속도 × 시간 + 1 / 2 × 가속도 × 시간²
		_vector VerVel = Ver * m_pSpeeds[i] + VerAcc * m_InstanceDesc->GravityScale * fTimeDelta;
		_vector VerDistance = VerVel * fTimeDelta;

		//전체 운동 거리 = (수평 운동 거리 + 수직 운동 거리) / 2
		_vector FinDistance = (HorDistance + VerDistance);

		_vector OriginPosition = XMVectorSet(pVertices[i].vTranslation.x, pVertices[i].vTranslation.y, pVertices[i].vTranslation.z, 1.f);

		_vector FinalPosition = OriginPosition + FinDistance;

		XMStoreFloat4(&pVertices[i].vTranslation, FinalPosition);


		//회전. 준비
		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook = XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationXMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].x * fTimeDelta);
		_matrix		RotationYMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pOriginalAngleVelocity[i].y * fTimeDelta);
		_matrix		RotationZMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].z * fTimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationXMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationYMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationZMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook));

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vRectSize.y = 0.f;

				_vector WorldPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				_vector			vDir = XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));
				XMStoreFloat4(&pVertices[i].vDirection, vDir);
				pVertices[i].vLifeTime.y = 0.f;

				if (!m_InstanceDesc->isAttach)//항상 붙여다닐꺼?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorldPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorldPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorldPosition);
				}

				_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
				_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
				_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

				_matrix StartRot = XMMatrixRotationRollPitchYaw(StartRotX, StartRotY, StartRotZ);

				pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
				pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
				pVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

				XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
				XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
				XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

				_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
				_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
				_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

				m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

				m_pSpeeds[i] = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);

				pVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);//회전
			}
			else
			{
				pVertices[i].vRectSize.x = 0.f;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::BloodSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_uint LiveNum = 0;
	if (0 != m_InstanceDesc->fDelay)
	{
		m_iCurrentTime += fTimeDelta;//시간추가 
		LiveNum = m_iCurrentTime / m_InstanceDesc->fDelay;

		if (LiveNum > m_InstanceDesc->iNumInstance)
			m_iCurrentTime = 0.f;
	}

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		if (0 != m_InstanceDesc->fDelay && LiveNum <= i)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vRectSize.y = 0.f;

				pVertices[i].vLifeTime.y = 0.f;

				_vector WorldPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				_vector			vDir = XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));
				XMStoreFloat4(&pVertices[i].vDirection, vDir);

				if (!m_InstanceDesc->isAttach)//항상 붙여다닐꺼?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorldPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorldPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorldPosition);
				}

				_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
				_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
				_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

				_matrix StartRot = XMMatrixRotationRollPitchYaw(StartRotX, StartRotY, StartRotZ);

				pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
				pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
				pVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

				XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
				XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
				XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

				_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
				_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
				_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

				m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

				m_pSpeeds[i] = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);

			}
			else
			{
				pVertices[i].vRectSize.x = 0.f;
			}
			continue;
		}

		pVertices[i].vRectSize.y = 1.f;

		pVertices[i].vLifeTime.y += fTimeDelta;
		//x가 최종,y 가 current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

		_vector			vDir = (XMLoadFloat4(&pVertices[i].vDirection));

		_vector Hor = XMVectorSetY(vDir, 0.f);
		//수평가속도 = -1/2 × 수평 속도²(크기는 1 고정)
		_vector HorAcc = -0.5f * m_InstanceDesc->CrossArea * XMVectorMultiply(Hor, Hor);

		_vector HorVel = Hor * m_pSpeeds[i] + HorAcc * fTimeDelta;
		if (XMVectorGetX(vDir) > 0.f)
		{
			if (XMVectorGetX(HorVel) < 0.f)
				HorVel = XMVectorSetX(HorVel, 0.f);
		}
		else
		{
			if (XMVectorGetX(HorVel) >= 0.f)
				HorVel = XMVectorSetX(HorVel, 0.f);
		}
		if (XMVectorGetZ(vDir) > 0.f)
		{
			if (XMVectorGetZ(HorVel) < 0.f)
				HorVel = XMVectorSetZ(HorVel, 0.f);
		}
		else
		{
			if (XMVectorGetZ(HorVel) >= 0.f)
				HorVel = XMVectorSetZ(HorVel, 0.f);
		}

		//수평 운동 거리 = 초기 수평 속도 × 시간 + 1 / 2 × 가속도 × 시간²
		_vector HorDistance = HorVel * fTimeDelta;

		_vector Ver = XMVectorSet(0.f, XMVectorGetY(vDir), 0.f, 0.f);

		//수직가속도 = 중력 - 공기 저항 = 9.81 - 1/2 × 수직 속도²
		_vector Gravity = XMVectorSet(0.f, -9.81f, 0.f, 0.f);
		_vector VerAcc = Gravity + (-0.5f * XMVectorMultiply(Ver, Ver));

		//수직 운동 거리 = 초기 수직 속도 × 시간 + 1 / 2 × 가속도 × 시간²
		_vector VerVel = Ver * m_pSpeeds[i] + VerAcc * m_InstanceDesc->GravityScale * fTimeDelta;
		_vector VerDistance = VerVel * fTimeDelta;

		//전체 운동 거리 = (수평 운동 거리 + 수직 운동 거리) / 2
		_vector FinDistance = (HorDistance + VerDistance);

		_vector OriginPosition = XMVectorSet(pVertices[i].vTranslation.x, pVertices[i].vTranslation.y, pVertices[i].vTranslation.z, 1.f);

		_vector FinalPosition = OriginPosition + FinDistance;

		XMStoreFloat4(&pVertices[i].vTranslation, FinalPosition);


		//회전. 준비
		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook = XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationXMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].x * fTimeDelta);
		_matrix		RotationYMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pOriginalAngleVelocity[i].y * fTimeDelta);
		_matrix		RotationZMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].z * fTimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationXMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationYMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationZMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook));
		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vRectSize.y = 0.f;
			}
		}
	
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::MeshSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		// 생명 시간 업데이트
		pVertices[i].vLifeTime.y += fTimeDelta; // 생명 시간 업데이트

		// 방향 벡터 정규화
		XMVECTOR vDir = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vDirection));
		XMVECTOR vNewDir = vDir;

		// 현재 속도 로드
		XMVECTOR vSpeed = XMLoadFloat3(&m_pMeshSpeed[i]);

		// 중력 계산
		XMVECTOR Gravity = XMVectorSet(0.f, -m_InstanceDesc->GravityScale * m_pWeight[i], 0.f, 0.f);	

		// 공기 저항 계산 (속도의 제곱에 비례)
		float airResistance = 0.5f * m_InstanceDesc->CrossArea * pow(XMVectorGetY(vSpeed), 2);
		XMVECTOR VerAcc = Gravity - XMVectorSet(0.f, airResistance, 0.f, 0.f);

		// 새로운 방향에 수직 가속도 추가
		vNewDir = XMVectorAdd(vNewDir, VerAcc * fTimeDelta);

		// 수직 속도 및 거리 계산
		XMVECTOR VerVel = vNewDir * fTimeDelta;

		// 새로운 방향 저장
		XMStoreFloat4(&pVertices[i].vDirection, vNewDir);

		// 현재 속도에서 Y 방향 속도 추출
		float currentSpeedY = XMVectorGetY(vNewDir);
		float currentSpeedXZ = sqrt(pow(XMVectorGetX(vSpeed), 2) + pow(XMVectorGetZ(vSpeed), 2));

		// XZ 방향으로의 이동 계산 (속도에 비례)
		float xOffset = currentSpeedXZ * sin(m_pFrequency[i].x * pVertices[i].vLifeTime.y) * m_pAmplitude[i].x;
		float zOffset = currentSpeedXZ * cos(m_pFrequency[i].z * pVertices[i].vLifeTime.y) * m_pAmplitude[i].z;

		// 원래 위치와 운동 거리의 합으로 새로운 위치 계산
		XMVECTOR OriginPosition = XMLoadFloat4(&pVertices[i].vTranslation);
		XMVECTOR newPosition = OriginPosition + VerVel + XMVectorSet(xOffset, 0.0f, zOffset, 0.0f);
		XMStoreFloat4(&pVertices[i].vTranslation, newPosition);

		// 흔들림 효과 추가
		float shakeX = m_pAmplitude[i].x * sin(m_pFrequency[i].x * pVertices[i].vLifeTime.y);
		float shakeY = m_pAmplitude[i].y * sin(m_pFrequency[i].y * pVertices[i].vLifeTime.y);
		float shakeZ = m_pAmplitude[i].z * sin(m_pFrequency[i].z * pVertices[i].vLifeTime.y);

		// 새로운 위치에 흔들림 추가
		XMVECTOR shakeOffset = XMVectorSet(shakeX, shakeY, shakeZ, 0.0f);
		newPosition = newPosition + shakeOffset;
		XMStoreFloat4(&pVertices[i].vTranslation, newPosition);

		// Y 방향이 음수일 때 속도 감소
		if (XMVectorGetY(vNewDir) < 0.f) {
			pVertices[i].vDirection.y = 0.f; // 더 이상 상승하지 않도록 설정
		}







		//회전. 준비
		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook = XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationXMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].x * fTimeDelta);
		_matrix		RotationYMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pOriginalAngleVelocity[i].y * fTimeDelta);
		_matrix		RotationZMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].z * fTimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationXMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationYMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationZMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook));


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				//m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x, m_InstanceDesc->vOffsetPos.y, m_InstanceDesc->vOffsetPos.z); // Loop를 위해 저장해준다.
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f);
				XMStoreFloat4(&pVertices[i].vDirection, vDir);

				_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
				_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
				_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

				_matrix StartRot = XMMatrixRotationX(StartRotX) * XMMatrixRotationY(StartRotY) * XMMatrixRotationZ(StartRotZ);

				XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
				XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
				XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

				_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
				_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
				_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

				m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

				_float MeshSpeedX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.x, m_InstanceDesc->vMaxSpeed.x);
				_float MeshSpeedY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.y, m_InstanceDesc->vMaxSpeed.y);
				_float MeshSpeedZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.z, m_InstanceDesc->vMaxSpeed.z);

				m_pMeshSpeed[i] = _float3(MeshSpeedX, MeshSpeedY, MeshSpeedZ);

				m_pWeight[i] = m_pGameInstance->Get_Random(m_InstanceDesc->fWeight.x, m_InstanceDesc->fWeight.y);

				_float FrequencyX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.x, m_InstanceDesc->vMaxFrequency.x);
				_float FrequencyY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.y, m_InstanceDesc->vMaxFrequency.y);
				_float FrequencyZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.z, m_InstanceDesc->vMaxFrequency.z);

				m_pFrequency[i] = _float3(FrequencyX, FrequencyY, FrequencyZ);

				_float AmplitudeX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.x, m_InstanceDesc->vMaxAmplitude.x);
				_float AmplitudeY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.y, m_InstanceDesc->vMaxAmplitude.y);
				_float AmplitudeZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.z, m_InstanceDesc->vMaxAmplitude.z);

				m_pAmplitude[i] = _float3(AmplitudeX, AmplitudeY, AmplitudeZ);

			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::IntervalSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_uint LiveNum = 0;
	_uint Next = 0;

	_bool LastFin = false;

	if (0 != m_InstanceDesc->fDelay)
	{
		m_iCurrentTime += fTimeDelta;//시간추가 
		LiveNum = m_iCurrentTime / m_InstanceDesc->fDelay;	

	}

	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{

		if (pVertices[i].vLifeTime.y > pVertices[i].vLifeTime.x)
		{
			pVertices[i].vRectSize.x = 0.f;
		}

		if (i < LiveNum && pVertices[i].vLifeTime.y <= pVertices[i].vLifeTime.x)
		{
			if (false == m_pIsPlay[i])
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);

				XMStoreFloat4(&pVertices[i].vDirection, XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));

				pVertices[i].vRectSize.x = m_pOriginalSize[i];

				if (!m_InstanceDesc->isAttach)//항상 붙여다닐꺼?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
				}
				m_pIsPlay[i] = true;
			}

			pVertices[i].vLifeTime.y += fTimeDelta;
			//x가 최종,y 가 current

			_vector			vDir = XMLoadFloat4(&pVertices[i].vDirection);

			XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
		}
		else
		{
			pVertices[i].vRectSize.x = 0.f;
		}

	}

	if (m_InstanceDesc->isLoop)
	{
		if (LiveNum > m_InstanceDesc->iNumInstance)
		{
			Reset();
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;	

		//x가 최종,y 가 current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

		pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
		_vector			vDir = XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));
		XMStoreFloat4(&pVertices[i].vDirection, vDir);
		pVertices[i].vLifeTime.y = 0.f;

		if (!m_InstanceDesc->isAttach)//항상 붙여다닐꺼?
		{
			pVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
			pVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
			pVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
		}

		_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
		_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
		_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

		_matrix StartRot = XMMatrixRotationX(StartRotX) * XMMatrixRotationY(StartRotY) * XMMatrixRotationZ(StartRotZ);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

		_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
		_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
		_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

		m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

		m_pSpeeds[i] = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);

		pVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);//회전

		if(0!=m_InstanceDesc->fDelay)
		{
			m_pIsPlay[i] = false;
			m_iCurrentTime = 0.f;
		}
	}

	m_isReset = true;
	m_pContext->Unmap(m_pVBInstance, 0);
}

_bool CVIBuffer_Instance::LifeTime_Check()
{

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		if (pVertices[i].vLifeTime.y <pVertices[i].vLifeTime.x)
		{
			return false;
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
	

	return true;
}

void CVIBuffer_Instance::SizeUp_Time(_float fTimeDelta)
{
	if(!m_isReset)
	{
		D3D11_MAPPED_SUBRESOURCE		SubResource{};

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
		{
			VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

			//x가 최종,y 가 current

			pVertices[i].vRectSize.x = (1.f + (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x)) * m_pOriginalSize[i];

			//if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
			//{
			//	if (true == m_InstanceDesc->isLoop)
			//	{
			//		pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
			//		pVertices[i].vLifeTime.y = 0.f;
			//		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);
			//		XMStoreFloat4(&pVertices[i].vDirection, vDir);
			//		pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);
			//	}
			//}

		}

		m_pContext->Unmap(m_pVBInstance, 0);
	}
}

void CVIBuffer_Instance::SizeDown_Time(_float fTimeDelta)
{
	if (!m_isReset)
	{
		D3D11_MAPPED_SUBRESOURCE		SubResource{};

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
		{
			VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

			//x가 최종,y 가 current
			pVertices[i].vRectSize.x = (1.f - (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x)) * m_pOriginalSize[i];

			//if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
			//{
			//	if (true == m_InstanceDesc->isLoop)
			//	{
			//		pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
			//		pVertices[i].vLifeTime.y = 0.f;
			//		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);
			//		XMStoreFloat4(&pVertices[i].vDirection, vDir);
			//		pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);
			//	}
			//}

		}

		m_pContext->Unmap(m_pVBInstance, 0);
	}
}
//
//void CVIBuffer_Instance::Leaf_Fall(_float fTimeDelta)
//{
//	bool allInstancesDead = true;
//	D3D11_MAPPED_SUBRESOURCE      SubResource{};
//	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
//	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
//	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
//	{
//
//		pVertices[i].vLifeTime.y += fTimeDelta;
//
//		_float fRatio = pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x;
//		_vector vRight, vUp, vLook, vPos;
//		vRight = XMLoadFloat4(&pVertices[i].vRight);
//		vUp = XMLoadFloat4(&pVertices[i].vUp);
//		vLook = XMLoadFloat4(&pVertices[i].vLook);
//		vPos = XMLoadFloat4(&pVertices[i].vTranslation);
//
//		if (fRatio <= 0.2f)
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//		else if (fRatio > 0.2f && fRatio <= 0.4f)
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//		else if (fRatio > 0.4f && fRatio <= 0.6f)
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//		else if (fRatio > 0.6f && fRatio <= 0.8f)
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//		else
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//
//		_vector vDir = XMVector4Normalize(vLook);
//		vPos += vDir * m_pSpeeds[i] * fTimeDelta;
//
//
//
//		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
//		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp) );
//		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook) );
//		XMStoreFloat4(&pVertices[i].vTranslation, vPos);	
//
//		pVertices[i].vTranslation.y -=0.98f * m_InstanceDesc->GravityScale* fTimeDelta;
//
//		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
//		{
//			if (true == m_InstanceDesc->isLoop)
//			{
//				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
//				pVertices[i].vLifeTime.y = 0.f;
//			}
//			else
//			{
//				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
//			}
//		}
//
//	}
//
//	m_pContext->Unmap(m_pVBInstance, 0);
//
//
//
//
//}

void CVIBuffer_Instance::Compute_Sort()
{
	//버퍼에 복사 해둠.
	D3D11_MAPPED_SUBRESOURCE		SubResourceOrigin{};
	D3D11_MAPPED_SUBRESOURCE		SubResourceCopy{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOrigin);	
	//m_pContext->Map(m_pOutBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceCopy);	

	VTXMATRIX* pVertices = (VTXMATRIX*)SubResourceOrigin.pData;

	COMPUTEMATRIX* pCopyBlends = new COMPUTEMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pCopyBlends, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	_vector CamPos = m_pGameInstance->Get_CamPosition();

	ZeroMemory(pCopyBlends, sizeof(COMPUTEMATRIX) * m_InstanceDesc->iNumInstance);//계산용은 초기화후 사용

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		memcpy(&pCopyBlends[i], &pVertices[i], sizeof(VTXMATRIX));	

		memcpy(&pCopyBlends[i].vCamPos, &CamPos, sizeof(_float4));
	}

//	m_pContext->Unmap(m_pOutBuffer, 0);
	m_pContext->Unmap(m_pVBInstance, 0);

	m_InitialData.pSysMem = pCopyBlends;

	if (FAILED(m_pDevice->CreateBuffer(&m_OutBufferDesc, &m_InitialData, &m_pOutBuffer)))
		return ;
	//셰이더에서 읽기,쓰기.
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pOutBuffer, &uavDesc, &mOutputUAV)))
		return ;


}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pOriginalPositions);
		Safe_Delete_Array(m_pOriginalOffsets);
		Safe_Delete_Array(m_pOriginalSize);
		Safe_Delete_Array(m_pOriginalAngleVelocity);
		Safe_Delete_Array(m_pMeshSpeed);
		Safe_Delete_Array(m_pWeight);
		Safe_Delete_Array(m_pFrequency);
		Safe_Delete_Array(m_pAmplitude);
		Safe_Delete_Array(m_pIsPlay);
	}

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pRenderBuffer);
	Safe_Release(m_pOutBuffer);
	Safe_Release(mOutputUAV);
	Safe_Release(m_pVBInstance);
}
