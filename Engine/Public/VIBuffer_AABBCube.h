#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// Occulusion culling을 위해 임의로 그릴 buffer

class ENGINE_DLL CVIBuffer_AABBCube final : public CVIBuffer
{
public:
    typedef struct tVIBufferAABBCube
    {
		XMFLOAT3 vScale;
    }AABBCUBE_DESC;

private:
	CVIBuffer_AABBCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_AABBCube(const CVIBuffer_AABBCube& rhs);
	virtual ~CVIBuffer_AABBCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

	void	Copy_ResultResources(ID3D11Buffer* pBuffer);

public:
	static CVIBuffer_AABBCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END