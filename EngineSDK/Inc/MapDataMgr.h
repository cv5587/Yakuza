#pragma once

#include "Base.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMapDataMgr final : public CBase
{
	DECLARE_SINGLETON(CMapDataMgr);

public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* 그냥 건축물 */
		NONINTERACT_OBJ,
		MONSTER,
		OBJ_END
	};

private:
	CMapDataMgr();
	virtual ~CMapDataMgr() = default;

public:
	HRESULT			Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void			Priority_Tick(_float fTimeDelta);
	void			Tick(_float fTimeDelta);
	void			Late_Tick(_float fTimeDelta);
	void			Render();

public:
	HRESULT			Export_Bin_Map_Data(MAP_TOTALINFORM_DESC* mapObjData);
	HRESULT			Import_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel);
	HRESULT			Import_Bin_Map_Data_OnTool(MAP_TOTALINFORM_DESC* mapObjData, char* fileName);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END