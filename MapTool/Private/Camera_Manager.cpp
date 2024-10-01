#include "Camera_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"
#include "SystemManager.h"

#include <iostream>
#include <io.h>

bool useWindowF = false;
int gizmoCountF = 1;
static ImGuizmo::OPERATION mCurrentGizmoOperationF(ImGuizmo::TRANSLATE);

// TRANSFORM의 MATRIX
void EditTransformF(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	editTransformDecomposition = true;
	if (editTransformDecomposition)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_T))
			mCurrentGizmoOperationF = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperationF = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperationF = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperationF == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperationF = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperationF == ImGuizmo::ROTATE))
			mCurrentGizmoOperationF = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperationF == ImGuizmo::SCALE))
			mCurrentGizmoOperationF = ImGuizmo::SCALE;
		/* if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			 mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperationF != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_S))
			useSnap = !useSnap;
		ImGui::Checkbox("##UseSnap", &useSnap);
		ImGui::SameLine();

		switch (mCurrentGizmoOperationF)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}
		ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;
	if (useWindowF)
	{
		ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
		ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
		viewManipulateTop = ImGui::GetWindowPos().y;
		//ImGuiWindow* window = ImGui::GetCurrentWindow();
		//gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
	}
	else
	{
		// 이거만 신경쓰기 얼마만큼 기즈모 화면값
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	//격자
	//ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
	// 큐브 그려주는애
	//ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);

	// 옮기는애
	// 객체 잡고 동그라미좌표계 그려주는애
	// 객체 잡고 동그라미좌표계 그려주는애
	// matrix : 객체월드행ㄹ려
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperationF, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	// 화면 상단 카메라 돌아가는애
	//ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

	if (useWindowF)
	{
		ImGui::End();
		// 이거해보기 터지면
		ImGui::PopStyleColor(1);
	}
}


bool useWindowE = false;
int gizmoCountE = 1;
static ImGuizmo::OPERATION mCurrentGizmoOperationE(ImGuizmo::TRANSLATE);

// TRANSFORM의 MATRIX
void EditTransformE(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	editTransformDecomposition = true;
	if (editTransformDecomposition)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_T))
			mCurrentGizmoOperationE = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperationE = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperationE = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperationE == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperationE = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperationE == ImGuizmo::ROTATE))
			mCurrentGizmoOperationE = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperationE == ImGuizmo::SCALE))
			mCurrentGizmoOperationE = ImGuizmo::SCALE;
		/* if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			 mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperationE != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_S))
			useSnap = !useSnap;
		ImGui::Checkbox("##UseSnap", &useSnap);
		ImGui::SameLine();

		switch (mCurrentGizmoOperationE)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}
		ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;
	if (useWindowE)
	{
		ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
		ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
		viewManipulateTop = ImGui::GetWindowPos().y;
		//ImGuiWindow* window = ImGui::GetCurrentWindow();
		//gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
	}
	else
	{
		// 이거만 신경쓰기 얼마만큼 기즈모 화면값
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	//격자
	//ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
	// 큐브 그려주는애
	//ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);

	// 옮기는애
	// 객체 잡고 동그라미좌표계 그려주는애
	// 객체 잡고 동그라미좌표계 그려주는애
	// matrix : 객체월드행ㄹ려
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperationE, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	// 화면 상단 카메라 돌아가는애
	//ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

	if (useWindowE)
	{
		ImGui::End();
		// 이거해보기 터지면
		ImGui::PopStyleColor(1);
	}
}

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager::Initialize()
{
	return S_OK;
}

void CCamera_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CCamera_Manager::Tick(_float fTimeDelta)
{
	if (nullptr != m_pCineCamera && true == m_bCameraTest)
	{
		m_pCineCamera->Tick(fTimeDelta);
	}
}

void CCamera_Manager::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_CameraFocusObjs)
		iter->Late_Tick(fTimeDelta);

	for (auto& iter : m_CameraEyeObjs)
		iter->Late_Tick(fTimeDelta);

	if (nullptr != m_pCineCamera && true == m_bCameraTest)
	{
		m_pCineCamera->Late_Tick(fTimeDelta);
	}
}

void CCamera_Manager::Render()
{
}

void CCamera_Manager::Show_FileName()
{
	ImGui::Begin(u8" 카메라 파일");

	ImGui::Text(u8" 카메라 리스트 ");
	ImGui::Text(u8" 저장 : 숫자 입력 후 저장 ");
	ImGui::Text(u8" 로드 : listbox에서 선택한 후 로드 버튼 누르기 ");

	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	static int layer_current_idx;
	if (ImGui::BeginListBox(u8"카메라 파일들"))
	{
		for (int n = 0; n < m_FileNames.size(); n++)
		{
			const bool is_selected = (layer_current_idx == n);
			if (ImGui::Selectable(m_FileNames[n], is_selected))
				layer_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	static	bool	bFirstLerp = false;
	bFirstLerp = m_bFirstLerp;
	ImGui::Checkbox(u8"처음선형보간", &bFirstLerp);
	m_bFirstLerp = bFirstLerp;

	static int iSaveNum;
	ImGui::InputInt(u8"저장번호 : ", &iSaveNum);

	if (ImGui::Button(u8"카메라 저장"))
	{
		Save_Cameras_InFile(iSaveNum, m_bFirstLerp);
		Update_FileName();
	}

	static int iTestNum;
	ImGui::InputInt(u8"테스트번호 : ", &iTestNum);
	ImGui::SameLine();
	if (ImGui::Button(u8"카메라 테스트"))
	{
		Test_Camera(iTestNum);
		m_bCameraTest = true;
	}

	if (ImGui::Button(u8"카메라 파일 로드") && m_FileNames.size() > 0)
	{
		Load_Cameras_File(layer_current_idx);
	}

	ImGui::End();
}

void CCamera_Manager::Install_Camera_IMGUI()
{
	ImGui::Text(u8" Lerp : 다음 카메라까지 이동할때 선형보간 할지 ");
	ImGui::Text(u8" 마지막 카메라 Lerp : 플레이어 카메라까지 돌아갈때 Lerp할지 ");
	ImGui::Text(u8" 마지막 카메라 MoveTime : 플레이어 카메라까지 돌아갈때의 속도 ");
	ImGui::Text(u8" Eye - 빨강, Focus - 파랑 ");



	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	ImVec2 size = ImVec2(200, 100);  // 너비: 200, 높이: 100

	static int eye_current_idx = 0;
	if (ImGui::BeginListBox(u8"Eye ", size))
	{
		for (int n = 0; n < m_CameraEyeNames.size(); n++)
		{
			const bool is_selected = (eye_current_idx == n);
			if (ImGui::Selectable(m_CameraEyeNames[n], is_selected))
				eye_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	ImGui::SameLine();

	static int focus_current_idx = 0;
	if (ImGui::BeginListBox(u8"Focus " , size))
	{
		for (int n = 0; n < m_CameraFocusNames.size(); n++)
		{
			const bool is_selected = (focus_current_idx == n);
			if (ImGui::Selectable(m_CameraFocusNames[n], is_selected))
				focus_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button(u8"Eye 추가"))
	{
		m_bEyeAdd = true;
	}


	// 동일한 줄에 빈 공간 추가
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(160.0f, 0.0f));  // 가로로 20.0f 만큼 빈 공간 추가
	ImGui::SameLine();

	if (ImGui::Button(u8"Foucs 추가"))
	{
		m_bFocusAdd = true;
	}

	static CAMERAOBJ_DESC		cameraObjDesc{};

	static float	fFovY = XMConvertToRadians(60.0f);
	static float	fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	static float	fNear = 0.1f;
	static float	fFar = 3000.f;

	if (ImGui::BeginTable("StructTable", 10)) {
		ImGui::TableNextColumn();
		ImGui::Text("fFovY");
		ImGui::TableNextColumn();
		ImGui::InputFloat("##fFovY", &fFovY); // "##"를 사용하여 ID 레이블을 숨깁니다.

		ImGui::TableNextColumn();
		ImGui::Text("Aspect");
		ImGui::TableNextColumn();
		ImGui::InputFloat("##fAspect", &fAspect);

		ImGui::TableNextColumn();
		ImGui::Text("fNear");
		ImGui::TableNextColumn();
		ImGui::InputFloat("##Near", &fNear);

		ImGui::TableNextColumn();
		ImGui::Text("fFar");
		ImGui::TableNextColumn();
		ImGui::InputFloat("##Far", &fFar);

		ImGui::TableNextColumn();
		ImGui::Text("fMoveTime");
		ImGui::TableNextColumn();
		ImGui::InputFloat("##fMoveTime", &cameraObjDesc.fMoveTime);

		ImGui::TableNextColumn();
		ImGui::Text("fMoveSpeed");
		ImGui::TableNextColumn();
		ImGui::InputFloat("##fMoveSpeed", &cameraObjDesc.fMoveSpeed);

		ImGui::TableNextColumn();
		ImGui::Text("fStayTime");
		ImGui::TableNextColumn();
		ImGui::InputFloat("##fStayTime", &cameraObjDesc.fStayTime);

		ImGui::TableNextColumn();
		ImGui::Text("Lerp");
		ImGui::TableNextColumn();	ImGui::Checkbox(u8"##선형보간할건지", &cameraObjDesc.bLerp);

		ImGui::EndTable();
	}

	cameraObjDesc.fFovY = fFovY;
	cameraObjDesc.fAspect = fAspect;
	cameraObjDesc.fNear = fNear;
	cameraObjDesc.fFar = fFar;

	if (ImGui::Button(u8"카메라 추가"))
	{
		m_bCameraAdd = true;
	}

	ImGui::NewLine();

	Add_Camera_Eye_Imgui();
	Add_Camera_Focus_Imgui();
	Add_CameraObj(cameraObjDesc);

	if (m_CameraObjs.size() > 0)
	{
		Show_Edit_Cameras_Inform_IMGUI();
	}

}

void CCamera_Manager::Update_FileName()
{
	for (auto& iter : m_FileNames)
		Safe_Delete(iter);

	m_FileNames.clear();
	string path = "../../Client/Bin/DataFiles/CameraData/*.dat";

	struct _finddata_t fd;
	intptr_t handle;

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
		return; // 파일없을때
	do
	{
		char* cfilename = new char[MAX_PATH];
		strcpy(cfilename, fd.name);
		m_FileNames.push_back(cfilename);
	} while (_findnext(handle, &fd) == 0);
	_findclose(handle);
}

void CCamera_Manager::Update_FocusName()
{
	if (0 <= m_CameraFocusObjs.size())
	{
		for (auto& iter : m_CameraFocusNames)
			Safe_Delete(iter);
		m_CameraFocusNames.clear();

	}

	if (0 < m_CameraFocusObjs.size())
	{
		for (int i = 0; i < m_CameraFocusObjs.size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "Focus");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_CameraFocusNames.push_back(szName);
		}
	}
}

void CCamera_Manager::Update_EyeName()
{
	if (0 <= m_CameraEyeObjs.size())
	{
		for (auto& iter : m_CameraEyeNames)
			Safe_Delete(iter);

		m_CameraEyeNames.clear();
	}

	if (0 < m_CameraEyeObjs.size())
	{
		for (int i = 0; i < m_CameraEyeObjs.size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "Eye");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_CameraEyeNames.push_back(szName);
		}
	}
}

void CCamera_Manager::Update_CameraName()
{
	if (0 <= m_CameraObjs.size())
	{
		for (auto& iter : m_CameraName)
			Safe_Delete(iter);

		m_CameraName.clear();
	}

	if (0 < m_CameraObjs.size())
	{
		for (int i = 0; i < m_CameraObjs.size(); i++)
		{
			char* szName = new char[MAX_PATH];
			strcpy(szName, "Camera");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_CameraName.push_back(szName);
		}
	}
}

bool CCamera_Manager::Add_Camera_Eye_Imgui()
{
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB) && m_bEyeAdd == true)
	{
		/* 처음추가될때*/
		//if (m_CameraEyeObjs.size() == 0)
		//	m_bUpdateInfrom = true;

		_matrix			startPos;
		startPos = XMMatrixIdentity();

		_bool		isPick;
		_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

		startPos = XMMatrixIdentity();
		startPos.r[3].m128_f32[0] = vTargetPos.m128_f32[0];
		startPos.r[3].m128_f32[1] = vTargetPos.m128_f32[1];
		startPos.r[3].m128_f32[2] = vTargetPos.m128_f32[2];
		startPos.r[3].m128_f32[3] = vTargetPos.m128_f32[3];

		CCameraInstallObj::CAMERAINSTALL_DESC		camInstallDesc;

		camInstallDesc.vStartPos = startPos;
		camInstallDesc.eInstallDesc = CCameraInstallObj::CAM_EYE;

		m_CameraEyeObjs.push_back(dynamic_cast<CCameraInstallObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_CameraInstallObj"), &camInstallDesc)));

		m_bEyeAdd = false;

		/* 추가되면 추가된 index로 이동 */
		//m_iCurrentLightIndex = m_LightObjects.size() - 1;

		Update_EyeName();

		return true;
	}

	return false;
}

bool CCamera_Manager::Add_Camera_Focus_Imgui()
{
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB) && m_bFocusAdd == true)
	{
		/* 처음추가될때*/
		//if (m_CameraEyeObjs.size() == 0)
		//	m_bUpdateInfrom = true;

		_matrix			startPos;
		startPos = XMMatrixIdentity();

		_bool		isPick;
		_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

		startPos = XMMatrixIdentity();
		startPos.r[3].m128_f32[0] = vTargetPos.m128_f32[0];
		startPos.r[3].m128_f32[1] = vTargetPos.m128_f32[1];
		startPos.r[3].m128_f32[2] = vTargetPos.m128_f32[2];
		startPos.r[3].m128_f32[3] = vTargetPos.m128_f32[3];

		CCameraInstallObj::CAMERAINSTALL_DESC		camInstallDesc;

		camInstallDesc.vStartPos = startPos;
		camInstallDesc.eInstallDesc = CCameraInstallObj::CAM_FOCUS;

		m_CameraFocusObjs.push_back(dynamic_cast<CCameraInstallObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_CameraInstallObj"), &camInstallDesc)));

		m_bFocusAdd = false;

		/* 추가되면 추가된 index로 이동 */
		//m_iCurrentLightIndex = m_LightObjects.size() - 1;

		Update_FocusName();

		return true;
	}

	return false;
}

void CCamera_Manager::Add_CameraObj(CAMERAOBJ_DESC cameraobjDesc)
{
	if (true == m_bCameraAdd)
	{
		m_bCameraAdd = false;

		int			iIndex = m_CameraObjs.size();

		/* focus, eye 위치 받아야해서 다 있을때 추가 가능 */
		if (iIndex >= m_CameraEyeObjs.size() || iIndex >= m_CameraFocusObjs.size())
		{
			m_bCameraAdd = true;
			return;
		}

		XMStoreFloat4(&cameraobjDesc.vFocus, m_CameraFocusObjs[iIndex]->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat4(&cameraobjDesc.vEye, m_CameraEyeObjs[iIndex]->Get_Transform()->Get_State(CTransform::STATE_POSITION));

		/* 처음 추가될때 */
		if (m_CameraObjs.size() == 0)
		{
			m_tCurCameraDesc = cameraobjDesc;
		}

		CCameraObj::CAMERAOBJINPUT_DESC		cameraInputDesc;
		cameraInputDesc.tObjDesc = cameraobjDesc;

		m_CameraObjs.push_back(dynamic_cast<CCameraObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_CameraObj"), &cameraInputDesc)));


		Update_CameraName();
	}
}

void CCamera_Manager::Edit_CameraObj()
{

}

void CCamera_Manager::Delete_Camera()
{
	vector<class CCameraObj*>::iterator	cameraiter = m_CameraObjs.begin();
	vector<class CCameraInstallObj*>::iterator	focusiter = m_CameraFocusObjs.begin();
	vector<class CCameraInstallObj*>::iterator	eyeiter = m_CameraEyeObjs.begin();

	for (int i = 0; i < m_iCurrentCameraIndex; i++)
	{
		cameraiter++;
		focusiter++;
		eyeiter++;
	}

	Safe_Release(*cameraiter);
	m_CameraObjs.erase(cameraiter);

	Safe_Release(*focusiter);
	m_CameraFocusObjs.erase(focusiter);

	Safe_Release(*eyeiter);
	m_CameraEyeObjs.erase(eyeiter);

	/* 인덱스 옮겨주기 */
	if (0 > m_iCurrentCameraIndex - 1)
	{
		m_iCurrentCameraIndex = 0;
	}
	else
	{
		m_iCurrentCameraIndex -= 1;
	}

	Update_CameraName();
	Update_EyeName();
	Update_FocusName();
}

void CCamera_Manager::Delete_AllCamera()
{
	m_iCurrentCameraIndex = 0;

	for (auto& iter : m_CameraObjs)
		Safe_Release(iter);
	m_CameraObjs.clear();

	for (auto& iter : m_CameraFocusObjs)
		Safe_Release(iter);
	m_CameraFocusObjs.clear();

	for (auto& iter : m_CameraEyeObjs)
		Safe_Release(iter);
	m_CameraEyeObjs.clear();

	Update_CameraName();
	Update_EyeName();
	Update_FocusName();

}

void CCamera_Manager::Show_Edit_Cameras_Inform_IMGUI()
{
	ImVec2 size = ImVec2(500, 100);  // 너비: 200, 높이: 100

	if (0 == m_CameraObjs.size())
		return;

	static int camera_current_idx = 0;

	if (m_iCurrentCameraIndex != camera_current_idx)
	{
		camera_current_idx = m_iCurrentCameraIndex;
		m_tCurCameraDesc = m_CameraObjs[m_iCurrentCameraIndex]->Get_CameraObjDesc();
	}


	if (ImGui::BeginListBox(u8"Camera ", size))
	{
		for (int n = 0; n < m_CameraName.size(); n++)
		{
			const bool is_selected = (camera_current_idx == n);
			if (ImGui::Selectable(m_CameraName[n], is_selected))
				camera_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}


	if (m_iCurrentCameraIndex != camera_current_idx)
	{
		m_iCurrentCameraIndex = camera_current_idx;
		m_tCurCameraDesc = m_CameraObjs[m_iCurrentCameraIndex]->Get_CameraObjDesc();
	}


	if (ImGui::Button(u8"카메라 선택 삭제"))
	{
		Delete_Camera();
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"카메라 전체 삭제"))
	{
		Delete_AllCamera();
	}

	ImGui::NewLine();

	ImGui::Text(u8"Camera 수정");

	if (0 < m_CameraObjs.size())
	{
		static CAMERAOBJ_DESC		cameraObjDesc = m_tCurCameraDesc;

		if (ImGui::BeginTable("StructTable", 10)) {
			ImGui::TableNextColumn();
			ImGui::Text("fFovY");
			ImGui::TableNextColumn();
			ImGui::InputFloat("##fFovY", &m_tCurCameraDesc.fFovY); // "##"를 사용하여 ID 레이블을 숨깁니다.

			ImGui::TableNextColumn();
			ImGui::Text("Aspect");
			ImGui::TableNextColumn();
			ImGui::InputFloat("##fAspect", &m_tCurCameraDesc.fAspect);

			ImGui::TableNextColumn();
			ImGui::Text("fNear");
			ImGui::TableNextColumn();
			ImGui::InputFloat("##Near", &m_tCurCameraDesc.fNear);

			ImGui::TableNextColumn();
			ImGui::Text("fFar");
			ImGui::TableNextColumn();
			ImGui::InputFloat("##Far", &m_tCurCameraDesc.fFar);

			ImGui::TableNextColumn();
			ImGui::Text("fMoveTime");
			ImGui::TableNextColumn();
			ImGui::InputFloat("##fMoveTime", &m_tCurCameraDesc.fMoveTime);

			ImGui::TableNextColumn();
			ImGui::Text("fMoveSpeed");
			ImGui::TableNextColumn();
			ImGui::InputFloat("##fMoveSpeed", &m_tCurCameraDesc.fMoveSpeed);

			ImGui::TableNextColumn();
			ImGui::Text("fStayTime");
			ImGui::TableNextColumn();
			ImGui::InputFloat("##fStayTime", &m_tCurCameraDesc.fStayTime);

			ImGui::TableNextColumn();
			ImGui::Text("Lerp");
			ImGui::TableNextColumn();	ImGui::Checkbox(u8"##선형보간할건지", &m_tCurCameraDesc.bLerp);

			ImGui::EndTable();
		}





		ImGui::NewLine();

		static	bool	bFocus = true;
		ImGui::Checkbox(u8"체크-Focus, 체크x-Eye", &bFocus);
		if (bFocus == true)
		{
			Edit_Focus_Transform();
		}
		else
		{
			Edit_Eye_Transform();
		}

		XMStoreFloat4(&m_tCurCameraDesc.vEye, m_CameraEyeObjs[m_iCurrentCameraIndex]->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat4(&m_tCurCameraDesc.vFocus, m_CameraFocusObjs[m_iCurrentCameraIndex]->Get_Transform()->Get_State(CTransform::STATE_POSITION));



		//ImGui::Text(u8"Focus - 확인용, 수정하지 말기!!!!");
		//ImGui::InputFloat("X", &m_tCurCameraDesc.vFocus.x);
		//ImGui::InputFloat("Y", &m_tCurCameraDesc.vFocus.y);
		//ImGui::InputFloat("Z", &m_tCurCameraDesc.vFocus.z);
		//ImGui::InputFloat("W", &m_tCurCameraDesc.vFocus.w);

		//ImGui::Text(u8"Eye - 확인용, 수정하지말기!!!!!!!");
		//ImGui::InputFloat("X", &m_tCurCameraDesc.vEye.x);
		//ImGui::InputFloat("Y", &m_tCurCameraDesc.vEye.y);
		//ImGui::InputFloat("Z", &m_tCurCameraDesc.vEye.z);
		//ImGui::InputFloat("W", &m_tCurCameraDesc.vEye.w);



		if (ImGui::Button(u8"카메라 수정"))
		{
			Edit_Camera();
		}
	}

	

}

void CCamera_Manager::Edit_Camera()
{
	m_CameraObjs[m_iCurrentCameraIndex]->Edit_CameraObjDesc(m_tCurCameraDesc);
}

void CCamera_Manager::Edit_Focus_Transform()
{
	if (0 == m_CameraFocusObjs.size())
		return;

	vector<CCameraInstallObj*>::iterator iter = m_CameraFocusObjs.begin();

	if (0 != m_iCurrentCameraIndex)
	{
		for (int i = 0; i < m_iCurrentCameraIndex; i++)
		{
			iter++;
		}
	}

	EditTransformF((float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW),
		(float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ),
		(float*)dynamic_cast<CCameraInstallObj*>(*iter)->Get_TransformCom()->Get_WorldFloat4x4(),
		true);
}

void CCamera_Manager::Edit_Eye_Transform()
{
	if (0 == m_CameraEyeObjs.size())
		return;

	vector<CCameraInstallObj*>::iterator iter = m_CameraEyeObjs.begin();

	if (0 != m_iCurrentCameraIndex)
	{
		for (int i = 0; i < m_iCurrentCameraIndex; i++)
		{
			iter++;
		}
	}

	EditTransformE((float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW),
		(float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ),
		(float*)dynamic_cast<CCameraInstallObj*>(*iter)->Get_TransformCom()->Get_WorldFloat4x4(),
		true);
}

void CCamera_Manager::Test_Camera(int iCamNum)
{
	if (nullptr == m_pCineCamera)
	{
		CCineCamera::CINE_CAMERA_DESC		cineDesc;
		cineDesc.iFileNum = iCamNum;
		m_pCineCamera = dynamic_cast<CCineCamera*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_CCineCamera"), &cineDesc));
	}
	else
	{
		m_pCineCamera->Load_CamBin(iCamNum);
		m_pCineCamera->Setting_Start_Cinemachine();
	}
	
	CSystemManager::GetInstance()->Set_Camera(CAMERA_CINEMACHINE);
}

void CCamera_Manager::Save_Cameras_InFile(int iSaveNum, bool bFirstLerp)
{
	CAMERAOBJ_IO			CameraIO;
	CameraIO.iCameraNum = m_CameraObjs.size();
	CameraIO.bFirstLerp = bFirstLerp;
	CameraIO.pCamObjDesc = new CAMERAOBJ_DESC[CameraIO.iCameraNum];

	int iIndex = 0;

	for (auto& iter : m_CameraObjs)
	{
		CameraIO.pCamObjDesc[iIndex] = m_CameraObjs[iIndex]->Get_CameraObjDesc();
		iIndex++;
	}

	Export_Bin_Cam_Data(&CameraIO, iSaveNum);

	Safe_Delete_Array(CameraIO.pCamObjDesc);
}

void CCamera_Manager::Load_Cameras_File(int iNum)
{
	Delete_AllCamera();

	CAMERAOBJ_IO		camIODesc;
	Import_Bin_Cam_Data_OnTool(&camIODesc, m_FileNames[iNum]);

	m_bFirstLerp = camIODesc.bFirstLerp;

	for (int i = 0; i < camIODesc.iCameraNum; i++)
	{
		CAMERAOBJ_DESC		cameraDesc;
		cameraDesc = camIODesc.pCamObjDesc[i];

		CCameraInstallObj::CAMERAINSTALL_DESC		camInstallDesc;

		/* Focus 추가 */
		XMMATRIX startPos;
		startPos = XMMatrixIdentity();
		startPos.r[3].m128_f32[0] = cameraDesc.vFocus.x;
		startPos.r[3].m128_f32[1] = cameraDesc.vFocus.y;
		startPos.r[3].m128_f32[2] = cameraDesc.vFocus.z;
		startPos.r[3].m128_f32[3] = cameraDesc.vFocus.w;

		camInstallDesc.vStartPos = startPos;
		camInstallDesc.eInstallDesc = CCameraInstallObj::CAM_FOCUS;
		m_CameraFocusObjs.push_back(dynamic_cast<CCameraInstallObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_CameraInstallObj"), &camInstallDesc)));

		/* Eye 추가 */
		startPos = XMMatrixIdentity();
		startPos.r[3].m128_f32[0] = cameraDesc.vEye.x;
		startPos.r[3].m128_f32[1] = cameraDesc.vEye.y;
		startPos.r[3].m128_f32[2] = cameraDesc.vEye.z;
		startPos.r[3].m128_f32[3] = cameraDesc.vEye.w;

		camInstallDesc.vStartPos = startPos;
		camInstallDesc.eInstallDesc = CCameraInstallObj::CAM_EYE;
		m_CameraEyeObjs.push_back(dynamic_cast<CCameraInstallObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_CameraInstallObj"), &camInstallDesc)));

		
		CCameraObj::CAMERAOBJINPUT_DESC		cameraInputDesc;
		cameraInputDesc.tObjDesc = cameraDesc;

		m_CameraObjs.push_back(dynamic_cast<CCameraObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_CameraObj"), &cameraInputDesc)));

	}

	m_iCurrentCameraIndex = 0;
	m_tCurCameraDesc = m_CameraObjs[0]->Get_CameraObjDesc();

	Update_CameraName();
	Update_EyeName();
	Update_FocusName();

	Safe_Delete_Array(camIODesc.pCamObjDesc);
}

HRESULT CCamera_Manager::Import_Bin_Cam_Data_OnTool(CAMERAOBJ_IO* camData, char* fileName)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/CameraData/");

	strcat_s(fullPath, fileName);

	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}


	in.read((char*)&camData->iCameraNum, sizeof(int));

	in.read((char*)&camData->bFirstLerp, sizeof(bool));

	camData->pCamObjDesc = new CAMERAOBJ_DESC[camData->iCameraNum];

	for (int i = 0; i < camData->iCameraNum; i++)
	{
		in.read((char*)&camData->pCamObjDesc[i], sizeof(CAMERAOBJ_DESC));
	}

	in.close();

	return S_OK;
}

HRESULT CCamera_Manager::Export_Bin_Cam_Data(CAMERAOBJ_IO* camData, int iSaveNum)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/CameraData/");

	strcat_s(fullPath, "Camera_Data");
	strcat_s(fullPath, "_");
	string strNum = to_string(iSaveNum);
	char cLevel[20];
	strcpy_s(cLevel, strNum.c_str());
	strcat_s(fullPath, cLevel);
	strcat_s(fullPath, ".dat");

	fs::path path(fullPath);

	ofstream out(fullPath, ios::binary);

	if (!out) {
		return E_FAIL;
	}

	/* 라이트 개수 저장 */
	int iNumCamObj = camData->iCameraNum;
	out.write((char*)&iNumCamObj, sizeof(int));

	bool bFistLerp = camData->bFirstLerp;
	out.write((char*)&bFistLerp, sizeof(bool));

	/* 라이트 정보 저장 */
	for (int i = 0; i < iNumCamObj; i++)
	{
		CAMERAOBJ_DESC pCamDesc = camData->pCamObjDesc[i];
		out.write((char*)&pCamDesc, sizeof(CAMERAOBJ_DESC));
	}

	out.close();

	return S_OK;
}

void CCamera_Manager::Free()
{
	for (auto& iter : m_FileNames)
		Safe_Delete(iter);
	m_FileNames.clear();

	for (auto& iter : m_CameraFocusNames)
		Safe_Delete(iter);
	m_CameraFocusNames.clear();

	for (auto& iter : m_CameraEyeNames)
		Safe_Delete(iter);
	m_CameraEyeNames.clear();

	for (auto& iter : m_CameraName)
		Safe_Delete(iter);
	m_CameraName.clear();

	for (auto& iter : m_CameraObjs)
		Safe_Release(iter);
	m_CameraObjs.clear();

	for (auto& iter : m_CameraFocusObjs)
		Safe_Release(iter);
	m_CameraFocusObjs.clear();

	for (auto& iter : m_CameraEyeObjs)
		Safe_Release(iter);
	m_CameraEyeObjs.clear();

	if (nullptr != m_pCineCamera)
		Safe_Release(m_pCineCamera);

	Safe_Release(m_pGameInstance);
}
