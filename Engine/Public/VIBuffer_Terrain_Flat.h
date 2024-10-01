#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CVIBuffer_Terrain_Flat final : public CVIBuffer
{
public:
	typedef struct TERRAIN_SIZEDESC
	{
		_float iNumVerticesX;
		_float iNumVerticesZ;
	}TERRAIN_SIZEDESC;

private:
	CVIBuffer_Terrain_Flat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain_Flat(const CVIBuffer_Terrain_Flat& rhs);
	virtual ~CVIBuffer_Terrain_Flat() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT			Change_LandScale(_uint iNumVerticesX, _uint iNumVerticesZ);

private:
	_uint				m_iNumVerticesX = { 0 };
	_uint				m_iNumVerticesZ = { 0 };


public:
	static CVIBuffer_Terrain_Flat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END