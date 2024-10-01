#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		VIBUFFER_TRAIL_DESC* pDesc = static_cast<VIBUFFER_TRAIL_DESC*>(pArg);
		m_iMaxTrail = pDesc->iMaxTrail;
		m_vInitPosA = pDesc->vInitPosA;
		m_vInitPosB = pDesc->vInitPosB;
	}

	m_GIFormat = DXGI_FORMAT_R32_UINT;
	m_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumVertexBuffers = 1;
	m_iCurrentIndices = 0;
	m_iNumVertices = 4 + (m_iMaxTrail - 1) * 2;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_Buffer_Desc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertexts = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertexts, sizeof(VTXPOSTEX) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		pVertexts[i].vPosition = _float3{ 0.f, 0.f, 0.f };
		pVertexts[i].vTexcoord = _float2(-1.f, -1.f);
	}
	// texUV
		// 0 2 4 6 8 10
		// 1 3 5 7 9 11

	m_InitialData.pSysMem = pVertexts;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertexts);

	m_iNumIndices = m_iMaxTrail * 6;//m_iNumVertices * 2 - 2;
	m_iIndexStride = 4;

	m_Buffer_Desc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_Buffer_Desc.MiscFlags = 0;
	m_Buffer_Desc.StructureByteStride = sizeof(_ulong);

	INDEX32* pIndices = new INDEX32[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(INDEX32) * m_iNumIndices);
	
	for (_uint i = 0; i < m_iNumIndices; i += 2)
	{
		pIndices[i]._0 = i + 3;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i;

		pIndices[i + 1]._0 = i + 2;
		pIndices[i + 1]._1 = i + 3;
		pIndices[i + 1]._2 = i ;

		/*pIndices[i]._0 = i ;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i + 2;

		pIndices[i + 1]._0 = i;
		pIndices[i + 1]._1 = i + 2;
		pIndices[i + 1]._2 = i + 3;*/
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
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

	m_pContext->DrawIndexed(m_iCurrentIndices, 0, 0);

	return S_OK;
}

void CVIBuffer_Trail::Add_Trail(const _float& fTimeDelta, const _matrix WorldMatrix)
{

		/*VTXPOSTEX* pVertex;
	_float3 vNewPos[2];

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* vResult = ((VTXPOSTEX*)SubResource.pData);

	XMStoreFloat3(&vNewPos[0], XMVector3TransformCoord(XMLoadFloat3(&m_vInitPosA), WorldMatrix));
	XMStoreFloat3(&vNewPos[1], XMVector3TransformCoord(XMLoadFloat3(&m_vInitPosB), WorldMatrix));

	if (m_iNumVertices <= m_TrailInfos.size() * 2)
		m_TrailInfos.erase(m_TrailInfos.begin());

	TRAIL_INFO Desc{};
	Desc.vPos[0] = vNewPos[0];
	Desc.vPos[1] = vNewPos[1];
	m_TrailInfos.push_back(Desc);

	auto tTrailInfo = m_TrailInfos.begin();

	_float vTexcoordX = m_TrailInfos.size() * 2;
	for (size_t i = 0; i < m_TrailInfos.size() * 2;)
	{

			vResult[i].vPosition = (*tTrailInfo).vPos[0];
			vResult[i].vTexcoord = _float2(1.f - (i / (vTexcoordX - 2.f)), 0.f);
			++i;

			vResult[i].vPosition = (*tTrailInfo).vPos[1];
			vResult[i].vTexcoord = _float2(1.f - ((i ) / (vTexcoordX - 2.f)), 1.f);
			++i;


		++tTrailInfo;
	}

	m_pContext->Unmap(m_pVB, 0);

	m_iCurrentIndices = (m_TrailInfos.size() - 1) * 2 * 3;
*/

	VTXPOSTEX* pVertex;
	_float3 vNewPos[2];

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* vResult = ((VTXPOSTEX*)SubResource.pData);

	XMStoreFloat3(&vNewPos[0], XMVector3TransformCoord(XMLoadFloat3(&m_vInitPosA), WorldMatrix));
	XMStoreFloat3(&vNewPos[1], XMVector3TransformCoord(XMLoadFloat3(&m_vInitPosB), WorldMatrix));

	if (m_iNumVertices <= m_TrailInfos.size() * 2)
		m_TrailInfos.erase(m_TrailInfos.begin());

	TRAIL_INFO Desc{};
	Desc.vPos[0] = vNewPos[0];
	Desc.vPos[1] = vNewPos[1];
	m_TrailInfos.push_back(Desc);

	auto tTrailInfo = m_TrailInfos.begin();

	_float vTexcoordX = m_TrailInfos.size();

	_uint trailIndex = 0;
	for (size_t i = 0; i < m_TrailInfos.size()*2;)	
	{

			_float fWeight = _float(trailIndex) / m_TrailInfos.size();//트레일 웨이트 [점2개 마다 보간]

			_float3 p0 = (trailIndex == 0) ? m_TrailInfos[trailIndex].vPos[0] : m_TrailInfos[trailIndex - 1].vPos[0];
			_float3 p1 = m_TrailInfos[trailIndex].vPos[0];
			_float3 p2 = (trailIndex + 1 < m_TrailInfos.size()) ? m_TrailInfos[trailIndex + 1].vPos[0] : m_TrailInfos[trailIndex].vPos[0];
			_float3 p3 = (trailIndex + 2 < m_TrailInfos.size()) ? m_TrailInfos[trailIndex + 2].vPos[0] : (trailIndex + 1 < m_TrailInfos.size()) ? m_TrailInfos[trailIndex + 1].vPos[0] : m_TrailInfos[trailIndex].vPos[0];

			XMStoreFloat3(&vResult[i].vPosition, XMVectorCatmullRom(XMLoadFloat3(&p0), XMLoadFloat3(&p1), XMLoadFloat3(&p2), XMLoadFloat3(&p3), fWeight));
			vResult[i].vTexcoord = _float2(1.f== vTexcoordX ? 0.f :1.f - (trailIndex / (vTexcoordX - 1.f)), 0.f);



			p0 = (trailIndex == 0) ? m_TrailInfos[trailIndex].vPos[1] : m_TrailInfos[trailIndex - 1].vPos[1];
			p1 = m_TrailInfos[trailIndex].vPos[1];
			p2 = (trailIndex + 1 < m_TrailInfos.size()) ? m_TrailInfos[trailIndex + 1].vPos[1] : m_TrailInfos[trailIndex].vPos[1];
			p3 = (trailIndex + 2 < m_TrailInfos.size()) ? m_TrailInfos[trailIndex + 2].vPos[1] : (trailIndex + 1 < m_TrailInfos.size()) ? m_TrailInfos[trailIndex + 1].vPos[1] : m_TrailInfos[trailIndex].vPos[1];

			XMStoreFloat3(&vResult[i + 1].vPosition, XMVectorCatmullRom(XMLoadFloat3(&p0), XMLoadFloat3(&p1), XMLoadFloat3(&p2), XMLoadFloat3(&p3), fWeight));
			vResult[i+1].vTexcoord = _float2(1.f == vTexcoordX ? 0.f : 1.f - ((trailIndex + 1) / (vTexcoordX - 1.f)), 1.f);
			i += 2;
			++trailIndex;
	}

	m_pContext->Unmap(m_pVB, 0);

	m_iCurrentIndices = (m_TrailInfos.size() - 1) * 2 * 3;
}

void CVIBuffer_Trail::Reset_Trail()
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* vResult = ((VTXPOSTEX*)SubResource.pData);

	for (size_t i = 0; i < m_iNumVertices;)
	{
  		vResult[i].vPosition = _float3{ 0.f, 0.f, 0.f };
		vResult[i].vTexcoord = _float2(0.f, 0.f);
		++i;

		vResult[i].vPosition = _float3{ 0.f, 0.f, 0.f };
		vResult[i].vTexcoord = _float2(0.f, 0.f);
		++i;
	}

	m_pContext->Unmap(m_pVB, 0);

	m_TrailInfos.clear();

	m_iCurrentIndices =0;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}
