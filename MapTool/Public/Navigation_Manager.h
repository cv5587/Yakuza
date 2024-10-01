#pragma once

#include "Base.h"
#include "Cell.h"
#include "Navigation.h"
#include "Shader.h"
#include "Client_Defines.h"
#include "VIBuffer_Line.h"
#include "NaviObj.h"

BEGIN(Engine)
class CGameInstance;
class CVIBuffer_Line;
END

class CNavigation_Manager final : public CBase
{
	DECLARE_SINGLETON(CNavigation_Manager);

private:
	CNavigation_Manager();
	virtual ~CNavigation_Manager() = default;

public:
	HRESULT					Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void					Priority_Tick(_float fTimeDelta);
	void					Tick(_float fTimeDelta);
	void					Late_Tick(_float fTimeDelta);
	HRESULT					Render();

public:
	TCHAR*					StringToTCHAR(string& s);

public:
	void					Load_Cell_IMGUI();
	void					Make_Point(_vector vPickingPos);
	HRESULT					Make_Cell();

	HRESULT					Save_Cells(_uint iIndex);
	HRESULT					Load_Cells(_uint iIndex);

	HRESULT					Load_Route(_uint iIndex); // route안의 cell 목록을 update

	void					Show_FileName();
	void					Update_FileName();
	void					Update_RouteFileName();

	void					Delete_Cell(_uint iIndex);
	void					Delete_AllCell();

	void					Delete_RouteCell(_int& iIndex);
	void					Delete_AllRouteCell(_int& iIndex);

	int						Get_Player_Monster_NaviIndex(_vector vPosition);
				
	/* cell 수정하는 부분 */
	void					Find_Cells();

	void					Make_Route();
	void					Add_Route_In_Navi();



	/* For IMGUI_MANAGER */
public:
	void					Show_Cells_IMGUI();

private:
	_float3					Find_ClosestPoint(_vector pickPos, _float* pMinDistance);



private:
	void					Update_CellsName();
	void					Update_RouteName();
	void					Update_IndexesName();
	void					Update_PointRouteNumName();


	void					Update_Routes(); // map 수정


	void					Delete_Route(_int& iIndex);


	// Line 그리기 위한 정보 업데이트
	void					Update_CellIndex_Draw();
	void					Update_Route_Draw();

	void							Edit_GameObject_Transform(int iNumObject);
	void							Click_To_Select_Object(int& iObjectNum);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CShader* m_pShader = nullptr;
	class CShader* m_pShaderLineCom = nullptr;

	class CVIBuffer_Line* m_pVIBufferCom = { nullptr };
	vector<class CVIBuffer_Line*>	m_vLineBufferLine;

	class CGameInstance* m_pGameInstance = { nullptr };

private:
	vector<char*>							m_FileNames;
	vector<char*>							m_RouteFileNames;

private:
	vector<class CCell*>					m_Cells;

	//vector<ROUTE_IO>						m_Route_CellIndexes; // 루트관련 index

	//map<int, vector<ROUTE_IO>>				m_Routes; // 현재 네비가 가지고 있는 루트와 그에 대한 cell index


	vector<class CNaviObj*>					m_Route_CellIndexes; // 루트관련 index

	map<int, vector<class CNaviObj*>>		m_Routes; // 현재 네비가 가지고 있는 루트와 그에 대한 cell index


	//Line 그리는거 때문에
	vector<ROUTE_IO>						m_Route_CellIndexes_Draw; 
	map<int, vector<ROUTE_IO>>				m_Routes_Draw; 



	static _float4x4						m_WorldMatrix; /* 객체들이 공유할 수 있게끔 static */

private:
	vector<char*>							m_CellsName;
	vector<char*>							m_RouteName; // 네비가 가지고 있는 루트들의 이름
	vector<char*>							m_IndexesName; // 루트 안에 있는 cell index 이름들
	vector<char*>							m_PointRouteSaveNumName; // 루트 안에 있는 cell index 이름들

private:
	vector<_float3>							m_vPoints;

	/* For IMGUI_MANAGER */
private:
	_int									m_iCurrentCellIndex = { 0 };
	_int									m_iCurrentFileNavi = { 0 };
	_int									m_iCurrentOption = { 0 };
	_int									m_iPointOption = { 0 };

	_int									m_iCurrentRouteCellIndex = { 0 };
	_int									m_iCurrentRouteNum = { 0 }; // route의 번호

	bool									m_bMakeRoute_IMGUI = { false };
	bool									m_bMakeRoute_Picking = { false };


	bool									m_bRouteAllView = { false };

	bool									m_bFindObject = { false };

public:
	virtual void Free() override;
};

