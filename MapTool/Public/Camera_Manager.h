#pragma once

#include "Base.h"
#include "CameraObj.h"
#include "CameraInstallObj.h"
#include "CineCamera.h"
#include "Client_Defines.h"

class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager);

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT											Initialize();
	void											Priority_Tick(_float fTimeDelta);
	void											Tick(_float fTimeDelta);
	void											Late_Tick(_float fTimeDelta);
	void											Render();

public:
	void											Show_FileName();
	void											Install_Camera_IMGUI();

private:
	void											Update_FileName();
	void											Update_FocusName();
	void											Update_EyeName();
	void											Update_CameraName();

	bool											Add_Camera_Eye_Imgui();
	bool											Add_Camera_Focus_Imgui();

	void											Add_CameraObj(CAMERAOBJ_DESC cameraobjDesc);
	void											Edit_CameraObj();

	void											Delete_Camera();
	void											Delete_AllCamera();

	void											Show_Edit_Cameras_Inform_IMGUI();
	void											Edit_Camera();

	void											Edit_Focus_Transform();
	void											Edit_Eye_Transform();

	void											Test_Camera(int iCamNum);

	/* 저장관련 */
private:
	void											Save_Cameras_InFile(int iSaveNum, bool bFirstLerp); 
	void											Load_Cameras_File(int iNum);

	HRESULT											Import_Bin_Cam_Data_OnTool(CAMERAOBJ_IO* camData, char* fileName);
	HRESULT											Export_Bin_Cam_Data(CAMERAOBJ_IO* camData, int iSaveNum);

private:
	CGameInstance*									m_pGameInstance;
	CCineCamera*									m_pCineCamera = { nullptr };

private:
	vector<char*>									m_FileNames;
	vector<char*>									m_CameraFocusNames;
	vector<char*>									m_CameraEyeNames;
	vector<char*>									m_CameraName;

	vector<CCameraObj*>								m_CameraObjs; /* Focus와 Eye를 합쳐서 Desc를 만들고 여기에 저장 */
	vector<CCameraInstallObj*>						m_CameraFocusObjs;
	vector<CCameraInstallObj*>						m_CameraEyeObjs;


private:
	_int											m_iCurrentCameraIndex = { 0 };
	_int											m_iCurrentCameraFile = { 0 };
	CAMERAOBJ_DESC									m_tCurCameraDesc;

	_bool											m_bEyeAdd = { false };
	_bool											m_bFocusAdd = { false };
	_bool											m_bCameraAdd = { false };

	_bool											m_bCameraTest = { false };
	_bool											m_bFirstLerp = { false };


public:
	virtual void									Free() override;
};

