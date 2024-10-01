#include "Trigger_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"
#include "SystemManager.h"


#include <iostream>
#include <io.h>

bool useWindowT = false;
int gizmoCountT = 1;
static ImGuizmo::OPERATION mCurrentGizmoOperationT(ImGuizmo::TRANSLATE);


// TRANSFORM의 MATRIX
void EditTransformT(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
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
			mCurrentGizmoOperationT = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperationT = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperationT = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperationT == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperationT = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperationT == ImGuizmo::ROTATE))
			mCurrentGizmoOperationT = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperationT == ImGuizmo::SCALE))
			mCurrentGizmoOperationT = ImGuizmo::SCALE;
		/* if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			 mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperationT != ImGuizmo::SCALE)
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

		switch (mCurrentGizmoOperationT)
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
	if (useWindowT)
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
	}
	else
	{
		// 이거만 신경쓰기 얼마만큼 기즈모 화면값
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperationT, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	if (useWindowT)
	{
		ImGui::End();
		// 이거해보기 터지면
		ImGui::PopStyleColor(1);
	}
}

IMPLEMENT_SINGLETON(CTrigger_Manager)

CTrigger_Manager::CTrigger_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTrigger_Manager::Initialize()
{
    return S_OK;
}

void CTrigger_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CTrigger_Manager::Tick(_float fTimeDelta)
{
}

void CTrigger_Manager::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_TriggerObjects)
		iter->Late_Tick(fTimeDelta);

}

void CTrigger_Manager::Render()
{
}

void CTrigger_Manager::Show_FileName()
{
	ImGui::Begin(u8"트리거 파일");

	ImGui::Text(u8"트리거 리스트 ");

	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	static int layer_current_idx;
	if (ImGui::BeginListBox("listbox 1"))
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

	/* 트리거 로드 */
	if (ImGui::Button(u8"트리거 파일 로드") && m_FileNames.size() > 0)
	{
		Load_Triggers_File(layer_current_idx);
		Update_FileName();
	}

	ImGui::NewLine();

	/* 저장 번호 */
	static int iSaveNum;
	ImGui::InputInt(u8"저장번호 : ", &iSaveNum);

	if (ImGui::Button(u8"트리거 저장"))
	{
		Save_Triggers_InFile(iSaveNum);
		Update_FileName();
	}


	if (0 < m_TriggerObjects.size())
	{
		static int trigger_current_idx = 0;

		if (trigger_current_idx != m_iCurrentTriggerIndex)
		{
			trigger_current_idx = m_iCurrentTriggerIndex;
			m_tCurTriggerDesc = m_TriggerObjects[m_iCurrentTriggerIndex]->Get_TriggerDesc();
		}


		if (ImGui::BeginListBox("listbox 0"))
		{
			for (int n = 0; n < m_TriggerNames.size(); n++)
			{
				/*const bool is_selected = (cell_current_idx == n);*/
				const bool is_selected = (trigger_current_idx == n);
				if (ImGui::Selectable(m_TriggerNames[n], is_selected))
				{
					/*cell_current_idx = n;
					m_iCurrentCellIndex = trigger_current_idx;*/
					trigger_current_idx = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		Click_To_Select_Trigger(trigger_current_idx);

		/* 다른 오브젝트 클릭시 */
		if (m_iCurrentTriggerIndex != trigger_current_idx && m_TriggerObjects.size() > 0)
		{
			m_iCurrentTriggerIndex = trigger_current_idx;
			m_tCurTriggerDesc = m_TriggerObjects[m_iCurrentTriggerIndex]->Get_TriggerDesc();
		}


	}


	ImGui::NewLine();

	if (!m_TriggerObjects.empty())
	{
		Edit_GameObject_Transform(m_iCurrentTriggerIndex);

		Edit_Installed_Trigger();

	}

	ImGui::End();
}

void CTrigger_Manager::Show_Add_Trigger_IMGUI()
{
	ImGui::Text(u8"트리거 종류");
	static int triggerType = TRIGGER_TYPE::TRIGGER_MOVE_LEVEL;
	if (ImGui::RadioButton(u8"Trigger MoveLevel", triggerType == TRIGGER_TYPE::TRIGGER_MOVE_LEVEL))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_MOVE_LEVEL;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"시네머신", triggerType == TRIGGER_TYPE::TRIGGER_CINEMACHINE))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_CINEMACHINE;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"요네다", triggerType == TRIGGER_TYPE::TRIGGER_YONEDA))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_YONEDA;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"monster trigger", triggerType == TRIGGER_TYPE::TRIGGER_MONSTER))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_MONSTER;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"Quest trigger", triggerType == TRIGGER_TYPE::TRIGGER_QUEST))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_QUEST;
	}

	ImGui::NewLine();

	ImGui::Text(u8"씬 이동 유무");
	static bool bscenemove = false;
	ImGui::Checkbox(u8"체크-씬이동o, 체크x-씬이동x", &bscenemove);


	ImGui::NewLine();

	static int			ilevelNum = 0;
	if (TRIGGER_TYPE::TRIGGER_MONSTER != triggerType)
	{
		if (ImGui::BeginListBox(u8"이동 레벨"))
		{
			for (int n = 0; n < m_Levels.size(); n++)
			{
				const bool is_selected = (ilevelNum == n);
				if (ImGui::Selectable(m_Levels[n], is_selected))
				{
					ilevelNum = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
	else
	{
		ImGui::Text(u8"사용x");
		if (ImGui::BeginListBox(u8"몬스터 트리거"))
		{
			for (int n = 0; n < m_TriggerLevel.size(); n++)
			{
				const bool is_selected = (ilevelNum == n);
				if (ImGui::Selectable(m_TriggerLevel[n], is_selected))
				{
					ilevelNum = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
	

	static int			iCineNum = 0;
	ImGui::InputInt(u8"시네머신 번호", &iCineNum);

	static int			iYonedaKnife = 0;
	ImGui::InputInt(u8"요네다 칼 유무", &iYonedaKnife);

	ImGui::NewLine();

	static int iTriggerID = 0;
	ImGui::InputInt(u8"트리거 ID - 몬스터트리거 넘버로도 사용", &iTriggerID);


	ImGui::Text(u8"트리거 추가 - 추가 버튼 누르고 pick ");

	if (ImGui::Button(u8"트리거 추가"))
	{
		m_bAddTrigger = true;
	}

	TRIGGER_DESC		triggerAddDesc;
	triggerAddDesc.iTriggerType = triggerType;
	triggerAddDesc.bMoveScene = bscenemove;
	triggerAddDesc.iLevelNum = ilevelNum;
	triggerAddDesc.iCineNum = iCineNum;
	triggerAddDesc.iYonedaKnife = iYonedaKnife;
	triggerAddDesc.iTriggerID = iTriggerID;

	/* 왼쪽 클릭할 시 트리거 추가 */
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB) && true == m_bAddTrigger)
	{
		_bool		isPick;
		_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);


		_matrix			startPos;
		startPos = XMMatrixIdentity();
		startPos.r[3].m128_f32[0] = vTargetPos.m128_f32[0];
		startPos.r[3].m128_f32[1] = vTargetPos.m128_f32[1];
		startPos.r[3].m128_f32[2] = vTargetPos.m128_f32[2];
		startPos.r[3].m128_f32[3] = vTargetPos.m128_f32[3];

		CTriggerObj::TRIGGEROBJ_DESC		triggerObjDesc;
		triggerObjDesc.vStartPos = startPos;
		triggerObjDesc.tTriggerDesc = triggerAddDesc;

		m_TriggerObjects.push_back(dynamic_cast<CTriggerObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_TriggerObj"), &triggerObjDesc)));

		m_bAddTrigger = false;

		/* 추가되면 추가된 index로 이동 */
		m_iCurrentTriggerIndex = m_TriggerObjects.size() - 1;

		Update_TriggerObjName();

	}
}

void CTrigger_Manager::Save_Triggers_InFile(int iSaveNum)
{
	TRIGGER_IO		triggerIO;

	triggerIO.iTriggerNum = m_TriggerObjects.size();
	triggerIO.pTriggers = new TRIGGER_DESC[m_TriggerObjects.size()];

	int iIndex = 0;

	for (auto& iter : m_TriggerObjects)
	{
		triggerIO.pTriggers[iIndex] = m_TriggerObjects[iIndex]->Get_TriggerDesc();
		iIndex++;
	}

	Export_Bin_Triggers_Data(&triggerIO, iSaveNum);

	Safe_Delete_Array(triggerIO.pTriggers);
}

void CTrigger_Manager::Load_Triggers_File(int iNum)
{
	Delete_AllTrigger();

	TRIGGER_IO		triggerIODesc;
	Import_Bin_Triggers_Data_OnTool(&triggerIODesc, m_FileNames[iNum]);

	for (int i = 0; i < triggerIODesc.iTriggerNum; i++)
	{
		TRIGGER_DESC		triggerDesc;
		triggerDesc = triggerIODesc.pTriggers[i];

		CTriggerObj::TRIGGEROBJ_DESC triggerObjDesc;
		triggerObjDesc.vStartPos = XMLoadFloat4x4(&triggerDesc.vTransform);
		triggerObjDesc.tTriggerDesc = triggerDesc;
		m_TriggerObjects.push_back(dynamic_cast<CTriggerObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_TriggerObj"), &triggerObjDesc)));
	}

	for (int i = 0; i < triggerIODesc.iTriggerNum; i++)
	{
		Safe_Delete(triggerIODesc.pTriggers);
	}

	Update_TriggerObjName();

	m_tCurTriggerDesc = m_TriggerObjects[0]->Get_TriggerDesc();

	Safe_Delete_Array(triggerIODesc.pTriggers);

}

HRESULT CTrigger_Manager::Import_Bin_Triggers_Data_OnTool(TRIGGER_IO* triggerIOData, char* fileName)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/TriggerData/");

	strcat_s(fullPath, fileName);

	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}


	in.read((char*)&triggerIOData->iTriggerNum, sizeof(int));

	triggerIOData->pTriggers = new TRIGGER_DESC[triggerIOData->iTriggerNum];

	for (int i = 0; i < triggerIOData->iTriggerNum; i++)
	{
		in.read((char*)&triggerIOData->pTriggers[i], sizeof(TRIGGER_DESC));
	}

	in.close();

	return S_OK;
}

HRESULT CTrigger_Manager::Export_Bin_Triggers_Data(TRIGGER_IO* triggerIOData, int iSaveNum)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/TriggerData/");

	strcat_s(fullPath, "Trigger_Data");

	string strNum = to_string(iSaveNum);
	char cLevel[20];
	strcpy_s(cLevel, strNum.c_str());
	strcat_s(fullPath, "_");
	strcat_s(fullPath, cLevel);
	strcat_s(fullPath, ".dat");

	fs::path path(fullPath);

	ofstream out(fullPath, ios::binary);

	if (!out) {
		return E_FAIL;
	}

	/* 라이트 개수 저장 */
	int iNumTriggerObj = triggerIOData->iTriggerNum;
	out.write((char*)&iNumTriggerObj, sizeof(int));


	/* 라이트 정보 저장 */
	for (int i = 0; i < iNumTriggerObj; i++)
	{
		TRIGGER_DESC pTriggerDesc = triggerIOData->pTriggers[i];
		out.write((char*)&pTriggerDesc, sizeof(TRIGGER_DESC));
	}

	out.close();

	return S_OK;
}

void CTrigger_Manager::Update_FileName()
{
	for (auto& iter : m_FileNames)
		Safe_Delete(iter);

	m_FileNames.clear();
	string path = "../../Client/Bin/DataFiles/TriggerData/*.dat";

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

void CTrigger_Manager::Update_TriggerObjName()
{
	if (0 <= m_TriggerNames.size())
	{
		for (auto& iter : m_TriggerNames)
			Safe_Delete(iter);

		m_TriggerNames.clear();
	}

	if (0 < m_TriggerObjects.size())
	{
		for (int i = 0; i < m_TriggerObjects.size(); i++)
		{
			m_TriggerObjects[i]->Set_ObjID(i + 1);
			char* szName = new char[MAX_PATH];
			strcpy(szName, "LightObject");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_TriggerNames.push_back(szName);
		}
	}
}


void CTrigger_Manager::Edit_GameObject_Transform(int iNumObject)
{
	vector<CTriggerObj*>::iterator iter = m_TriggerObjects.begin();

	if (0 != iNumObject)
	{
		for (int i = 0; i < iNumObject; i++)
		{
			iter++;
		}
	}

	EditTransformT((float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW),
		(float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ),
		(float*)dynamic_cast<CGameObject*>(*iter)->Get_TransformCom()->Get_WorldFloat4x4(),
		true);
}

void CTrigger_Manager::Edit_Installed_Trigger()
{
	m_tCurTriggerDesc = m_TriggerObjects[m_iCurrentTriggerIndex]->Get_TriggerDesc();

	ImGui::Text(u8"트리거 종류");
	static int triggerType = m_tCurTriggerDesc.iTriggerType;
	if (ImGui::RadioButton(u8"Trigger - move_level", m_tCurTriggerDesc.iTriggerType == TRIGGER_TYPE::TRIGGER_MOVE_LEVEL))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_MOVE_LEVEL;
		m_tCurTriggerDesc.iTriggerType = TRIGGER_TYPE::TRIGGER_MOVE_LEVEL;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"trigger - cinemachine", m_tCurTriggerDesc.iTriggerType == TRIGGER_TYPE::TRIGGER_CINEMACHINE))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_CINEMACHINE;
		m_tCurTriggerDesc.iTriggerType = TRIGGER_TYPE::TRIGGER_CINEMACHINE;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"trigger - yoneda", m_tCurTriggerDesc.iTriggerType == TRIGGER_TYPE::TRIGGER_YONEDA))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_YONEDA;
		m_tCurTriggerDesc.iTriggerType = TRIGGER_TYPE::TRIGGER_YONEDA;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"trigger - MONSTER ", m_tCurTriggerDesc.iTriggerType == TRIGGER_TYPE::TRIGGER_MONSTER))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_MONSTER;
		m_tCurTriggerDesc.iTriggerType = TRIGGER_TYPE::TRIGGER_MONSTER;
	}
	ImGui::NewLine();
	if (ImGui::RadioButton(u8"trigger - Quest ", m_tCurTriggerDesc.iTriggerType == TRIGGER_TYPE::TRIGGER_QUEST))
	{
		triggerType = TRIGGER_TYPE::TRIGGER_QUEST;
		m_tCurTriggerDesc.iTriggerType = TRIGGER_TYPE::TRIGGER_QUEST;
	}


	ImGui::NewLine();

	ImGui::Text(u8"레벨 이동");

	static bool bLevel = m_tCurTriggerDesc.bMoveScene;
	ImGui::Checkbox(u8"체크-씬이동", &m_tCurTriggerDesc.bMoveScene);

	ImGui::NewLine();


	static int			ilevelNum = m_tCurTriggerDesc.iLevelNum;

	if (TRIGGER_TYPE::TRIGGER_MONSTER != m_tCurTriggerDesc.iTriggerType)
	{
		if (ImGui::BeginListBox(u8"이동 레벨"))
		{
			for (int n = 0; n < m_Levels.size(); n++)
			{
				/*const bool is_selected = (cell_current_idx == n);*/
				const bool is_selected = (m_tCurTriggerDesc.iLevelNum == n);
				if (ImGui::Selectable(m_Levels[n], is_selected))
				{
					/*cell_current_idx = n;
					m_iCurrentCellIndex = trigger_current_idx;*/
					m_tCurTriggerDesc.iLevelNum = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
	else
	{
		if (ImGui::BeginListBox(u8"몬스터 트리거"))
		{
			for (int n = 0; n < m_TriggerLevel.size(); n++)
			{
				const bool is_selected = (ilevelNum == n);
				if (ImGui::Selectable(m_TriggerLevel[n], is_selected))
				{
					ilevelNum = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}


	ImGui::NewLine();

	static int iCineNum = m_tCurTriggerDesc.iCineNum;
	ImGui::InputInt(u8"시네머신번호", &m_tCurTriggerDesc.iCineNum);

	ImGui::NewLine();

	static int iYonedaKnife = m_tCurTriggerDesc.iYonedaKnife;
	ImGui::InputInt(u8"요네다 칼 유무", &m_tCurTriggerDesc.iYonedaKnife);

	ImGui::NewLine();

	static int iTriggerID = m_tCurTriggerDesc.iTriggerID;
	ImGui::InputInt(u8"트리거 ID", &m_tCurTriggerDesc.iTriggerID);


	ImGui::NewLine();

	


	m_TriggerObjects[m_iCurrentTriggerIndex]->Set_TriggerDesc(m_tCurTriggerDesc);



	if (ImGui::Button(u8"trigger 선택 삭제"))
	{
		if (0 < m_TriggerObjects.size())
			Delete_Trigger();
	}

	if (ImGui::Button(u8"trigger 전체 삭제"))
	{
		if (0 < m_TriggerObjects.size())
			Delete_AllTrigger();
	}
}

void CTrigger_Manager::Click_To_Select_Trigger(int& iObjectNum)
{
	if (HOLD == m_pGameInstance->GetKeyState(DIK_F))
	{
		m_bFindObject = true;
	}
	else
	{
		m_bFindObject = false;
	}

	if (true == m_bFindObject)
	{
		m_bFindObject = false;

		_bool		isPick;

		_float		iIndex = 0;
		iIndex = m_pGameInstance->FindObjID(&isPick);

		if (iIndex > 0)
		{
			/* ID는 1부터 시작하니까 INDEX 받을때는 -1 해주기 */
			iObjectNum = iIndex - 1;

		}

	}
}

void CTrigger_Manager::Delete_Trigger()
{
	vector<class CTriggerObj*>::iterator	iter = m_TriggerObjects.begin();

	for (int i = 0; i < m_iCurrentTriggerIndex; i++)
	{
		iter++;
	}

	Safe_Release(*iter);
	m_TriggerObjects.erase(iter);

	/* 인덱스 옮겨주기 */
	if (0 > m_iCurrentTriggerIndex - 1)
	{
		m_iCurrentTriggerIndex = 0;
	}
	else
	{
		m_iCurrentTriggerIndex -= 1;
	}

	Update_TriggerObjName();
}

void CTrigger_Manager::Delete_AllTrigger()
{
	for (auto& iter : m_TriggerObjects)
		Safe_Release(iter);
	m_TriggerObjects.clear();

	Update_TriggerObjName();

	m_iCurrentTriggerIndex = 0;
}

void CTrigger_Manager::Free()
{
	for (auto& iter : m_TriggerObjects)
		Safe_Release(iter);
	m_TriggerObjects.clear();

	for (auto& iter : m_FileNames)
		Safe_Delete(iter);
	m_FileNames.clear();

	for (auto& iter : m_TriggerNames)
		Safe_Delete(iter);
	m_TriggerNames.clear();

	m_Levels.clear();
	m_TriggerLevel.clear();

	Safe_Release(m_pGameInstance);
}
