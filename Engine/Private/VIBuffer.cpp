#include "VIBuffer.h"

#include "ComputeShader.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{pDevice, pContext}
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent{rhs},
	m_pIB { rhs.m_pIB},
	m_pVB{ rhs.m_pVB },
	m_Buffer_Desc {rhs.m_Buffer_Desc},
	m_InitialData { rhs.m_InitialData},
	m_iNumVertices {rhs.m_iNumVertices},
	m_iVertexStride { rhs.m_iVertexStride},
	m_iNumIndices { rhs.m_iNumIndices},
	m_iIndexStride { rhs.m_iIndexStride},
	m_GIFormat { rhs.m_GIFormat},
	m_Primitive_Topology{ rhs.m_Primitive_Topology },
	m_iNumVertexBuffers { rhs.m_iNumVertexBuffers }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer* pVertices[] = {
		m_pVB,
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
	};

	_uint pStartVertices[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);
	
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Render_Compute()
{
	ID3D11Buffer* pVertices[] = {
		m_pVB,
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
	};

	_uint pStartVertices[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** pOut)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_Buffer_Desc, &m_InitialData, pOut)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Bind_Compute(CComputeShader* pShader)
{
	if (m_pVertexBufferSRV == nullptr && m_pResultBufferUAV == nullptr)
	{

	}

	m_pContext->CopyResource(m_pSRVIn, m_pVB);
	m_pContext->CSSetShaderResources(0, 1, &m_pVertexBufferSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pResultBufferUAV, nullptr);

	_uint iGroupNum = (m_iNumVertices + 63) / 64;

	pShader->Render(iGroupNum, 1, 1);

	return S_OK;
}

HRESULT CVIBuffer::Bind_Compute_AABBCube(CComputeShader* pShader)
{
	m_pContext->CSSetShaderResources(0, 1, &m_pVertexBufferSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pResultBufferUAV, nullptr);

	_uint iGroupNum = (m_iNumVertices + 7) / 8;

	pShader->Render(iGroupNum, 1, 1);

	return S_OK;
}

HRESULT CVIBuffer::Ready_BoneBuffer()
{
	// 처리된 결과를 위한 Unordered Access View 생성
	D3D11_BUFFER_DESC srvbufferDesc = {};
	srvbufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	srvbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	srvbufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	srvbufferDesc.CPUAccessFlags = 0;
	srvbufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	srvbufferDesc.StructureByteStride = m_iVertexStride;

	if (FAILED(m_pDevice->CreateBuffer(&srvbufferDesc, nullptr, &m_pSRVIn)))
		return E_FAIL;

	// 정점 버퍼를 위한 Shader Resource View 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 정점 버퍼의 형식에 따라 적절히 설정
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_iNumVertices;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pSRVIn, &srvDesc, &m_pVertexBufferSRV)))
		return E_FAIL;

	// 처리된 결과를 위한 Unordered Access View 생성
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(VTXANIMBONE) * m_iNumVertices;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(VTXANIMBONE);

	if (FAILED(m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pUAVOut)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN; // 처리된 결과의 형식에 따라 적절히 설정
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_iNumVertices;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pUAVOut, &uavDesc, &m_pResultBufferUAV)))
		return E_FAIL;

	D3D11_BUFFER_DESC Desc{};
	Desc.ByteWidth = sizeof(VTXANIMBONE) * m_iNumVertices;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = sizeof(VTXANIMBONE);

	if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pProcessedVertexBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Ready_AABBCubeBuffer()
{
	// 처리된 결과를 위한 Unordered Access View 생성
	D3D11_BUFFER_DESC SRVbufferDesc = {};
	SRVbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	SRVbufferDesc.ByteWidth = sizeof(VTXCUBE_OCCULUSION) * m_iNumVertices;
	SRVbufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	SRVbufferDesc.CPUAccessFlags = 0;
	SRVbufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	SRVbufferDesc.StructureByteStride = sizeof(VTXCUBE_OCCULUSION);

	if (FAILED(m_pDevice->CreateBuffer(&SRVbufferDesc, nullptr, &m_pSRVIn)))
		return E_FAIL;

	// 정점 버퍼를 위한 Shader Resource View 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 정점 버퍼의 형식에 따라 적절히 설정
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_iNumVertices;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pSRVIn, &srvDesc, &m_pVertexBufferSRV)))
		return E_FAIL;

	// 처리된 결과를 위한 Unordered Access View 생성
	D3D11_BUFFER_DESC UAVbufferDesc = {};
	UAVbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	UAVbufferDesc.ByteWidth = sizeof(int) * m_iNumVertices;
	UAVbufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	UAVbufferDesc.CPUAccessFlags = 0;
	UAVbufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	UAVbufferDesc.StructureByteStride = sizeof(int);

	if (FAILED(m_pDevice->CreateBuffer(&UAVbufferDesc, nullptr, &m_pUAVOut)))
		return E_FAIL;

	// Unordered Access View (UAV) 생성
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN; // 처리된 결과의 형식에 따라 적절히 설정
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_iNumVertices;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pUAVOut, &uavDesc, &m_pResultBufferUAV)))
		return E_FAIL;

	D3D11_BUFFER_DESC Desc{};
	Desc.ByteWidth = sizeof(VTXCUBE_OCCULUSION) * m_iNumVertices;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = sizeof(VTXCUBE_OCCULUSION);

	if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pProcessedVertexBuffer)))
		return E_FAIL;
	
	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pUAVOut);
	Safe_Release(m_pIB);
	Safe_Release(m_pSRVIn);

	Safe_Release(m_pVertexBufferSRV);
	Safe_Release(m_pResultBufferUAV);
	Safe_Release(m_pProcessedVertexBuffer);
}
