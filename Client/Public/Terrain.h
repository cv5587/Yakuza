#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CNavigation;
class CVIBuffer_Terrain;
class CVIBuffer_Terrain_Flat;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	typedef struct TERRAIN_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		XMFLOAT2	vPlaneSize;
		int			iLevel;
	}TERRAIN_DESC;
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void							Change_LandScale(_uint iNumX, _uint iNumZ);
	_float2							Get_TerrainScale() { return m_vTerrainScale; }

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CNavigation*					m_pNavigationCom = { nullptr };
	//CVIBuffer_Terrain*			m_pVIBufferCom = { nullptr };
	CVIBuffer_Terrain_Flat*			m_pVIBufferCom = { nullptr };

private:
	_float4x4						m_ViewMatrix, m_ProjMatrix;

private:
	_float2							m_vTerrainScale = { 0,0 };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END