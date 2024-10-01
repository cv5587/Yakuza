#include "VIBuffer_Instance_Rect.h"
#include "GameInstance.h"
CVIBuffer_Instance_Rect::CVIBuffer_Instance_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Instance_Rect::CVIBuffer_Instance_Rect(const CVIBuffer_Instance_Rect& rhs)
	: CVIBuffer_Instance { rhs }
{
}

HRESULT CVIBuffer_Instance_Rect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance_Rect::Initialize(void* pArg)
{
	INSTANCE_RECT_DESC* pDesc = static_cast<INSTANCE_RECT_DESC*>(pArg);
	m_pParentMatrix[0] = pDesc->pNeckMatrix;
	m_pParentMatrix[1] = pDesc->pLHandMatrix;
	m_pParentMatrix[2] = pDesc->pRHandMatrix;
	m_pParentMatrix[3]= pDesc->pLFootMatrix;
	m_pParentMatrix[4]= pDesc->pRFootMatrix;

	m_iInstanceStride = sizeof(VTXMATRIX);


	m_InstanceDesc = new INSTANCE_DESC();
	m_InstanceDesc->iNumInstance = pDesc->iNumInstance;
	m_InstanceDesc->vRectSize = pDesc->vRectSize;
	m_InstanceDesc->vSize = pDesc->vSize;
	m_InstanceDesc->WorldMatrix = pDesc->WorldMatrix;

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
	pVertexts[0].vPSize = _float2{ m_InstanceDesc->vSize.x, m_InstanceDesc->vSize.y };

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

	m_pCurrentWorldMatrix = m_InstanceDesc->WorldMatrix;

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		_float	fRectSize = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);

		// Right, Up, Loop, Pos 순서로 월드 행렬의 좌표를 넣어준다.
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		_matrix WorldMatrix = XMLoadFloat4x4(m_pParentMatrix[i]) ;

		XMStoreFloat4(&pInstanceVertices[i].vTranslation, WorldMatrix.r[3]);
	//	pInstanceVertices[i].vTranslation = _float4(m_pParentMatrix[i]->_41+ m_pCurrentWorldMatrix->_41, m_pParentMatrix[i]->_42 + m_pCurrentWorldMatrix->_42, m_pParentMatrix[i]->_43 + m_pCurrentWorldMatrix->_43, 1.f);
	}

	m_InitialData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;


	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

void CVIBuffer_Instance_Rect::Rim_tick(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
		//_matrix World = XMLoadFloat4x4(m_pParentMatrix[i]);

		_matrix WorldMatrix = XMLoadFloat4x4(m_pParentMatrix[i]) ;

		XMStoreFloat4(&pVertices[i].vTranslation, WorldMatrix.r[3]);
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Instance_Rect* CVIBuffer_Instance_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Instance_Rect* pInstance = new CVIBuffer_Instance_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Instance_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instance_Rect::Clone(void* pArg)
{
	CVIBuffer_Instance_Rect* pInstance = new CVIBuffer_Instance_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Instance_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instance_Rect::Free()
{
	__super::Free();
}
