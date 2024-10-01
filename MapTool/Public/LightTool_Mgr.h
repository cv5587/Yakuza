#pragma once

#include "Base.h"
#include "LightObj.h"
#include "Client_Defines.h"

class CLightTool_Mgr final : public CBase
{
	DECLARE_SINGLETON(CLightTool_Mgr);

private:
	CLightTool_Mgr();
	virtual ~CLightTool_Mgr() = default;

public:
	HRESULT Initialize();
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

public:
	void							Show_Add_Light_IMGUI();
	void							Show_FileName();
	void							Set_LightObj_Render(_bool bRender) { m_bRender = bRender; }

private:
	void							Save_Lights_InFile(int iSaveNum); /* 현재 map에 놓여진 light 저장*/
	void							Load_Lights_File(int iNum); /* light 로드하기 */

	HRESULT							Import_Bin_Light_Data_OnTool(LIGHT_DESC_IO* lightObjData, char* fileName);
	HRESULT							Export_Bin_Light_Data(LIGHT_DESC_IO* lightObjData, int iSaveNum);


	void							Update_FileName();

	void							Select_LightDesc();
	bool							Add_Light_Imgui(LIGHT_DESC	lightAddDesc);
	void							Delete_Light();
	void							Delete_AllLights();


	void							Show_Installed_LightsList();

private:
	void							Update_LightsName();
	void							Edit_Light();

	void							Initialize_Light_Environment();
	void							Edit_GameObject_Transform(int iNumObject);
	void							Edit_Installed_Light();
	void							Click_To_Select_Light(int& iObjectNum);


private:
	CGameInstance*					m_pGameInstance;

private:
	vector<char*>					m_FileNames;
	vector<CLightObj*>				m_LightObjects;
	vector<char*>					m_LightsName;

private:
	_int							m_iCurrentLightIndex = { 0 };
	_int							m_iCurrentLightFile = { 0 };
	LIGHT_DESC						m_tCurLightDesc;

	bool							m_bUpdateInfrom = { false };

	bool							m_bLightAdd = { false };
	bool							m_bRender = { false };
	bool							m_bFindObject = { false };

public:
	virtual void Free() override;
};

