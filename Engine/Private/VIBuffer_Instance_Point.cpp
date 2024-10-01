#include "VIBuffer_Instance_Point.h"
#include "GameInstance.h"

CVIBuffer_Instance_Point::CVIBuffer_Instance_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Instance_Point::CVIBuffer_Instance_Point(const CVIBuffer_Instance_Point& rhs)
	: CVIBuffer_Instance{ rhs }
{
}

HRESULT CVIBuffer_Instance_Point::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance_Point::Initialize(void* pArg)
{

	m_iInstanceStride = sizeof(VTXMATRIX);

	if (nullptr != pArg)
	{
		INSTANCE_POINT_DESC* pDesc = static_cast<INSTANCE_POINT_DESC*>(pArg);

	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_GIFormat = DXGI_FORMAT_R16_UINT;
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOINT);

	m_iIndexStride = 2;
	m_iIndexCountPerInstance = 1;
	m_iNumIndices = m_iIndexCountPerInstance * m_InstanceDesc->iNumInstance;
	//m_iInstanceStride = sizeof(VTXMATRIX);

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXPOINT* pVertexts = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertexts, sizeof(VTXPOINT) * m_iNumVertices);

	pVertexts[0].vPosition = _float3{ 0.f, 0.f, 0.f };
	pVertexts[0].vPSize = _float2{ m_InstanceDesc->vSize.x, m_InstanceDesc->vSize. y};

	m_InitialData.pSysMem = pVertexts;

	__super::Create_Buffer(&m_pVB);

	Safe_Delete_Array(pVertexts);

#pragma endregion


#pragma region INDEX_BUFFER
	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	m_InitialData.pSysMem = pIndices;

	__super::Create_Buffer(&m_pIB);
	
	Safe_Delete_Array(pIndices);
#pragma endregion

#pragma region INSTANCE_BUFFER
	// 파티클들의 월드 좌표를 저장하기 위한 버퍼
	// 좌표 뿐만 아니라 다양한 정보를 저장하여 사용한다.
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_InstanceDesc->iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	m_pSpeeds = new _float[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_InstanceDesc->iNumInstance);

	m_pOriginalPositions = new _float3[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pOriginalPositions, sizeof(_float3) * m_InstanceDesc->iNumInstance);

	m_pOriginalSize = new _float[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pOriginalSize, sizeof(_float) * m_InstanceDesc->iNumInstance);

	m_pOriginalOffsets = new _float3[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pOriginalOffsets, sizeof(_float3) * m_InstanceDesc->iNumInstance);

	//돈 용 제작
	m_pOriginalAngleVelocity = new _float3[m_InstanceDesc->iNumInstance];
	ZeroMemory(m_pOriginalAngleVelocity, sizeof(_float3) * m_InstanceDesc->iNumInstance);

	if (0 != m_InstanceDesc->fDelay)
	{
		m_pIsPlay = new _bool[m_InstanceDesc->iNumInstance];
		ZeroMemory(m_pIsPlay, sizeof(_bool) * m_InstanceDesc->iNumInstance);
	}

	m_pCurrentWorldMatrix = m_InstanceDesc->WorldMatrix;

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{	
	_float	fRectSize = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);
	

	_float Speed = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);	
	_float RangeX = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.x - m_InstanceDesc->vRange.x * 0.5f, m_InstanceDesc->vPivotPos.x + m_InstanceDesc->vRange.x * 0.5f);
	_float RangeY = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.y - m_InstanceDesc->vRange.y * 0.5f, m_InstanceDesc->vPivotPos.y + m_InstanceDesc->vRange.y * 0.5f);
	_float RangeZ = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.z - m_InstanceDesc->vRange.z * 0.5f, m_InstanceDesc->vPivotPos.z + m_InstanceDesc->vRange.z * 0.5f);

	_float RadiusX = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.x -m_InstanceDesc->fRadius, m_InstanceDesc->vPivotPos.x+m_InstanceDesc->fRadius);
	_float RadiusY = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.y -m_InstanceDesc->fRadius, m_InstanceDesc->vPivotPos.y+ m_InstanceDesc->fRadius);
	_float RadiusZ = m_pGameInstance->Get_Random(m_InstanceDesc->vPivotPos.z  -m_InstanceDesc->fRadius, m_InstanceDesc->vPivotPos.z+m_InstanceDesc->fRadius);


	_float LifeTime = m_pGameInstance->Get_Random(m_InstanceDesc->vLifeTime.x, m_InstanceDesc->vLifeTime.y);

	
		_vector WorlPosition= XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];		

		pInstanceVertices[i].vRectSize.y = 0.f;

		if(m_InstanceDesc->isBillboard)
		{
			if (m_InstanceDesc->bRadius)
				pInstanceVertices[i].vTranslation = _float4(RadiusX, RadiusY, RadiusZ, 1.f);
			else
				pInstanceVertices[i].vTranslation = _float4(RangeX, RangeY, RangeZ, 1.f);

			_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
			_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
			_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

			_matrix StartRot = XMMatrixRotationX(StartRotX) * XMMatrixRotationY(StartRotY) * XMMatrixRotationZ(StartRotZ);

			XMStoreFloat4(&pInstanceVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
			XMStoreFloat4(&pInstanceVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
			XMStoreFloat4(&pInstanceVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

			m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x, m_InstanceDesc->vOffsetPos.y, m_InstanceDesc->vOffsetPos.z); // Loop를 위해 저장해준다.
			m_pOriginalPositions[i] = _float3(pInstanceVertices[i].vTranslation.x, pInstanceVertices[i].vTranslation.y, pInstanceVertices[i].vTranslation.z);
			XMStoreFloat4(&pInstanceVertices[i].vDirection, XMVectorSetW(XMLoadFloat4(&pInstanceVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));

			if(!m_InstanceDesc->isAttach)//항상 붙여다닐꺼?
			{
				pInstanceVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
				pInstanceVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
				pInstanceVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
			}

			pInstanceVertices[i].vLifeTime.x = LifeTime; // 파티클이 살아있을 수 있는 시간.

			m_pOriginalSize[i] = pInstanceVertices[i].vRectSize.x = fRectSize;

			
			_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
			_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
			_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

			m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

			m_pSpeeds[i] = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);


		}
		else
		{
			

			// Look을 랜덤으로 잡아주면 나머지 계산해주기(hlsl에서)
			_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
			_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
			_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

			_matrix StartRot = XMMatrixRotationRollPitchYaw(StartRotX, StartRotY, StartRotZ);

			pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
			pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
			pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

			XMStoreFloat4(&pInstanceVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
			XMStoreFloat4(&pInstanceVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
			XMStoreFloat4(&pInstanceVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

			if (m_InstanceDesc->bRadius)
				pInstanceVertices[i].vTranslation = _float4(RadiusX, RadiusY, RadiusZ, 1.f);
			else
				pInstanceVertices[i].vTranslation = _float4(RangeX, RangeY, RangeZ, 1.f);

			m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x, m_InstanceDesc->vOffsetPos.y , m_InstanceDesc->vOffsetPos.z ); // Loop를 위해 저장해준다.
			m_pOriginalPositions[i] = _float3(pInstanceVertices[i].vTranslation.x , pInstanceVertices[i].vTranslation.y , pInstanceVertices[i].vTranslation.z ); // Loop를 위해 저장해준다.
			
			pInstanceVertices[i].vLifeTime.x = LifeTime; // 파티클이 살아있을 수 있는 시간.
			m_pOriginalSize[i] = pInstanceVertices[i].vRectSize.x = fRectSize;
			XMStoreFloat4(&pInstanceVertices[i].vDirection, XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pInstanceVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f)));

			if (!m_InstanceDesc->isAttach)//항상 붙여다닐꺼?
			{
				pInstanceVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
				pInstanceVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
				pInstanceVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
			}

			pInstanceVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);


			m_pSpeeds[i] = Speed;
		}
		if (0 != m_InstanceDesc->fDelay)
		{
			pInstanceVertices[i].vRectSize.x = 0.f;
			m_pIsPlay[i] = false;
		}
	}
	
	m_InitialData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;


	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

CVIBuffer_Instance_Point* CVIBuffer_Instance_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Instance_Point* pInstance = new CVIBuffer_Instance_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Instance_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instance_Point::Clone(void* pArg)
{
	CVIBuffer_Instance_Point* pInstance = new CVIBuffer_Instance_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Instance_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instance_Point::Free()
{
	__super::Free();
}
