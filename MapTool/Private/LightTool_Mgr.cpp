#include "LightTool_Mgr.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"

#include <iostream>
#include <io.h>

bool useWindowL = false;
int gizmoCountL = 1;
static ImGuizmo::OPERATION mCurrentGizmoOperationL(ImGuizmo::TRANSLATE);


// TRANSFORM의 MATRIX
void EditTransformL(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
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
			mCurrentGizmoOperationL = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperationL = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperationL = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperationL == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperationL = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperationL == ImGuizmo::ROTATE))
			mCurrentGizmoOperationL = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperationL == ImGuizmo::SCALE))
			mCurrentGizmoOperationL = ImGuizmo::SCALE;
		/* if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			 mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperationL != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		//if (ImGui::IsKeyPressed(ImGuiKey_S))
		//	useSnap = !useSnap;
		//ImGui::Checkbox("##UseSnap", &useSnap);
		//ImGui::SameLine();

		//switch (mCurrentGizmoOperationL)
		//{
		//case ImGuizmo::TRANSLATE:
		//	ImGui::InputFloat3("Snap", &snap[0]);
		//	break;
		//case ImGuizmo::ROTATE:
		//	ImGui::InputFloat("Angle Snap", &snap[0]);
		//	break;
		//case ImGuizmo::SCALE:
		//	ImGui::InputFloat("Scale Snap", &snap[0]);
		//	break;
		//}
		/*ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}*/
	}

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;
	if (useWindowL)
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
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperationL, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	// 화면 상단 카메라 돌아가는애
	//ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);


	if (useWindowL)
	{
		ImGui::End();
		// 이거해보기 터지면
		ImGui::PopStyleColor(1);
	}
}


IMPLEMENT_SINGLETON(CLightTool_Mgr)

CLightTool_Mgr::CLightTool_Mgr()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLightTool_Mgr::Initialize()
{
	m_tCurLightDesc.vDiffuse = _float4(1, 1, 1, 1);
	m_tCurLightDesc.vAmbient = _float4(1, 1, 1, 1);
	m_tCurLightDesc.vSpecular = _float4(1, 1, 1, 1);

	return S_OK;
}

void CLightTool_Mgr::Priority_Tick(_float fTimeDelta)
{
}

void CLightTool_Mgr::Tick(_float fTimeDelta)
{
}

void CLightTool_Mgr::Late_Tick(_float fTimeDelta)
{
	if (true == m_bRender)
	{
		for (auto& iter : m_LightObjects)
			iter->Late_Tick(fTimeDelta);
	}
}

void CLightTool_Mgr::Render()
{
}

void CLightTool_Mgr::Show_Add_Light_IMGUI()
{


	ImGui::Text(u8"라이트 배치 전 초기화 누르고 시작해야함 - engine light manager 초기화하고 시작");

	ImGui::NewLine();

	if (ImGui::Button(u8"라이트 초기화"))
	{
		Initialize_Light_Environment();
	}

	ImGui::Text(u8"라이트 종류");
	static int lightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	if (ImGui::RadioButton(u8"TYPE_DIRECTIONAL", lightType == LIGHT_DESC::TYPE_DIRECTIONAL))
	{
		lightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"TYPE_POINT", lightType == LIGHT_DESC::TYPE_POINT))
	{
		lightType = LIGHT_DESC::TYPE_POINT;
	}

	ImGui::NewLine();

	ImGui::Text(u8"라이트 방향");
	static XMFLOAT4		vDirection = _float4(0,0,0,0);
	ImGui::InputFloat4(u8"라이트 방향", reinterpret_cast<float*>(&vDirection));
	/*ImGui::InputFloat("x", &m_tCurLightDesc.vDirection.x);
	ImGui::InputFloat("y", &m_tCurLightDesc.vDirection.y);
	ImGui::InputFloat("z", &m_tCurLightDesc.vDirection.z);
	ImGui::InputFloat("w", &m_tCurLightDesc.vDirection.w);*/

	ImGui::NewLine();

	static XMFLOAT4		vPosition = _float4(0, 0, 0, 0);

	ImGui::Text(u8"라이트 위치 - Directional은 값 넣어주기, POINT는 픽킹값 ");
	if (lightType == LIGHT_DESC::TYPE_DIRECTIONAL)
	{
		ImGui::Text(u8"라이트 위치");
		ImGui::InputFloat4(u8"라이트 위치", reinterpret_cast<float*>(&vPosition));
	}

	ImGui::NewLine();

	ImGui::Text(u8"라이트 범위");
	static float		fRange = 0;
	ImGui::InputFloat("x", &fRange);

	ImGui::NewLine();
	//ImGui::ColorEdit4(u8"vDiffuse 선택", (float*)&m_tCurLightDesc.vDiffuse);
	static ImVec4 vDiffuseColor = ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
	ImGui::ColorEdit4(u8"vDiffuse 선택", (float*)&vDiffuseColor, ImGuiColorEditFlags_Float);


	ImGui::NewLine();
	static ImVec4 vAmbientColor = ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
	ImGui::ColorEdit4(u8"vAmbient 선택", (float*)&vAmbientColor, ImGuiColorEditFlags_Float);
	

	ImGui::NewLine();
	static ImVec4 vSpecularColor = ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
	ImGui::ColorEdit4(u8"vSpecular 선택", (float*)&vSpecularColor, ImGuiColorEditFlags_Float);


	ImGui::Text(u8"라이트 추가 : 추가버튼 누른후 왼쪽 마우스 클릭 ");
	if (ImGui::Button(u8"라이트 추가"))
	{
		m_bLightAdd = true;
	}

	LIGHT_DESC		lightDesc;
	lightDesc.eType = static_cast<LIGHT_DESC::TYPE>(lightType);
	lightDesc.vDirection = vDirection;
	lightDesc.vPosition = vPosition;
	lightDesc.fRange = fRange;
	lightDesc.vDiffuse = _float4(vDiffuseColor.x, vDiffuseColor.y, vDiffuseColor.z, vDiffuseColor.w);
	lightDesc.vAmbient = _float4(vAmbientColor.x, vAmbientColor.y, vAmbientColor.z, vAmbientColor.w);
	lightDesc.vSpecular = _float4(vSpecularColor.x, vSpecularColor.y, vSpecularColor.z, vSpecularColor.w);


	/* 왼쪽 클릭할 시 라이트 추가 */
	Add_Light_Imgui(lightDesc);



}

void CLightTool_Mgr::Show_FileName()
{
	ImGui::Begin(u8"라이트 파일");

	ImGui::Text(u8" 라이트 리스트 ");

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

	/* 라이트 로드 */
	if (ImGui::Button(u8"라이트 파일 로드") && m_FileNames.size() > 0)
	{
		Load_Lights_File(layer_current_idx);
		Update_FileName();
	}

	Show_Installed_LightsList();

	ImGui::End();
}

void CLightTool_Mgr::Save_Lights_InFile(int iSaveNum)
{
	LIGHT_DESC_IO		lightDescIO;

	lightDescIO.iNumLights = m_LightObjects.size();
	lightDescIO.pLightDesc = new LIGHT_DESC[m_LightObjects.size()];

	int iIndex = 0;

	for (auto& iter : m_LightObjects)
	{
		lightDescIO.pLightDesc[iIndex] = m_LightObjects[iIndex]->Get_LightDesc();
		iIndex++;
	}

	Export_Bin_Light_Data(&lightDescIO, iSaveNum);

	Safe_Delete_Array(lightDescIO.pLightDesc);
}

void CLightTool_Mgr::Load_Lights_File(int iNum)
{
	/* 맵 저장 정보 로드 */
	Delete_AllLights();

	LIGHT_DESC_IO		lightIODesc;
	Import_Bin_Light_Data_OnTool(&lightIODesc, m_FileNames[iNum]);

	for (int i = 0; i < lightIODesc.iNumLights; i++)
	{
		LIGHT_DESC		lightDesc;
		lightDesc = lightIODesc.pLightDesc[i];

		/* gameinstance에 추가 */
		m_pGameInstance->Add_Light(lightDesc);

		XMMATRIX startPos;
		startPos = XMMatrixIdentity();
		startPos.r[3].m128_f32[0] = lightDesc.vPosition.x;
		startPos.r[3].m128_f32[1] = lightDesc.vPosition.y;
		startPos.r[3].m128_f32[2] = lightDesc.vPosition.z;
		startPos.r[3].m128_f32[3] = lightDesc.vPosition.w;

		CLightObj::LIGHTOBJ_DESC		lightObjDesc;
		lightObjDesc.vStartPos = startPos;
		lightObjDesc.tLightDesc = lightDesc;
		m_LightObjects.push_back(dynamic_cast<CLightObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_LightObj"), &lightObjDesc)));
	}

	for (int i = 0; i < lightIODesc.iNumLights; i++)
	{
		Safe_Delete(lightIODesc.pLightDesc);
	}

	Update_LightsName();

	m_tCurLightDesc = m_LightObjects[0]->Get_LightDesc();

	Safe_Delete_Array(lightIODesc.pLightDesc);

}

HRESULT CLightTool_Mgr::Import_Bin_Light_Data_OnTool(LIGHT_DESC_IO* lightObjData, char* fileName)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/LightData/");

	strcat_s(fullPath, fileName);

	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}


	in.read((char*)&lightObjData->iNumLights, sizeof(int));

	lightObjData->pLightDesc = new LIGHT_DESC[lightObjData->iNumLights];

	for (int i = 0; i < lightObjData->iNumLights ; i++)
	{
		in.read((char*)&lightObjData->pLightDesc[i], sizeof(LIGHT_DESC));
	}

	in.close();

	return S_OK;
}

HRESULT CLightTool_Mgr::Export_Bin_Light_Data(LIGHT_DESC_IO* lightObjData, int iSaveNum)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/LightData/");

	strcat_s(fullPath, "Light_Data");

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
	int iNumLightObj = lightObjData->iNumLights;
	out.write((char*)&iNumLightObj, sizeof(int));


	/* 라이트 정보 저장 */
	for (int i = 0; i < iNumLightObj ; i++)
	{
		LIGHT_DESC pLightDesc = lightObjData->pLightDesc[i];
		out.write((char*)&pLightDesc, sizeof(LIGHT_DESC));
	}

	out.close();

	return S_OK;
}

void CLightTool_Mgr::Update_FileName()
{
	for (auto& iter : m_FileNames)
		Safe_Delete(iter);

	m_FileNames.clear();
	string path = "../../Client/Bin/DataFiles/LightData/*.dat";

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

void CLightTool_Mgr::Select_LightDesc()
{
}

bool CLightTool_Mgr::Add_Light_Imgui(LIGHT_DESC	lightAddDesc)
{
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB) && m_bLightAdd == true)
	{
		/* 처음추가될때*/
		if (m_LightObjects.size() == 0)
			m_bUpdateInfrom = true;

		_matrix			startPos;
		startPos = XMMatrixIdentity();

		if (lightAddDesc.eType == LIGHT_DESC::TYPE_POINT)
		{
			_bool		isPick;
			_vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

			startPos = XMMatrixIdentity();
			startPos.r[3].m128_f32[0] = vTargetPos.m128_f32[0];
			startPos.r[3].m128_f32[1] = vTargetPos.m128_f32[1];
			startPos.r[3].m128_f32[2] = vTargetPos.m128_f32[2];
			startPos.r[3].m128_f32[3] = vTargetPos.m128_f32[3];

			lightAddDesc.vPosition = _float4(vTargetPos.m128_f32[0], vTargetPos.m128_f32[1], vTargetPos.m128_f32[2], vTargetPos.m128_f32[3]);
		}
		else if (lightAddDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL)
		{
			startPos = XMMatrixIdentity();
			startPos.r[3].m128_f32[0] = lightAddDesc.vPosition.x;
			startPos.r[3].m128_f32[1] = lightAddDesc.vPosition.y;
			startPos.r[3].m128_f32[2] = lightAddDesc.vPosition.z;
			startPos.r[3].m128_f32[3] = lightAddDesc.vPosition.w;

			lightAddDesc.vPosition = _float4(lightAddDesc.vPosition.x, lightAddDesc.vPosition.y, lightAddDesc.vPosition.z, lightAddDesc.vPosition.w);
		}
		

		CLightObj::LIGHTOBJ_DESC		lightObjDesc;
		lightObjDesc.vStartPos = startPos;
		lightObjDesc.tLightDesc = lightAddDesc;

		m_LightObjects.push_back(dynamic_cast<CLightObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_LightObj"), &lightObjDesc)));
		m_pGameInstance->Add_Light(lightAddDesc);

		m_bLightAdd = false;

		/* 추가되면 추가된 index로 이동 */
		m_iCurrentLightIndex = m_LightObjects.size() - 1;

		Update_LightsName();

		return true;
	}

	return false;
}

void CLightTool_Mgr::Delete_Light()
{
	m_pGameInstance->Delete_Light(m_iCurrentLightIndex);

	vector<class CLightObj*>::iterator	iter = m_LightObjects.begin();

	for (int i = 0; i < m_iCurrentLightIndex; i++)
	{
		iter++;
	}

	Safe_Release(*iter);
	m_LightObjects.erase(iter);

	/* 인덱스 옮겨주기 */
	if (0 > m_iCurrentLightIndex - 1)
	{
		m_iCurrentLightIndex = 0;
	}
	else
	{
		m_iCurrentLightIndex -= 1;
	}

	Update_LightsName();

}

void CLightTool_Mgr::Delete_AllLights()
{
	m_iCurrentLightIndex = 0;

	m_pGameInstance->Delete_AllLights();

	for (auto& iter : m_LightObjects)
		Safe_Release(iter);
	m_LightObjects.clear();

	Update_LightsName();
}

void CLightTool_Mgr::Show_Installed_LightsList()
{
	ImGui::NewLine();

	/* 저장 번호 */
	static int iSaveNum = 70;
	ImGui::InputInt(u8"저장번호 : ", &iSaveNum);

	if (ImGui::Button(u8"라이트 저장"))
	{
		Save_Lights_InFile(iSaveNum);
		Update_FileName();
	}

	/* LightDesc 특성 선택 */
	if (0 < m_LightObjects.size())
	{
		static int light_current_idx = 0;

		if (light_current_idx != m_iCurrentLightIndex)
		{
			light_current_idx = m_iCurrentLightIndex;
			m_tCurLightDesc = m_LightObjects[m_iCurrentLightIndex]->Get_LightDesc();
		}

		if (m_bUpdateInfrom == true)
		{
			m_tCurLightDesc = m_LightObjects[m_iCurrentLightIndex]->Get_LightDesc();
			m_bUpdateInfrom = false;
		}

		if (ImGui::BeginListBox("listbox 0"))
		{
			for (int n = 0; n < m_LightsName.size(); n++)
			{
				/*const bool is_selected = (cell_current_idx == n);*/
				const bool is_selected = (light_current_idx == n);
				if (ImGui::Selectable(m_LightsName[n], is_selected))
				{
					/*cell_current_idx = n;
					m_iCurrentCellIndex = cell_current_idx;*/
					light_current_idx = n;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		Click_To_Select_Light(light_current_idx);

		/* 다른 오브젝트 클릭시 */
		if (m_iCurrentLightIndex != light_current_idx && m_LightObjects.size() > 0)
		{
			m_iCurrentLightIndex = light_current_idx;
			m_tCurLightDesc = m_LightObjects[m_iCurrentLightIndex]->Get_LightDesc();
		}


	}


	ImGui::NewLine();

	if (!m_LightObjects.empty())
	{
		Edit_GameObject_Transform(m_iCurrentLightIndex);

		Edit_Installed_Light();
		
	}


	
}

void CLightTool_Mgr::Update_LightsName()
{
	if (0 <= m_LightsName.size())
	{
		for (auto& iter : m_LightsName)
			Safe_Delete(iter);

		m_LightsName.clear();
	}

	if (0 < m_LightObjects.size())
	{
		for (int i = 0; i < m_LightObjects.size(); i++)
		{
			m_LightObjects[i]->Set_ObjID(i+1);
			char* szName = new char[MAX_PATH];
			strcpy(szName, "LightObject");
			char buff[MAX_PATH];
			sprintf(buff, "%d", i);
			strcat(szName, buff);
			m_LightsName.push_back(szName);
		}
	}
}

void CLightTool_Mgr::Edit_Light()
{
	/* 맵 툴 상 light 수정 */
	m_LightObjects[m_iCurrentLightIndex]->Set_LightDesc(m_tCurLightDesc);

	/* gameinstance에서 수정 */
	m_pGameInstance->Edit_Light(m_iCurrentLightIndex, m_tCurLightDesc);
}

void CLightTool_Mgr::Initialize_Light_Environment()
{
	Delete_AllLights();
}

void CLightTool_Mgr::Edit_GameObject_Transform(int iNumObject)
{

	vector<CLightObj*>::iterator iter = m_LightObjects.begin();

	if (0 != iNumObject)
	{
		for (int i = 0; i < iNumObject; i++)
		{
			iter++;
		}
	}

	EditTransformL((float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW),
		(float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ),
		(float*)dynamic_cast<CGameObject*>(*iter)->Get_TransformCom()->Get_WorldFloat4x4(),
		true);
}

void CLightTool_Mgr::Edit_Installed_Light()
{
	/* 배치한 라이트 수정*/

	ImGui::Text(u8"라이트 종류");
	static int lightType = m_tCurLightDesc.eType;
	if (ImGui::RadioButton(u8"TYPE_DIRECTIONAL", m_tCurLightDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL))
	{
		lightType = LIGHT_DESC::TYPE_DIRECTIONAL;
		m_tCurLightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"TYPE_POINT", m_tCurLightDesc.eType == LIGHT_DESC::TYPE_POINT))
	{
		lightType = LIGHT_DESC::TYPE_POINT;
		m_tCurLightDesc.eType = LIGHT_DESC::TYPE_POINT;
	}

	ImGui::NewLine();

	ImGui::Text(u8"라이트 방향");
	ImGui::InputFloat4(u8"라이트 방향", reinterpret_cast<float*>(&m_tCurLightDesc.vDirection));
	/*ImGui::InputFloat("x", &m_tCurLightDesc.vDirection.x);
	ImGui::InputFloat("y", &m_tCurLightDesc.vDirection.y);
	ImGui::InputFloat("z", &m_tCurLightDesc.vDirection.z);
	ImGui::InputFloat("w", &m_tCurLightDesc.vDirection.w);*/

	ImGui::NewLine();


	ImGui::Text(u8"라이트 위치 : 이건 만지지 말기!!!!!!!!!!! 위치는 위에서만 배치..");
	/* 위치에 따른 구조체 수정 */
	XMStoreFloat4(&m_tCurLightDesc.vPosition, m_LightObjects[m_iCurrentLightIndex]->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	ImGui::InputFloat4(u8"라이트 위치", reinterpret_cast<float*>(&m_tCurLightDesc.vPosition));


	ImGui::NewLine();

	ImGui::Text(u8"라이트 범위");
	ImGui::InputFloat("x", &m_tCurLightDesc.fRange);

	ImGui::NewLine();
	//ImGui::ColorEdit4(u8"vDiffuse 선택", (float*)&m_tCurLightDesc.vDiffuse);
	static ImVec4 vDiffuseColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
	if (ImGui::ColorEdit4(u8"vDiffuse 선택", (float*)&m_tCurLightDesc.vDiffuse, ImGuiColorEditFlags_Float))
	{
		//m_tCurLightDesc.vDiffuse = _float4(vDiffuseColor.x, vDiffuseColor.y, vDiffuseColor.z, vDiffuseColor.w);
		//m_tCurLightDesc.vDiffuse = _float4(vDiffuseColor.x, vDiffuseColor.y, vDiffuseColor.z, vDiffuseColor.w);
	}

	ImGui::NewLine();
	/*ImGui::ColorEdit4(u8"vAmbient 선택", (float*)&m_tCurLightDesc.vAmbient);*/
	static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
	if (ImGui::ColorEdit4(u8"vAmbient 선택", (float*)&m_tCurLightDesc.vAmbient, ImGuiColorEditFlags_Float))
	{
		//m_tCurLightDesc.vAmbient = _float4(vAmbientColor.x, vAmbientColor.y, vAmbientColor.z, vAmbientColor.w);
	}

	ImGui::NewLine();
	/*ImGui::ColorEdit4(u8"vSpecular 선택", (float*)&m_tCurLightDesc.vSpecular);*/
	static ImVec4 vSpecularColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
	if (ImGui::ColorEdit4(u8"vSpecular 선택", (float*)&m_tCurLightDesc.vSpecular, ImGuiColorEditFlags_Float))
	{
		//m_tCurLightDesc.vSpecular = _float4(vSpecularColor.x, vSpecularColor.y, vSpecularColor.z, vSpecularColor.w);
	}


	ImGui::Text(u8"라이트 추가 : 추가버튼 누른후 왼쪽 마우스 클릭 ");
	//if (ImGui::Button(u8"라이트 추가"))
	//{
	//	m_bLightAdd = true;
	//}

	Edit_Light();
	/*if (ImGui::Button(u8"라이트 수정"))
	{
		if (0 < m_LightObjects.size())
			Edit_Light();
	}*/


	if (ImGui::Button(u8"라이트 선택 삭제"))
	{
		if (0 < m_LightObjects.size())
			Delete_Light();
	}

	if (ImGui::Button(u8"라이트 전체 삭제"))
	{
		if (0 < m_LightObjects.size())
			Delete_AllLights();
	}
}

void CLightTool_Mgr::Click_To_Select_Light(int& iObjectNum)
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

void CLightTool_Mgr::Free()
{
	for (auto& iter : m_LightObjects)
		Safe_Release(iter);
	m_LightObjects.clear();

	for (auto& iter : m_LightsName)
		Safe_Delete(iter);
	m_LightsName.clear();

	for (auto& iter : m_FileNames)
		Safe_Delete(iter);
	m_FileNames.clear();

	Safe_Release(m_pGameInstance);
}



