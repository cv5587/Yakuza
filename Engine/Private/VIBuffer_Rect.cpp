#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& rhs)
	: CVIBuffer{rhs}
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	// Á¤Á¡ 
//	UINT ByteWidth;
//	D3D11_USAGE Usage;
//	UINT BindFlags;
//	UINT CPUAccessFlags;
//	UINT MiscFlags;
//	UINT StructureByteStride;
//} 	D3D11_BUFFER_DESC;

	m_GIFormat = DXGI_FORMAT_R16_UINT;
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertexts = new VTXPOSTEX[m_iNumVertices];

	pVertexts[0].vPosition = _float3{ -0.5f, 0.5f, 0.f };
	pVertexts[0].vTexcoord = _float2{ 0.f, 0.f };

	pVertexts[1].vPosition = _float3{ 0.5f, 0.5f, 0.f };
	pVertexts[1].vTexcoord = _float2{ 1.f, 0.f };

	pVertexts[2].vPosition = _float3{ 0.5f, -0.5f, 0.f };
	pVertexts[2].vTexcoord = _float2{ 1.f, 1.f };

	pVertexts[3].vPosition = _float3{ -0.5f, -0.5f, 0.f };
	pVertexts[3].vTexcoord = _float2{ 0.f, 1.f };

	m_InitialData.pSysMem = pVertexts;

	__super::Create_Buffer(&m_pVB);

	Safe_Delete_Array(pVertexts);

	m_iNumIndices = 6;
	m_iIndexStride = 2;

	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	m_InitialData.pSysMem = pIndices;

	__super::Create_Buffer(&m_pIB);

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
	if(nullptr!= pArg)
	{
		RECT_DESC* pDesc = static_cast<RECT_DESC*>(pArg);

		_float fX = pDesc->fStartUV.x;
		_float fY = pDesc->fStartUV.y;
		_float fWeightX = pDesc->fEndUV.x - pDesc->fStartUV.x;
		_float fWeightY = pDesc->fEndUV.y - pDesc->fStartUV.y;

		Safe_Release(m_pVB);
		Safe_Release(m_pIB);
		m_GIFormat = DXGI_FORMAT_R16_UINT;
		m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		m_iNumVertexBuffers = 1;
		m_iNumVertices = 4;
		m_iVertexStride = sizeof(VTXPOSTEX);

		m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
		m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_Buffer_Desc.MiscFlags = 0;
		m_Buffer_Desc.StructureByteStride = m_iVertexStride;

		VTXPOSTEX* pVertexts = new VTXPOSTEX[m_iNumVertices];

		pVertexts[0].vPosition = _float3{ -0.5f+pDesc->vUpPoint.x, 0.5f + pDesc->vUpPoint.y, 0.f };
		pVertexts[0].vTexcoord = { fX,fY };

		pVertexts[1].vPosition = _float3{ 0.5f + pDesc->vUpPoint.z, 0.5f + pDesc->vUpPoint.w, 0.f };
		pVertexts[1].vTexcoord = { fX + fWeightX,fY };

		pVertexts[2].vPosition = _float3{ 0.5f + pDesc->vDownPoint.z, -0.5f + pDesc->vDownPoint.w, 0.f };
		pVertexts[2].vTexcoord = { fX + fWeightX,fY + fWeightY };

		pVertexts[3].vPosition = _float3{ -0.5f + pDesc->vDownPoint.x, -0.5f + pDesc->vDownPoint.y, 0.f };
		pVertexts[3].vTexcoord = { fX,fY + fWeightY };

		m_InitialData.pSysMem = pVertexts;

		__super::Create_Buffer(&m_pVB);

		Safe_Delete_Array(pVertexts);

		m_iNumIndices = 6;
		m_iIndexStride = 2;

		m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_Buffer_Desc.CPUAccessFlags = 0;
		m_Buffer_Desc.MiscFlags = 0;
		m_Buffer_Desc.StructureByteStride = 0;

		_ushort* pIndices = new _ushort[m_iNumIndices];
		ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

		pIndices[0] = 0;
		pIndices[1] = 1;
		pIndices[2] = 2;

		pIndices[3] = 0;
		pIndices[4] = 2;
		pIndices[5] = 3;

		m_InitialData.pSysMem = pIndices;

		__super::Create_Buffer(&m_pIB);

		Safe_Delete_Array(pIndices);
	}

	return S_OK;
}

HRESULT CVIBuffer_Rect::EditUV(_float2 fStartUV, _float2 fEndUV)	
{
	_float fX = fStartUV.x;
	_float fY = fStartUV.y;
	_float fWeightX = fEndUV.x - fStartUV.x;
	_float fWeightY = fEndUV.y - fStartUV.y;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* pVertices = (VTXPOSTEX*)SubResource.pData;	

	pVertices[0].vTexcoord = {fX,fY};
	pVertices[1].vTexcoord = {fX+fWeightX,fY};
	pVertices[2].vTexcoord = {fX+fWeightX,fY+fWeightY};
	pVertices[3].vTexcoord = {fX,fY+fWeightY};

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Rect::Edit_Point(_float4 vUpPoint, _float4 vDownPoint)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* pVertices = (VTXPOSTEX*)SubResource.pData;

	pVertices[0].vPosition = _float3{ -0.5f + vUpPoint.x, 0.5f + vUpPoint.y, 0.f };

	pVertices[1].vPosition = _float3{ 0.5f + vUpPoint.z, 0.5f + vUpPoint.w, 0.f };

	pVertices[2].vPosition = _float3{ 0.5f +vDownPoint.z, -0.5f +vDownPoint.w, 0.f };

	pVertices[3].vPosition = _float3{ -0.5f +vDownPoint.x, -0.5f +vDownPoint.y, 0.f };

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
		

	return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
	CComponent* pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();
}
