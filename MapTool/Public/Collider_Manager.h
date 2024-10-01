#pragma once

#include "Base.h"
#include "ColliderObj.h"
#include "Client_Defines.h"

class CImgui_Manager;

class CCollider_Manager : public CBase
{
	DECLARE_SINGLETON(CCollider_Manager);

private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT											Initialize();
	void											Priority_Tick(_float fTimeDelta);
	void											Tick(_float fTimeDelta);
	void											Late_Tick(_float fTimeDelta);
	void											Render();

public:
	void											Show_FileName();
	void											Add_Collider_IMGUI();

private:
	void											Save_Collider_InFile(int iSaveNum); 
	void											Load_Collider_File(int iNum); 

	HRESULT											Import_Bin_Collider_Data_OnTool(COLLIDER_IO* ColliderData, char* fileName);
	HRESULT											Export_Bin_Collider_Data(COLLIDER_IO* ColliderData, int iSaveNum);

	bool											Add_Collider_In_Manager();
	void											Update_FileName();
	void											Load_Colliders_File(int iNum);
	void											Delete_Collider(int iNumCollider); /* 선택 object 삭제 */
	void											Delete_AllCollider();
	void											Update_ColliderNameList();
	void											Edit_Collider_Transform(int iColliderNum);
	void											Click_To_Select_Object(int& iObjectNum);


private:
	vector<CColliderObj*>							m_pColliders;
	_matrix											m_WorldMatrix;
	vector<char*>									m_FileNames;
	vector<char*>									m_ColliderNames;

private:
	CGameInstance*									m_pGameInstance;
	CImgui_Manager*									m_pImguiMgr;

private:
	_int											m_iCurentColliderIndex = { 0 };
	bool											m_bColliderAdd = { false };
	bool											m_bFindObject;


public:
	virtual void									Free() override;
};

