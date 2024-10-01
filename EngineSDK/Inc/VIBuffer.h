#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer :
    public CComponent
{
protected:
    CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer(const CVIBuffer& rhs);
    virtual ~CVIBuffer() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Render();
    virtual HRESULT Render_Compute();

public:
    HRESULT Create_Buffer(ID3D11Buffer** pOut);
    _uint Get_NumVertex() { return m_iNumVertices; }
    _uint Get_NumIndices() { return m_iNumIndices; }

    HRESULT Bind_Compute(class CComputeShader* pShader);
    HRESULT Bind_Compute_AABBCube(class CComputeShader* pShader);

protected:
    ID3D11ShaderResourceView* m_pVertexBufferSRV = { nullptr };
    ID3D11UnorderedAccessView* m_pResultBufferUAV = { nullptr };
    ID3D11Buffer* m_pProcessedVertexBuffer = { nullptr };

    ID3D11Buffer* m_pVB;
    ID3D11Buffer* m_pSRVIn;
    ID3D11Buffer* m_pUAVOut;
    ID3D11Buffer* m_pIB;

protected:
    D3D11_BUFFER_DESC m_Buffer_Desc = {};
    D3D11_SUBRESOURCE_DATA m_InitialData = {};

    _uint m_iNumVertexBuffers = { 0 };
    _uint m_iVertexStride = { 0 };
    _uint m_iNumVertices = { 0 };

    _uint m_iIndexStride = { 0 };
    _uint m_iNumIndices = { 0 };

    DXGI_FORMAT m_GIFormat = {};
    D3D11_PRIMITIVE_TOPOLOGY m_Primitive_Topology = {};

protected:
    HRESULT Ready_BoneBuffer();
    HRESULT Ready_AABBCubeBuffer();

public:
    virtual void Free() override;
};
END
