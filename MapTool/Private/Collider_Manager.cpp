#include "Collider_Manager.h"
#include "GameInstance.h"

#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"
#include "SystemManager.h"


#include <iostream>
#include <io.h>

bool useWindowC = false;
int gizmoCountC = 1;
static ImGuizmo::OPERATION mCurrentGizmoOperationC(ImGuizmo::TRANSLATE);


// TRANSFORM의 MATRIX
void EditTransformC(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
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
			mCurrentGizmoOperationC = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperationC = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperationC = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperationC == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperationC = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperationC == ImGuizmo::ROTATE))
			mCurrentGizmoOperationC = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperationC == ImGuizmo::SCALE))
			mCurrentGizmoOperationC = ImGuizmo::SCALE;
		/* if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			 mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;*/
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperationC != ImGuizmo::SCALE)
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

		switch (mCurrentGizmoOperationC)
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
	if (useWindowC)
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
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperationC, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	// 화면 상단 카메라 돌아가는애
	//ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);


	if (useWindowC)
	{
		ImGui::End();
		// 이거해보기 터지면
		ImGui::PopStyleColor(1);
	}
}


IMPLEMENT_SINGLETON(CCollider_Manager)

CCollider_Manager::CCollider_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollider_Manager::Initialize()
{
	m_WorldMatrix = XMMatrixIdentity();
	return S_OK;
}

void CCollider_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CCollider_Manager::Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	for (auto& pCollider : m_pColliders)
//		pCollider->Tick(m_WorldMatrix);
//#endif

	for (auto& pCollider : m_pColliders)
		pCollider->Tick(fTimeDelta);

}

void CCollider_Manager::Late_Tick(_float fTimeDelta)
{
	for (auto& pCollider : m_pColliders)
		pCollider->Late_Tick(fTimeDelta);
}

void CCollider_Manager::Render()
{

//#ifdef _DEBUG
//	for (auto& pCollider : m_pColliders)
//		m_pGameInstance->Add_DebugComponent(pCollider);
//#endif
}

void CCollider_Manager::Show_FileName()
{
	ImGui::Text(u8" 콜라이더 파일 리스트 ");

	if (m_FileNames.empty())
	{
		Update_FileName();
	}

	static int layer_current_idx = 0;
	if (ImGui::BeginListBox(u8"콜라이더 파일들"))
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

	/* 저장 번호 */
	static int iSaveNum = 70;
	ImGui::InputInt(u8"저장번호 : ", &iSaveNum);

	if (ImGui::Button(u8"콜라이더 저장"))
	{
		Save_Collider_InFile(iSaveNum);
		Update_FileName();
	}

	if (ImGui::Button(u8"콜라이더 파일 로드") && m_FileNames.size() > 0)
	{
		//로드
		Load_Collider_File(layer_current_idx);
	}


	ImGui::Text(u8" pick로 찾을땐 f 누르기 ");
	ImGui::Text(u8" 배치 오브젝트 리스트 ");

	list<string>	m_layer;

	static int object_current_idx = 0;

	if (m_iCurentColliderIndex != object_current_idx)
	{
		/* 밑에서 수정이 생겼을 경우 */
		object_current_idx = m_iCurentColliderIndex;
	}

	if (ImGui::BeginListBox("listbox 1"))
	{
		for (int n = 0; n < m_ColliderNames.size(); n++)
		{
			const bool is_selected = (object_current_idx == n);
			if (ImGui::Selectable(m_ColliderNames[n], is_selected))
				object_current_idx = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	Click_To_Select_Object(object_current_idx);

	if (object_current_idx != m_iCurentColliderIndex)
	{
		m_iCurentColliderIndex = object_current_idx;
	}


	if (ImGui::Button(u8"콜라이더 선택 삭제"))
	{
		if (0 < m_pColliders.size())
			Delete_Collider(object_current_idx);
	}


	if (ImGui::Button(u8"콜라이더 전체 삭제"))
	{
		if (0 < m_pColliders.size())
			Delete_AllCollider();
	}


	if (0 < m_pColliders.size())
	{
		Edit_Collider_Transform(m_iCurentColliderIndex);
	}


	/* 다른 오브젝트 클릭시 */
	//if (m_iCurentColliderIndex != object_current_idx)
	//{
	//	m_iCurentColliderIndex = object_current_idx;

	//}



}

void CCollider_Manager::Add_Collider_IMGUI()
{
	ImGui::Begin(u8" 콜라이더");

	// 추가하는 버튼
	Add_Collider_In_Manager();

	ImGui::End();
}

void CCollider_Manager::Save_Collider_InFile(int iSaveNum)
{
	COLLIDER_IO		colliderIO;

	colliderIO.iColliderNum = m_pColliders.size();
	colliderIO.pColliderDesc = new COLLIDER_DESC[m_pColliders.size()];

	int iIndex = 0;

	for (auto& iter : m_pColliders)
	{
		colliderIO.pColliderDesc[iIndex] = m_pColliders[iIndex]->Get_ColliderDesc();
		iIndex++;
	}

	Export_Bin_Collider_Data(&colliderIO, iSaveNum);

	Safe_Delete_Array(colliderIO.pColliderDesc);
}

void CCollider_Manager::Load_Collider_File(int iNum)
{
	Delete_AllCollider();

	COLLIDER_IO		colliderIODesc;
	Import_Bin_Collider_Data_OnTool(&colliderIODesc, m_FileNames[iNum]);

	for (int i = 0; i < colliderIODesc.iColliderNum; i++)
	{
		COLLIDER_DESC		colliderDesc;
		colliderDesc = colliderIODesc.pColliderDesc[i];

		CColliderObj::COLLIDEROBJ_DESC		colliderObjDesc;
		colliderObjDesc.bLoad = true;
		colliderObjDesc.vStartPos = XMLoadFloat4x4(&colliderDesc.vTransform);
		colliderObjDesc.vCenter = colliderDesc.vCenter;
		colliderObjDesc.vExtents = colliderDesc.vExtents;
		colliderObjDesc.vQuaternion = colliderDesc.vQuaternion;
		colliderObjDesc.vStartPos = XMLoadFloat4x4(&colliderDesc.vTransform);
		m_pColliders.push_back(dynamic_cast<CColliderObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_ColliderObj"), &colliderObjDesc)));
	}


	Update_ColliderNameList();

	m_iCurentColliderIndex = 0;

	Safe_Delete_Array(colliderIODesc.pColliderDesc);
}

HRESULT CCollider_Manager::Import_Bin_Collider_Data_OnTool(COLLIDER_IO* ColliderData, char* fileName)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/ColliderData/");

	strcat_s(fullPath, fileName);

	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}


	in.read((char*)&ColliderData->iColliderNum, sizeof(int));

	ColliderData->pColliderDesc = new COLLIDER_DESC[ColliderData->iColliderNum];

	for (int i = 0; i < ColliderData->iColliderNum ; i++)
	{
		in.read((char*)&ColliderData->pColliderDesc[i].vTransform, sizeof(XMFLOAT4X4));
		in.read((char*)&ColliderData->pColliderDesc[i].vCenter, sizeof(_float3));
		in.read((char*)&ColliderData->pColliderDesc[i].vExtents, sizeof(_float3));
		in.read((char*)&ColliderData->pColliderDesc[i].vQuaternion, sizeof(_float3));
	}

	in.close();

	return S_OK;
}

HRESULT CCollider_Manager::Export_Bin_Collider_Data(COLLIDER_IO* ColliderData, int iSaveNum)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/ColliderData/");

	strcat_s(fullPath, "Collider_Data");

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

	/* collider 개수 저장 */
	int iNumColliderNum = ColliderData->iColliderNum;
	out.write((char*)&iNumColliderNum, sizeof(int));



	for (int i = 0; i < iNumColliderNum; i++)
	{
		COLLIDER_DESC pColliderDesc = ColliderData->pColliderDesc[i];
		out.write((char*)&pColliderDesc.vTransform, sizeof(XMFLOAT4X4));
		out.write((char*)&pColliderDesc.vCenter, sizeof(_float3));
		out.write((char*)&pColliderDesc.vExtents, sizeof(_float3));
		out.write((char*)&pColliderDesc.vQuaternion, sizeof(_float3));
	}

	out.close();

	return S_OK;
}

bool CCollider_Manager::Add_Collider_In_Manager()
{
	if (ImGui::Button(u8"콜라이더 추가"))
	{
		m_bColliderAdd = true;
	}

	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB) && m_bColliderAdd == true)
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

		CColliderObj::COLLIDEROBJ_DESC		colliderInstallDesc;

		colliderInstallDesc.vStartPos = startPos;
		colliderInstallDesc.bLoad = false;
		m_pColliders.push_back(dynamic_cast<CColliderObj*>(CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_ColliderObj"), &colliderInstallDesc)));

		m_bColliderAdd = false;

		/* 추가되면 추가된 index로 이동 */
		//m_iCurrentLightIndex = m_LightObjects.size() - 1;

		Update_ColliderNameList();

		return true;
	}

	return false;
}

void CCollider_Manager::Update_FileName()
{
	for (auto& iter : m_FileNames)
		Safe_Delete(iter);

	m_FileNames.clear();
	string path = "../../Client/Bin/DataFiles/ColliderData/*.dat";

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

void CCollider_Manager::Load_Colliders_File(int iNum)
{
}

void CCollider_Manager::Delete_Collider(int iNumCollider)
{
	vector<CColliderObj*>::iterator	iter = m_pColliders.begin();

	for (int i = 0; i < iNumCollider; i++)
	{
		iter++;
	}

	Safe_Release(*iter);
	m_pColliders.erase(iter);

	/* 인덱스 옮겨주기 */
	if (0 > m_iCurentColliderIndex - 1)
	{
		m_iCurentColliderIndex = 0;
	}
	else
	{
		m_iCurentColliderIndex -= 1;
	}

	Update_ColliderNameList();
}

void CCollider_Manager::Delete_AllCollider()
{
	m_iCurentColliderIndex = 0;

	for (auto& iter : m_pColliders)
		Safe_Release(iter);
	m_pColliders.clear();

	Update_ColliderNameList();
}

void CCollider_Manager::Update_ColliderNameList()
{
	if (0 <= m_ColliderNames.size())
	{
		for (auto& iter : m_ColliderNames)
			Safe_Delete(iter);

		m_ColliderNames.clear();
	}
	
	int iIndex = 0;

	if (0 < m_pColliders.size())
	{
		for (auto& iter : m_pColliders)
		{
			iter->Set_ObjID(iIndex + 1);
			char* szName = new char[MAX_PATH];
			strcpy(szName, "Collider");
			char buff[MAX_PATH];
			sprintf(buff, "%d", iIndex);
			strcat(szName, buff);
			m_ColliderNames.push_back(szName);

			iIndex++;
		}
	}
}

void CCollider_Manager::Edit_Collider_Transform(int iColliderNum)
{
	vector<CColliderObj*>::iterator iter = m_pColliders.begin();

	if (0 != iColliderNum)
	{
		for (int i = 0; i < iColliderNum; i++)
		{
			iter++;
		}
	}


	

	EditTransformC((float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW),
		(float*)CGameInstance::GetInstance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ),
		(float*)dynamic_cast<CColliderObj*>(*iter)->Get_ColliderWorldMatrix(),
		true);
}

void CCollider_Manager::Click_To_Select_Object(int& iObjectNum)
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

void CCollider_Manager::Free()
{
	for (auto& iter : m_pColliders)
		Safe_Release(iter);
	m_pColliders.clear();

	Safe_Release(m_pGameInstance);
}
