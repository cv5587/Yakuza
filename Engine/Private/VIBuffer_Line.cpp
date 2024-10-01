#include "VIBuffer_Line.h"

#include "Cell.h"

CVIBuffer_Line::CVIBuffer_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Line::CVIBuffer_Line(const CVIBuffer_Line& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CVIBuffer_Line::Initialize_Prototype(list<CCell*> Cells)
{
	m_GIFormat = DXGI_FORMAT_R16_UINT;
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	//m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumVertices = Cells.size();
	m_iVertexStride = sizeof(VTXPOS);

	m_iNumVertexBuffers = 1;
	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertexts = new VTXPOS[m_iNumVertices];

	auto iter = Cells.begin();
	for (size_t i = 0; i < Cells.size(); ++i)
	{
		XMStoreFloat3(&pVertexts[i].vPosition, (*iter)->Get_WayPoint());
		iter++;
	}

	m_InitialData.pSysMem = pVertexts;

	__super::Create_Buffer(&m_pVB);

	Safe_Delete_Array(pVertexts);

	m_iNumIndices = Cells.size();
	m_iIndexStride = 2;

	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	for (size_t i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = i;
	}

	m_InitialData.pSysMem = pIndices;

	__super::Create_Buffer(&m_pIB);

	Safe_Delete_Array(pIndices);
	return S_OK;
	
}

HRESULT CVIBuffer_Line::Initialize_Prototype(vector<ROUTE_IO> routes)
{
	m_GIFormat = DXGI_FORMAT_R16_UINT;
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	//m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumVertices = routes.size();
	m_iVertexStride = sizeof(VTXPOS);

	m_iNumVertexBuffers = 1;
	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertexts = new VTXPOS[m_iNumVertices];

	auto iter = routes.begin();
	for (size_t i = 0; i < routes.size(); ++i)
	{
		_float3 vPos;
		vPos = _float3((*iter).vPosition.x, (*iter).vPosition.y, (*iter).vPosition.z);
		pVertexts[i].vPosition = vPos;
		iter++;
	}

	m_InitialData.pSysMem = pVertexts;

	__super::Create_Buffer(&m_pVB);

	Safe_Delete_Array(pVertexts);

	m_iNumIndices = routes.size();
	m_iIndexStride = 2;

	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = 0;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	for (size_t i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = i;
	}

	m_InitialData.pSysMem = pIndices;

	__super::Create_Buffer(&m_pIB);

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CVIBuffer_Line::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Line* CVIBuffer_Line::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, list<class CCell*> Cells)
{
	CVIBuffer_Line* pInstance = new CVIBuffer_Line(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Cells)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Line* CVIBuffer_Line::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<ROUTE_IO> routes)
{
	CVIBuffer_Line* pInstance = new CVIBuffer_Line(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(routes)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Line::Clone(void* pArg)
{
	CVIBuffer_Line* pInstance = new CVIBuffer_Line(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Line::Free()
{
	__super::Free();
}
