#pragma once

#include "Base.h"
#include "Terrain.h"
#include "Client_Defines.h"

// IMGUI에서 사용할 아이
// TERRAIN에 대한 정보

class CTerrain_Manager final : public CBase
{
	DECLARE_SINGLETON(CTerrain_Manager);

private:
	CTerrain_Manager();
	virtual ~CTerrain_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

public:
	void			Set_Terrain(CTerrain* terrain) { m_pTerrain = terrain; }

public:
	void			Change_LandScale(_uint iNumX, _uint iNumZ);
	_float2			Get_LandScale() { return _float2(m_iNumVerticesX, m_iNumVerticesZ); }

private:
	CTerrain*					m_pTerrain;
	_uint						m_iNumVerticesX = 100;
	_uint						m_iNumVerticesZ = 100;
	_uint						m_iNumLandMesh = 0; // 지금 선택된 landmesh 종류, 추가할때 사용

public:
	virtual void Free() override;
};

