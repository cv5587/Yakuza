#include "Terrain_Manager.h"
#include "GameInstance.h"


IMPLEMENT_SINGLETON(CTerrain_Manager)

CTerrain_Manager::CTerrain_Manager()
{
}

HRESULT CTerrain_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return S_OK;
}

void CTerrain_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CTerrain_Manager::Tick(_float fTimeDelta)
{
}

void CTerrain_Manager::Late_Tick(_float fTimeDelta)
{
}

void CTerrain_Manager::Render()
{
}

void CTerrain_Manager::Change_LandScale(_uint iNumX, _uint iNumZ)
{
	m_iNumVerticesX = iNumX;
	m_iNumVerticesZ = iNumZ;
	m_pTerrain->Change_LandScale(iNumX, iNumZ);
}

void CTerrain_Manager::Free()
{
	//Safe_Release(m_pTerrain);
}
