#pragma once

#include "Base.h"
#include "TriggerObj.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

class CTrigger_Manager : public CBase
{
public:
	enum TRIGGER_TYPE {
		TRIGGER_MOVE_LEVEL,
		TRIGGER_CINEMACHINE,
		TRIGGER_YONEDA,
		TRIGGER_MONSTER,
		TRIGGER_QUEST,
		TRIGGER_END
	};

	DECLARE_SINGLETON(CTrigger_Manager);

private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;


public:
	HRESULT								Initialize();
	void								Priority_Tick(_float fTimeDelta);
	void								Tick(_float fTimeDelta);
	void								Late_Tick(_float fTimeDelta);
	void								Render();

public:
	void								Set_TriggerObj_Render(_bool bRender) { m_bRender = bRender; }
	void								Show_FileName();
	void								Show_Add_Trigger_IMGUI();


private:
	void								Save_Triggers_InFile(int iSaveNum); /* 현재 map에 놓여진 light 저장*/
	void								Load_Triggers_File(int iNum); /* light 로드하기 */

	HRESULT								Import_Bin_Triggers_Data_OnTool(TRIGGER_IO* triggerIOData, char* fileName);
	HRESULT								Export_Bin_Triggers_Data(TRIGGER_IO* triggerIOData, int iSaveNum);

	void								Update_FileName();
	void								Update_TriggerObjName();

	void								Edit_GameObject_Transform(int iNumObject);
	void								Edit_Installed_Trigger();
	void								Click_To_Select_Trigger(int& iObjectNum);

	void								Delete_Trigger();
	void								Delete_AllTrigger();

private:
	CGameInstance*						m_pGameInstance;

private:
	vector<const char*>					m_Levels = { "LEVEL_STATIC", "LEVEL_LOADING", "LEVEL_LOGO", "LEVEL_OFFICE_1F", "LEVEL_OFFICE_2F", "LEVEL_OFFICE_BOSS", "LEVEL_DOGIMAZO", "LEVEL_DOGIMAZO_STAIRS", "LEVEL_DOGIMAZO_LOBBY", "LEVEL_DOGIMAZO_BOSS", "LEVEL_TEST", "LEVEL_END" };
	vector<const char*>					m_TriggerLevel = { "MONSTER_TRIGGER_0", "MONSTER_TRIGGER_1", "MONSTER_TRIGGER_END" };
	vector<char*>						m_FileNames;
	vector<char*>						m_TriggerNames;
	vector<CTriggerObj*>				m_TriggerObjects;

private:
	_int								m_iCurrentTriggerIndex = { 0 };
	_int								m_iCurrentTriggerFile = { 0 };
	TRIGGER_DESC						m_tCurTriggerDesc;

	bool								m_bRender = { false };
	bool								m_bAddTrigger = { false };
	bool								m_bFindObject = { false };

public:
	virtual void Free() override;
};

