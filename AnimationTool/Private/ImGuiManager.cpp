#include "ImguiManager.h"
#include "GameInstance.h"

#include "AnimModel.h"
#include "ModelBoneSphere.h"
#include "TRailEffect.h"

#include "ImGuiFileDialog.h"

#pragma region "Model"
#include "Animation.h"
#include "Anim.h"
#include "Bone.h"
#include "Channel.h"
#include "Mesh.h"
#pragma endregion

wstring g_wstrModelName;

CImguiManager::CImguiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImguiManager::Initialize(void* pArg)
{
	Setting_InitialData("Player/");

	//ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &(ImGui::GetIO()); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io->Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\malgun.ttf", 18.0f, NULL, io->Fonts->GetGlyphRangesKorean());

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	fs::path projectRootDir = fs::current_path();
	fs::path ParentDir = projectRootDir.parent_path();
	m_RootDir = ParentDir.parent_path();
	return S_OK;
}

void CImguiManager::Tick(const _float& fTimeDelta)
{
	if (LEVEL_EDIT == m_pGameInstance->Get_CurrentLevel())
		Connect_Model_Ref();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ModelList();

	ImGui::SameLine();
	if (ImGui::Button("Open"))
		m_isOnToolWindows = !m_isOnToolWindows;

	if (m_isOnToolWindows)
	{
		AnimListWindow();
		BoneListWindow();
		KeyFrameWindow();
		MeshListWindow();
		AnimEventWindow();

		if (m_isEffectListWindow)
			EffectListWindow();
		if (m_isSoundListWindow)
			SoundListWindow();
		if (m_isRimLightWindow)
			RimLightWindow();
		if (m_isTrailWindow)
			TrailWindow();
		if (m_isFaceWindow)
			FaceEventWindow();
		if (m_isBloodWindow)
			BloodEventWindow();
		if (m_isRadialEventWindow)
			RadialEventWindow();
	}

	Play_SoundEvent();

	// 애니메이션 이벤트가 있는 콜라이더의 색상을 시안색으로 바꿔주는 기능
	if (m_pGameInstance->Get_CurrentLevel() != LEVEL_LOADING)
	{
		if (0 < m_AnimNameList.size())
		{
			auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

			// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
			if (lower_bound_iter != m_AnimationEvents.end() && (*lower_bound_iter).first != m_AnimNameList[m_iAnimIndex])
				return;

			auto upper_bound_iter = m_AnimationEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

			auto pBoneSpheres = m_pRenderModel->Get_BoneSpheres();

			for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
			{
				Animation_Event Value = (*lower_bound_iter).second;

				CModel* pModel = static_cast<CModel*>(m_pRenderModel->Get_Model());
				if (nullptr != pModel)
				{
					_double Position = 0.0;
					if(m_iModelType == PLAYER)
						Position = *pModel->Get_AnimationCurrentPosition();
					else
						Position = *pModel->Get_AnimationCurrentPosition(m_pRenderModel->Get_AnimComponent());

					if (Value.iType == COLLIDER_ACTIVATION && Value.fAinmPosition < Position)
					{
						pBoneSpheres[Value.iBoneIndex]->Change_ColliderColor(true);
					}
					if (Value.iType == COLLIDER_DISABLE && Value.fAinmPosition < Position)
					{
						pBoneSpheres[Value.iBoneIndex]->Change_ColliderColor(false);
					}
				}
			}
		}

	}

}

HRESULT CImguiManager::Render()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	return S_OK;
}

void CImguiManager::ModelList()
{
	if (ImGui::RadioButton(u8"플레이어 모델", m_iModelPathType == PLAYER))
	{
		m_iModelPathType = PLAYER;
		Setting_ModelList("Player/");
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"몬스터 모델", m_iModelPathType == ENEMY))
	{
		m_iModelPathType = ENEMY;
		Setting_ModelList("Monster/");
	}	
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"NPC 모델", m_iModelPathType == NPC))
	{
		m_iModelPathType = NPC;
		Setting_ModelList("NPC/");
	}


	ImGui::Text(u8"적 체크 시 AnimComponent의 애니메이션 목록으로 설정한다");
	if (ImGui::RadioButton(u8"모델 내 애니메이션", m_iModelType == MODEL_ANIMS))
	{
		m_iModelType = MODEL_ANIMS;

		Change_Model();

		m_Anims = m_pRenderModel->Get_Animations();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"애님 컴", m_iModelType == ANIM_COM))
	{
		m_iModelType = ANIM_COM;

		Change_Model();

		auto pAnimCom = m_pRenderModel->Get_AnimComponent();
		m_Anims = pAnimCom->Get_Animations();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"싱크액션 애님컴", m_iModelType == SYNC_COM))
	{
		m_iModelType = SYNC_COM;

		Change_Model();

		auto pAnimCom = m_pRenderModel->Get_AnimComponent();
		m_Anims = pAnimCom->Get_Animations();
	}

	if (ImGui::DragFloat3("Position", m_ModelPosition, 0.1f))
	{
		Update_Model_Position();
	}

	if (ImGui::DragFloat("Rotation X", &m_ModelRotation[0], 10.f))
	{
		Update_Model_RotationX();

		m_ModelRotation[0] = 0.f;
	}

	if (ImGui::DragFloat("Rotation Y", &m_ModelRotation[1], 10.f))
	{
		Update_Model_RotationY();

		m_ModelRotation[1] = 0.f;
	}

	if (ImGui::DragFloat("Rotation Z", &m_ModelRotation[2], 10.f))
	{
		Update_Model_RotationZ();

		m_ModelRotation[2] = 0.f;
	}

	if (ImGui::DragFloat("All Scale", &m_ModelScale, 0.001f))
	{
		if (0.001f > m_ModelScale) m_ModelScale = 0.001f;

		Update_Model_Scaled();
	}

	ImGui::NewLine();
	ImGui::Text("Model List");

	vector<const char*> items;

	if (0 < m_ModelNameList.size()) {
		for (const string& str : m_ModelNameList) {
			items.push_back(str.c_str());
		}
	}

	if (ImGui::ListBox("##", &m_iModelSelectedIndex, items.data(), m_ModelNameList.size()))
	{
		Change_Model();
	}

	//if (ImGui::Button(u8"데이터 저장하기"))
	//{
	//	All_Save();
	//}
	if (ImGui::Button(u8"데이터 로드하기"))
	{
		All_Load();
	}

}

void CImguiManager::AnimListWindow()
{
	ImGui::Begin("Anim List", &m_isOnToolWindows);

	m_AnimNameList.clear();
	m_AnimNameList.resize(m_Anims.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pAnim : m_Anims)
	{
		string strChannelName = m_pGameInstance->Extract_String(pAnim->Get_AnimName(), '[', ']');

		m_AnimNameList[i] = strChannelName;
		items.push_back(m_AnimNameList[i].c_str());

		i++;
	}

	ImGui::Text("Animation List");
	if (ImGui::ListBox("##", &m_iAnimIndex, items.data(), items.size()))
	{
		m_fAnimationPosition = 0.f;
		m_pRenderModel->Change_Animation(m_iAnimIndex, m_iModelType == PLAYER ? false : true);
		//m_isAnimLoop = m_pRenderModel->Get_AnimLoop(m_iAnimIndex);
	}


	ImGui::InputInt("Search Anim Index", &m_iSearchAnimIndex);
	if (ImGui::Button(u8"애니메이션 인덱스 검색하기"))
		m_iAnimIndex = m_iSearchAnimIndex;

	ImGui::SameLine();
	ImGui::Text("Anim Index: %d", m_iAnimIndex);

	if (ImGui::Button("Add"))
	{
		if (m_AddedAnims.find(m_iAnimIndex) == m_AddedAnims.end())
		{
			m_AddedAnims.emplace(m_iAnimIndex, m_AnimNameList[m_iAnimIndex]);
		}
	}

	vector<const char*> Addeditems;
	for (auto& AddedAnim : m_AddedAnims)
		Addeditems.push_back(AddedAnim.second.c_str());

	ImGui::ListBox("Loop Animations", &m_iAddedAnimSelectedIndex, Addeditems.data(), Addeditems.size());

	if (ImGui::Button(u8"루프 애니메이션 목록 저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		AnimationLoop_Save(strDirectory);
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		auto iter = m_AddedAnims.begin();

		for (size_t i = 0; i < m_iAddedAnimSelectedIndex; i++)
		{
			iter++;
		}

		m_AddedAnims.erase(iter);
	}


	ImGui::End();

}

void CImguiManager::BoneListWindow()
{
	ImGui::Begin("Bone List", &m_isOnToolWindows);

	m_BoneNameList.clear();
	const vector<CBone*> pBones = m_pRenderModel->Get_Bones();
	m_BoneNameList.resize(pBones.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pBone : pBones)
	{
		string strChannelName = pBone->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pBone->Get_Name(), '[', ']');

		m_BoneNameList[i] = strChannelName;
		items.push_back(m_BoneNameList[i].c_str());

		i++;
	}

	ImGui::InputText("Bone Name Search", m_szSearchBoneName, sizeof(m_szSearchBoneName) * _MAX_PATH);

	if (ImGui::Button(u8"뼈 이름 검색하기"))
	{
		for (size_t i = 0; i < m_BoneNameList.size(); i++)
		{
			if (m_BoneNameList[i] == string(m_szSearchBoneName))
				m_iBoneSelectedIndex = i;
		}

		Gui_Select_Bone(m_iBoneSelectedIndex);
	}

	//뼈 리스트
	if (ImGui::ListBox("##", &m_iBoneSelectedIndex, items.data(), items.size()))
	{
		Gui_Select_Bone(m_iBoneSelectedIndex);
	}

	if (ImGui::RadioButton("AABB", m_iColliderType == AABB))
	{
		m_iColliderType = AABB;
		Reset_Collider_Value();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("OBB", m_iColliderType == OBB))
	{
		m_iColliderType = OBB;
		Reset_Collider_Value();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Sphere", m_iColliderType == SPHERE))
	{
		m_iColliderType = SPHERE;
		Reset_Collider_Value();
	}

	if (ImGui::DragFloat3("Position", m_ColliderPosition, 0.1f))
	{
		_float3 vPosition = _float3(m_ColliderPosition[0], m_ColliderPosition[1], m_ColliderPosition[2]);
		m_pRenderModel->Set_Collider_Center(m_iBoneSelectedIndex, vPosition);
	}


	switch (m_iColliderType)
	{
	case AABB:
	{
		if (ImGui::DragFloat3("Extents", m_ColliderExtents, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, m_ColliderExtents);
		break;
	}
	case OBB:
	{
		if (ImGui::DragFloat3("Extents", m_ColliderExtents, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, m_ColliderExtents);
		break;
	}
	case SPHERE:
	{
		if (ImGui::DragFloat("Radius", &m_fColliderRadius, 0.1f))
			m_pRenderModel->Set_Collider_Value(m_iBoneSelectedIndex, &m_fColliderRadius);
		break;
	}
	}

	if (m_iColliderActionType == HIT)
		m_isAlwaysCollider = true;
	else
		m_isAlwaysCollider = false;

	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::Checkbox(u8"상시", &m_isAlwaysCollider);
	ImGui::PopItemFlag();
	ImGui::SameLine();

	if (ImGui::RadioButton("Attack", m_iColliderActionType == ATTACK))
	{
		m_iColliderActionType = ATTACK;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Hit", m_iColliderActionType == HIT))
	{
		m_iColliderActionType = HIT;
	}

	switch (m_iColliderActionType)
	{
	case ATTACK:
		ImGui::Text(u8"공격용 콜라이더 타입");
		if (ImGui::RadioButton("Hand", m_iColliderPartType == HAND_A))
		{
			m_iColliderPartType = HAND_A;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Foot", m_iColliderPartType == FOOT_A))
		{
			m_iColliderPartType = FOOT_A;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Joint", m_iColliderPartType == JOINT_A))
		{
			m_iColliderPartType = JOINT_A;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Head", m_iColliderPartType == HEAD_A))
		{
			m_iColliderPartType = HEAD_A;
		}
		break;
	case HIT:

		ImGui::Text(u8"피격용 콜라이더 타입");
		if (ImGui::RadioButton("Head(Neck)", m_iColliderPartType == HEAD_H))
		{
			m_iColliderPartType = HEAD_H;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Body", m_iColliderPartType == BODY_H))
		{
			m_iColliderPartType = BODY_H;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Leg", m_iColliderPartType == LEG_H))
		{
			m_iColliderPartType = LEG_H;
		}
		break;
	}

	if (ImGui::Button("Create Collier"))
	{
		void* pValue = nullptr;
		switch (m_iColliderType)
		{
		case AABB:
			pValue = &m_ColliderExtents;
			break;
		case OBB:
			pValue = &m_ColliderExtents;
			break;
		case SPHERE:
			pValue = &m_fColliderRadius;
			break;
		}

		_float3 vCenter;
		memcpy(&vCenter, &m_ColliderPosition, sizeof(_float3));
		if (!m_pRenderModel->Created_BoneCollider(m_iBoneSelectedIndex))
		{
			m_AddedColliders.emplace(m_iBoneSelectedIndex, Collider_State{ m_BoneNameList[m_iBoneSelectedIndex], m_iColliderActionType,  m_iColliderPartType, m_isAlwaysCollider });
			m_pRenderModel->Create_BoneCollider(m_iColliderType, m_iBoneSelectedIndex, vCenter, pValue);
		}
	}

	vector<const char*> Addeditems;

	for (auto& iter : m_AddedColliders)
	{
		Addeditems.push_back(iter.second.strBoneName.c_str());
	}

	if (ImGui::ListBox(u8"추가된 콜라이더 리스트", &m_iColliderSelectedIndex, Addeditems.data(), Addeditems.size()))
	{
		auto iter = m_AddedColliders.begin();

		for (size_t i = 0; i < m_iColliderSelectedIndex; i++)
		{
			iter++;
		}

		Gui_Select_Bone((*iter).first);
		m_isAlwaysCollider = (*iter).second.isAlways;
		m_iColliderActionType = (*iter).second.iColliderActionType;
		m_iColliderPartType = (*iter).second.iColliderPartType;
	}

	if (ImGui::Button(u8"콜라이더 삭제"))
	{
		auto iter = m_AddedColliders.begin();

		for (size_t i = 0; i < m_iColliderSelectedIndex; i++)
		{
			iter++;
		}

		m_pRenderModel->Release_BoneCollider((*iter).first);
		m_AddedColliders.erase((*iter).first);		// 지울 때에는 뼈인덱스를 기준으로 지워야하기 때문에 m_iColliderSelectedIndex가 아니라 m_iBoneSelectedIndex로 지워야한다.
	}

	if (ImGui::Button(u8"콜라이더 정보 저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		ColliderState_Save(strDirectory);
	}

	ImGui::End();
}


void CImguiManager::MeshListWindow()
{
	ImGui::Begin("Mesh List", &m_isOnToolWindows);

	m_MeshNameList.clear();
	const vector<CMesh*> pMeshes = m_pRenderModel->Get_Meshes();
	m_MeshNameList.resize(pMeshes.size());

	_uint i = 0;

	vector<const char*> items;

	for (auto pMesh : pMeshes)
	{
		string strChannelName = pMesh->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pMesh->Get_Name(), '[', ']');

		m_MeshNameList[i] = strChannelName;
		items.push_back(m_MeshNameList[i].c_str());

		i++;
	}

	ImGui::Text("Mesh List");
	if (ImGui::ListBox("##", &m_iMeshSelectedIndex, items.data(), items.size()))
	{
		m_pRenderModel->Select_Mesh(m_iMeshSelectedIndex);
	}

	if (ImGui::Button("Add"))
	{
		m_AddedMeshes.emplace(m_iMeshSelectedIndex, m_MeshNameList[m_iMeshSelectedIndex]);
	}
	ImGui::SameLine();
	// 블랜드 기능 적용하는 버튼
	if (ImGui::Button("Apply"))
	{
		for (auto& AddedMesh : m_AddedMeshes)
		{
			pMeshes[AddedMesh.first]->Set_AlphaApply(true);
		}
	}

	//추가한 메시 리스트
	vector<const char*> Addeditems;
	for (auto& AddedMesh : m_AddedMeshes)
	{
		Addeditems.push_back(AddedMesh.second.c_str());
	}
	ImGui::ListBox("Added Meshes", &m_iAddedMeshSelectedIndex, Addeditems.data(), Addeditems.size());

	if (ImGui::Button(u8"추가한 메시 저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		AlphaMesh_Save(strDirectory);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		auto iter = m_AddedMeshes.begin();

		for (size_t i = 0; i < m_iAddedMeshSelectedIndex; i++)
		{
			iter++;
		}

		m_AddedMeshes.erase(iter);
	}

	ImGui::End();
}

void CImguiManager::KeyFrameWindow()
{
	if (m_Anims.size() < 1) return;

	ImGui::Begin("KeyFrame", NULL);

	auto Anims = m_Anims;

	_float Duration = (_float)(*(Anims[m_iAnimIndex]->Get_Duration()));
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));

	// 애니메이션 재생 위치를 보여준다.
	ImGui::SliderFloat("##", &CurrentPosition, 0.f, Duration);

	Anims[m_iAnimIndex]->Set_CurrentPosition((_double)CurrentPosition);
	Anims[m_iAnimIndex]->Update_KeyframeIndex();

#pragma region 애니메이션 재생 관련 버튼
	if (ImGui::Button("Stop/Play"))
	{
		m_isPause = !m_isPause;

		if (m_isPause)
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), 0.f);
		else
			m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fTimeDeltaScale);
	}
	ImGui::SameLine();

	if (ImGui::SliderFloat("Speed Scale", &m_fTimeDeltaScale, 0.f, 3.f))
	{
		m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fTimeDeltaScale);
	}
	ImGui::SameLine();

#pragma endregion

	//현재 애니메이션에 맞는 채널들을 보여준다
	DrawChannels();

	// 이벤트를 추가할 애니메이션 포지션
	ImGui::InputFloat("Animation Position", &m_fAnimationPosition);

#pragma region 콜라이더 이벤트 버튼들
	ImGui::Text("Collider Event");
	if (ImGui::Button("Collider Activation"))
	{
		auto Anims = m_Anims;
		auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

		if (m_pRenderModel->Created_BoneCollider(Channels[m_iChannelSelectedIndex]->Get_BoneIndex()))
		{
			Animation_Event Event{ COLLIDER_ACTIVATION, m_fAnimationPosition, m_ChannelNameList[m_iChannelSelectedIndex], m_iBoneSelectedIndex };

			m_AnimationEvents.emplace(m_AnimNameList[m_iAnimIndex], Event);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Collider Disable"))
	{
		auto Anims = m_Anims;
		auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

		if (m_pRenderModel->Created_BoneCollider(Channels[m_iChannelSelectedIndex]->Get_BoneIndex()))
		{
			Animation_Event Event{ COLLIDER_DISABLE, m_fAnimationPosition, m_ChannelNameList[m_iChannelSelectedIndex], m_iBoneSelectedIndex };

			m_AnimationEvents.emplace(m_AnimNameList[m_iAnimIndex], Event);
		}
	}
#pragma endregion

#pragma region 이펙트 이벤트 버튼들
	ImGui::Text("Effect Event");
	if (ImGui::Button("Add Effect"))
	{
		m_isEffectListWindow = true;
	}
#pragma endregion

#pragma region 사운드 이벤트 버튼
	ImGui::Text("Sound Event");
	if (ImGui::Button("Add Sound"))
	{
		m_isSoundListWindow = true;
	}
#pragma endregion

#pragma region 림라이트 이벤트 버튼
	ImGui::Text(u8"개별 이벤트들");
	if (ImGui::Button("RimLight Op"))
	{
		m_isRimLightWindow = true;
	}
#pragma endregion

#pragma region 트레일 이벤트 버튼
	ImGui::SameLine();
	if (ImGui::Button("Trail Op"))
	{
		m_isTrailWindow = true;
	}
#pragma endregion

#pragma region 페이스 이벤트 버튼
	if (ImGui::Button("Face Op"))
	{
		m_isFaceWindow = true;
	}
#pragma endregion

#pragma region 블러드 이펙트 이벤트 버튼
	ImGui::SameLine();
	if (ImGui::Button("Blood Op"))
	{
		m_isBloodWindow = true;
	}
#pragma endregion


#pragma region 래디얼 이펙트 이벤트 버튼
	ImGui::SameLine();
	if (ImGui::Button("Radiul Op"))
	{
		m_isRadialEventWindow = true;
	}
#pragma endregion

	if (ImGui::Button(u8"애니메이션 이벤트 저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		AnimationEvent_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::AnimEventWindow()
{
	if (m_AnimNameList.size() < 1) return;

	ImGui::Begin("Animation Events", NULL);

	vector<_uint> EventTypes;
	vector<const char*> items;
	for (auto& Event : m_AnimationEvents)
	{
		if (Event.first == m_AnimNameList[m_iAnimIndex])
			EventTypes.push_back(Event.second.iType);
	}
	for (auto& iEventType : EventTypes)
	{
		switch (iEventType)
		{
		case 0:
			items.push_back("Collider On");
			break;
		case 1:
			items.push_back("Collider Off");
			break;
		case 2:
			items.push_back("Sound On");
			break;
		}
	}

	if (ImGui::ListBox("##", &m_iEventSelectedIndex, items.data(), items.size()))
	{
		// 모든 이벤트들의 Selected를 끈다
		for (auto& Event : m_AnimationEvents)
			Event.second.isSelected = false;

		// 현재 선택된 애니메이션 이름을 key로 가진 원소의 첫번째를 찾아서
		auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

		//선택된 이벤트만큼 이터 증가시키고
		for (size_t i = 0; i < m_iEventSelectedIndex; i++)
			lower_bound_iter++;

		// 찾은 원소를 선택한 것으로 변경한다
		(*lower_bound_iter).second.isSelected = true;
	}

	ImGui::SameLine();
	if (ImGui::Button("Event Delete"))
	{
		auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

		for (size_t i = 0; i < m_iEventSelectedIndex; i++)
			lower_bound_iter++;

		m_AnimationEvents.erase(lower_bound_iter);
	}

	ImGui::Text("Animation Name : %s", m_AnimNameList[m_iAnimIndex].c_str());

	auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_AnimationEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		Animation_Event Value = (*lower_bound_iter).second;
		if (Value.isSelected) m_iEventBoneIndex = Value.iBoneIndex;
	}

	if (-1 < m_iEventBoneIndex)
		ImGui::Text("[Bone Info]\nIndex: %i Name: %s", m_iEventBoneIndex, m_BoneNameList[m_iEventBoneIndex].c_str());
	else
		ImGui::Text("[Bone Info]\nNone");

	ImGui::Text(u8"Bone Info가 None이라면 선택된 이벤트가 없거나\n본에 영향을 받지 않는 이벤트를 선택한 상태");

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	DrawTimeline(draw_list);

	ImGui::End();
}

void CImguiManager::EffectListWindow()
{
	ImGui::Begin("Effect List", &m_isEffectListWindow);

	if (ImGui::Checkbox(u8"이펙트 끄기", &m_isEffectOff))
	{
		if (m_isEffectOff)
		{
			auto pGameObjects = m_pGameInstance->Get_GameObjects(LEVEL_EDIT, TEXT("Layer_Effect"));
			for (auto& pObject : pGameObjects)
			{
				static_cast<CEffect*>(pObject)->Set_Off(true);
			}
		}
		else
		{
			auto pGameObjects = m_pGameInstance->Get_GameObjects(LEVEL_EDIT, TEXT("Layer_Effect"));
			for (auto& pObject : pGameObjects)
			{
				static_cast<CEffect*>(pObject)->Set_Off(false);
			}
		}

	}

	for (size_t i = 0; i < m_EffectTypeList.size(); i++)
	{
		ImGui::SameLine();
		if (ImGui::RadioButton(m_EffectTypeList[i].c_str(), m_iEffectType == i))
		{
			m_iEffectType = i;
		}
	}

	//추가한 메시 리스트
	auto lower_bound_iter = m_EffectFiles.lower_bound(m_iEffectType);
	auto upper_bound_iter = m_EffectFiles.upper_bound(m_iEffectType);

	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.c_str());
	}

	ImGui::ListBox("##", &m_iEffectSelectedIndex, items.data(), items.size());
	m_SelectedEffectName = m_pGameInstance->Get_FileName(items[m_iEffectSelectedIndex]);

	if (ImGui::Button(u8"이펙트 생성"))
	{
		Create_Effect(m_BoneNameList[m_iBoneSelectedIndex], m_SelectedEffectName);
	}


	vector<const char*> Addeditems;
	for (auto& strEffect : m_EffectState)
		Addeditems.push_back(strEffect.second.data());

	auto iter = m_EffectState.begin();
	for (size_t i = 0; i < m_iAddedEffectSelectedIndex; i++)	iter++;

	if (0 < Addeditems.size())
	{
		if (iter != m_EffectState.end())
			ImGui::Text(u8"대상 뼈 이름: %s", (*iter).first.c_str());
	}
	else
		ImGui::Text(u8"대상 뼈 이름: None");

	ImGui::ListBox("Added Effects", &m_iAddedEffectSelectedIndex, Addeditems.data(), Addeditems.size());
	if (ImGui::Button(u8"이펙트 삭제"))
	{
		auto iter = m_EffectState.begin();
		for (size_t i = 0; i < m_iAddedEffectSelectedIndex; i++)
		{
			iter++;
		}

		auto pEffectList = m_pGameInstance->Get_GameObjects(LEVEL_EDIT, TEXT("Layer_Effect"));
		
		for (auto pEffect : pEffectList)
		{
			//pEffect == (*iter).first
		}

		CEffect* pEffect = reinterpret_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_EDIT, TEXT("Layer_Effect"), m_iAddedEffectSelectedIndex));
		pEffect->Set_Dead();

		m_EffectState.erase(iter);
	}

	if (ImGui::Button(u8"이펙트 정보 저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

		EffectState_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::RimLightWindow()
{
	ImGui::Begin(u8"림라이트 설정 창", &m_isRimLightWindow);

	ImGui::Text(u8"선택된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::Text(u8"선택된 메시: %s", m_MeshNameList[m_iMeshSelectedIndex].c_str());

	ImGui::Text(u8"현재 입력된 애니메이션 포지션: %f", m_fAnimationPosition);

	if (ImGui::Button(u8"림라이트 On"))
	{
		Animation_RimLightState RimLightState{};
		RimLightState.iType = 0;					
		RimLightState.fAinmPosition = m_fAnimationPosition;
		RimLightState.strMeshName = m_MeshNameList[m_iMeshSelectedIndex];

		m_RimLightEvents.emplace(m_AnimNameList[m_iAnimIndex], RimLightState);
	}

	if (ImGui::Button(u8"림라이트 Off"))
	{
		Animation_RimLightState RimLightState{};
		RimLightState.iType = 1;
		RimLightState.fAinmPosition = m_fAnimationPosition;
		RimLightState.strMeshName = m_MeshNameList[m_iMeshSelectedIndex];

		m_RimLightEvents.emplace(m_AnimNameList[m_iAnimIndex], RimLightState);
	}

	//림라이트 이벤트 리스트
	auto lower_bound_iter = m_RimLightEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_RimLightEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	ImGui::Text(u8"림라이트 이벤트 리스트");
	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.strMeshName.c_str());
	}

	ImGui::ListBox("##", &m_iRimLightEventIndex, items.data(), items.size());

	auto lower_iter = m_RimLightEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	for (size_t i = 0; i < m_iRimLightEventIndex; i++)
	{
		lower_iter++;
	}

	if (lower_iter != upper_bound_iter && lower_iter != m_RimLightEvents.end())
	{
		if ((*lower_iter).second.iType == 0)
		{
			ImGui::Text(u8"On 이벤트");

			ImGui::Text(u8"저장된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
			ImGui::Text(u8"저장된 메시: %s", (*lower_iter).second.strMeshName.c_str());

			ImGui::Text(u8"저장된 애니메이션 포지션: %f", (*lower_iter).second.fAinmPosition);
		}
		else
		{
			ImGui::Text(u8"Off 이벤트");

			ImGui::Text(u8"저장된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
			ImGui::Text(u8"저장된 메시: %s", (*lower_iter).second.strMeshName.c_str());

			ImGui::Text(u8"저장된 애니메이션 포지션: %f", (*lower_iter).second.fAinmPosition);
		}
	}

	// 림라이트 적용할 메시 이름 갱신하는 함수
	Setting_RimLight();

	if (ImGui::Button(u8"삭제"))
	{
		auto iterator = m_RimLightEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

		for (size_t i = 0; i < m_iTrailEventIndex; i++)
		{
			iterator++;
		}
		m_RimLightEvents.erase(iterator);
	}

	if (ImGui::Button(u8"저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		RimEvent_Save(strDirectory);
	}
		

	ImGui::End();
}

void CImguiManager::TrailWindow()
{
	ImGui::Begin(u8"트레일 설정 창", &m_isTrailWindow);

	ImGui::Text(u8"선택된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::Text(u8"선택된 본(채널리스트에서 선택한 값): %s", m_ChannelNameList[m_iChannelSelectedIndex].c_str());

	ImGui::Text(u8"현재 입력된 애니메이션 포지션: %f", m_fAnimationPosition);

	if (ImGui::Button(u8"트레일 On"))
	{
		Animation_TrailState TrailState{};
		TrailState.iType = 0;
		TrailState.fAinmPosition = m_fAnimationPosition;
		TrailState.strBonelName = m_ChannelNameList[m_iChannelSelectedIndex];
		TrailState.strTrailProtoName = m_SelectedEffectName;

		auto& Channels = m_Anims[m_iAnimIndex]->Get_Channels();
		TrailState.iBoneIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

		m_TrailEvents.emplace(m_AnimNameList[m_iAnimIndex], TrailState);

		Create_Effect(m_BoneNameList[m_iBoneSelectedIndex], m_SelectedEffectName, TEXT("Layer_Effect"));
	}

	if (ImGui::Button(u8"트레일 Off"))
	{
		Animation_TrailState TrailState{};
		TrailState.iType = 1;
		TrailState.fAinmPosition = m_fAnimationPosition;
		TrailState.strBonelName = m_ChannelNameList[m_iChannelSelectedIndex];
		TrailState.strTrailProtoName = m_SelectedEffectName;

		auto& Channels = m_Anims[m_iAnimIndex]->Get_Channels();
		TrailState.iBoneIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

		m_TrailEvents.emplace(m_AnimNameList[m_iAnimIndex], TrailState);
	}

	//림라이트 이벤트 리스트
	auto lower_bound_iter = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (!(lower_bound_iter != m_TrailEvents.end() && (*lower_bound_iter).first != m_AnimNameList[m_iAnimIndex]))
	{
		auto upper_bound_iter = m_TrailEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

		ImGui::Text(u8"트레일 이벤트 리스트");
		vector<const char*> items;
		for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
		{
			items.push_back((*lower_bound_iter).second.strBonelName.c_str());
		}

		ImGui::ListBox("##", &m_iTrailEventIndex, items.data(), items.size());

		auto lower_iter = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

		for (size_t i = 0; i < m_iTrailEventIndex; i++)
		{
			if(lower_iter != m_TrailEvents.end())
				lower_iter++;
			else
			{
				MSG_BOX("트레일 이벤트 목록에서 iterator가 end입니다. 다시 선택해주세요.");
				return;
			}
		}

		if (lower_iter != upper_bound_iter && lower_iter != m_TrailEvents.end())
		{
			if ((*lower_iter).second.iType == 0)
			{
				ImGui::Text(u8"On 이벤트");

				ImGui::Text(u8"저장된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
				ImGui::Text(u8"저장된 뼈 이름: %s", (*lower_iter).second.strBonelName.c_str());
				ImGui::Text(u8"저장된 뼈 인덱스: %i", (*lower_iter).second.iBoneIndex);

				ImGui::Text(u8"저장된 애니메이션 포지션: %f", (*lower_iter).second.fAinmPosition);
			}
			else
			{
				ImGui::Text(u8"Off 이벤트");

				ImGui::Text(u8"저장된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
				ImGui::Text(u8"저장된 뼈 이름: %s", (*lower_iter).second.strBonelName.c_str());
				ImGui::Text(u8"저장된 뼈 인덱스: %i", (*lower_iter).second.iBoneIndex);

				ImGui::Text(u8"저장된 애니메이션 포지션: %f", (*lower_iter).second.fAinmPosition);
			}
		}

		// 트레일 온/오프 갱신 함수
		Setting_Trail();

		if (ImGui::Button(u8"삭제"))
		{
			auto iterator = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

			for (size_t i = 0; i < m_iTrailEventIndex; i++)
			{
				iterator++;
			}
			m_TrailEvents.erase(iterator);
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"저장"))
		{
			string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
			TrailEvent_Save(strDirectory);
		}
	}

	ImGui::End();
}

void CImguiManager::SoundListWindow()
{
	ImGui::Begin("Sound List", &m_isSoundListWindow);

	if (ImGui::Button(u8"사운드 파일 불러오기"))
	{
		IGFD::FileDialogConfig config;
		config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Sounds" ).string();
		ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryGroupSoundFiles", "Choose File", ".wav, .mp3", config);
	}

	if (ImGuiFileDialog::Instance()->Display("ChooseBinaryGroupSoundFiles"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			m_strSelectSoundFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

			ImGuiFileDialog::Instance()->Close();
		}
	}

	if (ImGui::Button(u8"사운드 듣기"))
		m_pGameInstance->PlaySound_W(m_pGameInstance->StringToWstring(ImGuiFileDialog::Instance()->GetCurrentFileName()), m_eSoundCannel, m_fSoundVolume);
	ImGui::SameLine();
	if (ImGui::Button(u8"사운드 끄기"))
		m_pGameInstance->StopAll();

	ImGui::Checkbox(u8"컷신용", &m_isCutSceneEvents);

	ImGui::InputFloat(u8"사운드 볼륨", &m_fSoundVolume);

	/*
	* 		Animation_SoundEventState Desc{};
			Desc.eChannel = m_eSoundCannel;
			Desc.fAinmPosition = m_fAnimationPosition;
			Desc.eChannel = m_eSoundCannel;


			m_SoundEvents.emplace()
	*/
	ImGui::Text(u8"선택된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::InputFloat(u8"애니메이션 포지션", &m_fAnimationPosition);
	ImGui::Text(u8"선택된 사운드 파일: %s", m_strSelectSoundFileName.c_str());

	/*
	* 	enum CHANNELID {
		SOUND_EFFECT,
		SOUND_BGM,
		SOUND_AMBIENT,
		SOUND_PLAYER_VOICE,
		SOUND_PLAYER_EFFECT,
		SOUND_ENEMY_VOICE,
		SOUND_ENEMY_EFFECT,
		SOUND_UI,
		SOUND_END
	};
	*/

	ImGui::Text(u8"사운드 채널 종류");
	if (ImGui::RadioButton(u8"EFFECT", m_eSoundCannel == SOUND_EFFECT))
	{
		m_eSoundCannel = SOUND_EFFECT;
	}
	if (ImGui::RadioButton(u8"BGM", m_eSoundCannel == SOUND_BGM))
	{
		m_eSoundCannel = SOUND_BGM;
	}
	if (ImGui::RadioButton(u8"AMBIENT", m_eSoundCannel == SOUND_AMBIENT))
	{
		m_eSoundCannel = SOUND_AMBIENT;
	}
	if (ImGui::RadioButton(u8"PLAYER_VOICE", m_eSoundCannel == SOUND_PLAYER_VOICE))
	{
		m_eSoundCannel = SOUND_PLAYER_VOICE;
	}
	if (ImGui::RadioButton(u8"PLAYER_EFFECT", m_eSoundCannel == SOUND_PLAYER_EFFECT))
	{
		m_eSoundCannel = SOUND_PLAYER_EFFECT;
	}
	if (ImGui::RadioButton(u8"ENEMY_VOICE", m_eSoundCannel == SOUND_ENEMY_VOICE))
	{ 
		m_eSoundCannel = SOUND_ENEMY_VOICE;
	}
	if (ImGui::RadioButton(u8"ENEMY_EFFECT", m_eSoundCannel == SOUND_ENEMY_EFFECT))
	{
		m_eSoundCannel = SOUND_ENEMY_EFFECT;
	}

	if (ImGui::Button(u8"사운드 추가하기"))
	{
		Animation_SoundEventState Desc{};

		Desc.iChannel = static_cast<_uint>(m_eSoundCannel);
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.fSoundVolume = m_fSoundVolume;
		Desc.strSoundFileName = m_strSelectSoundFileName;

		m_SoundEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}

	ImGui::Text(u8"사운드 이벤트 리스트");

	auto upper_bound_iter = m_SoundEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);
	auto lower_bound_iter = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.strSoundFileName.c_str());
	}

	if (ImGui::ListBox("##", &m_iSoundEventIndex, items.data(), items.size()))
	{
		auto lower_bound_iter2 = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iSoundEventIndex; i++)
		{
			lower_bound_iter2++;
		}

		m_strSelectSoundFileName = lower_bound_iter2->second.strSoundFileName;
		m_fAnimationPosition = lower_bound_iter2->second.fAinmPosition;
		m_fSoundVolume = lower_bound_iter2->second.fSoundVolume;
	}

	if (ImGui::Button(u8"불러오기"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		SoundEvent_Load(strDirectory);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"삭제"))
	{
		auto iterator = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iSoundEventIndex; i++)
		{
			iterator++;
		}
		m_SoundEvents.erase(iterator);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		SoundEvent_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::FaceEventWindow()
{
	ImGui::Begin("Face Event", &m_isFaceWindow);

	ImGui::Text(u8"[0] f201_special_01 총격전에서 로켓 격파하는 컷신에서 쓰임");
	ImGui::Text(u8"[1] f202_special_02 인상씀(화난)");
	ImGui::Text(u8"[2] f203_special_03 인상씀(뭐야저건 이런표정) 황당해하는듯");
	ImGui::Text(u8"[3] f204_special_04 인상쓰고있는데 오묘하게 웃음");
	ImGui::Text(u8"[4] f205_special_05 뭔지모르겠는 오묘한 표정");
	ImGui::Text(u8"[5] f206_special_06 미심쩍게 웃음");
	ImGui::Text(u8"[6] f207_special_07 아파할 때 쓰일듯?");
	ImGui::Text(u8"[7] f208_special_08 뭔가 안쓰럽게보는거같아보임");
	ImGui::Text(u8"[8] f209_special_09 크게 화낼 때");
	ImGui::Text(u8"[9] f210_special_10 눈 부릅뜨는");
	ImGui::Text(u8"[10] f211_special_11 인상쓰고 실눈뜨고봄");
	ImGui::Text(u8"[11] f212_special_12 눈 크게뜸 (놀란듯)\n");
		

	ImGui::Text(u8"선택된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::InputFloat(u8"애니메이션 포지션", &m_fAnimationPosition);

	ImGui::InputInt(u8"표정 인덱스", &m_iFaceAnimIndex);

	if (ImGui::Button(u8"표정 On"))
	{
		Animation_FaceEventState Desc{};
		Desc.iType = 0;
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.iFaceAnimIndex = m_iFaceAnimIndex;

		m_FaceEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}
	if (ImGui::Button(u8"표정 Off"))
	{
		Animation_FaceEventState Desc{};
		Desc.iType = 1;
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.iFaceAnimIndex = m_iFaceAnimIndex;

		m_FaceEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}
	if (ImGui::Button(u8"표정 Change"))
	{
		Animation_FaceEventState Desc{};
		Desc.iType = 2;
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.iFaceAnimIndex = m_iFaceAnimIndex;

		m_FaceEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}

	ImGui::Text(u8"표정 이벤트 리스트");

	auto upper_bound_iter = m_FaceEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);
	auto lower_bound_iter = m_FaceEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.iType == 0 ? "On" : ((*lower_bound_iter).second.iType == 1 ? "Off" : "Change"));
	}

	if (ImGui::ListBox("##", &m_iFaceEventIndex, items.data(), items.size()))
	{
		auto lower_bound_iter2 = m_FaceEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iFaceEventIndex; i++)
		{
			lower_bound_iter2++;
		}

		m_iFaceAnimIndex = lower_bound_iter2->second.iFaceAnimIndex;
		m_fAnimationPosition = lower_bound_iter2->second.fAinmPosition;
	}

	if (ImGui::Button(u8"불러오기"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		FaceEvent_Load(strDirectory);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"삭제"))
	{
		auto iterator = m_FaceEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iFaceEventIndex; i++)
		{
			iterator++;
		}
		m_FaceEvents.erase(iterator);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		FaceEvent_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::BloodEventWindow()
{
	ImGui::Begin("Blood Particle Event", &m_isBloodWindow);

	ImGui::Text(u8"선택된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::Text(u8"선택된 본(본 리스트에서 선택한 값): %s", m_BoneNameList[m_iBoneSelectedIndex].c_str());

	ImGui::InputFloat(u8"애니메이션 포지션", &m_fAnimationPosition);

	ImGui::Text(u8"파싱할 때 정해둔 타입을 int형으로 입력한다.");
	ImGui::Text(u8"0: 코, 1: 입, 2: ...");
	ImGui::InputInt(u8"출력할 이펙트 타입: ", &m_eBloodEffectType);

	if(m_isBloodEffectOn)
		ImGui::Text(u8"선택한건 On 이벤트");
	else
		ImGui::Text(u8"선택한건 Off 이벤트");

	if (ImGui::RadioButton(u8"루프 O", m_isBloodEffectLoop))
	{
		m_isBloodEffectLoop = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"루프 X", !m_isBloodEffectLoop))
	{
		m_isBloodEffectLoop = false;
	}

	if (ImGui::Button(u8"코"))
	{
		m_iBoneSelectedIndex = 99;
		Gui_Select_Bone(99);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"입"))
	{
		m_iBoneSelectedIndex = 89;
		Gui_Select_Bone(89);
	}

	if (ImGui::Button(u8"피 이펙트 On"))
	{
		Animation_BloodEventState Desc{};
		Desc.fAinmPosition = m_fAnimationPosition;
		Desc.iBoneIndex = m_iBoneSelectedIndex;
		Desc.strBonelName = m_BoneNameList[m_iBoneSelectedIndex];
		Desc.iBloodEffectType = m_eBloodEffectType;
		Desc.isLoop = m_isBloodEffectLoop;
		Desc.isOn = true;
		m_isBloodEffectOn = true;

		m_BloodEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}
	if (m_isBloodEffectLoop)
	{
		if (ImGui::Button(u8"피 이펙트 Off"))
		{
			Animation_BloodEventState Desc{};
			Desc.fAinmPosition = m_fAnimationPosition;
			Desc.iBoneIndex = m_iBoneSelectedIndex;
			Desc.strBonelName = m_BoneNameList[m_iBoneSelectedIndex];
			Desc.iBloodEffectType = m_eBloodEffectType;
			Desc.isLoop = m_isBloodEffectLoop;
			Desc.isOn = false;
			m_isBloodEffectOn = false;

			m_BloodEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
		}
	}

	auto upper_bound_iter = m_BloodEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);
	auto lower_bound_iter = m_BloodEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	ImGui::Text(u8"피 이펙트 이벤트 리스트");
	vector<string> itemStrings; // std::string을 사용하여 문자열 저장
	vector<const char*> items; // const char* 리스트

	itemStrings.reserve(distance(lower_bound_iter, upper_bound_iter));
	items.reserve(distance(lower_bound_iter, upper_bound_iter));

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		itemStrings.push_back(to_string((*lower_bound_iter).second.iBloodEffectType)); // 정수를 문자열로 변환하여 저장
		items.push_back(itemStrings.back().c_str()); // 변환된 문자열을 리스트에 추가
	}

	if (ImGui::ListBox(u8"##", &m_iBloodEventIndex, items.data(), items.size()))
	{
		auto lower_bound_iter2 = m_BloodEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iBloodEventIndex; i++)
		{
			lower_bound_iter2++;
		}

		m_fAnimationPosition = lower_bound_iter2->second.fAinmPosition;
		Gui_Select_Bone(lower_bound_iter2->second.iBoneIndex);
		m_isBloodEffectLoop = lower_bound_iter2->second.isLoop;
		m_eBloodEffectType = lower_bound_iter2->second.iBloodEffectType;
		m_isBloodEffectOn = lower_bound_iter2->second.isOn;
	}

	if (ImGui::Button(u8"불러오기"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		BloodEvent_Load(strDirectory);
	}
	if (ImGui::Button(u8"삭제"))
	{
		auto iterator = m_BloodEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iBloodEventIndex; i++)
		{
			iterator++;
		}
		m_BloodEvents.erase(iterator);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		BloodEvent_Save(strDirectory);
	}


	ImGui::End();
}

void CImguiManager::RadialEventWindow()
{
	ImGui::Begin("Radial Event", &m_isRadialEventWindow);

	ImGui::Text(u8"선택된 애니메이션: %s", m_AnimNameList[m_iAnimIndex].c_str());
	ImGui::Text(u8"현재 입력된 애니메이션 포지션: %f", m_fAnimationPosition);

	auto upper_bound_iter = m_RadialEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);
	auto lower_bound_iter = m_RadialEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);

	ImGui::Text(u8"레디얼블러 이벤트 리스트");
	vector<const char*> items;
	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		items.push_back((*lower_bound_iter).second.iType == 0 ? "On" : "Off");
	}

	if (ImGui::ListBox("##", &m_iRadialEventIndex, items.data(), items.size()))
	{
		auto lower_bound_iter2 = m_RadialEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iRadialEventIndex; i++)
		{
			lower_bound_iter2++;
		}

		m_fRadialForce = lower_bound_iter2->second.fForce;
		m_fAnimationPosition = lower_bound_iter2->second.fAinmPosition;
	}

	ImGui::InputFloat(u8"레디얼블러 강도", &m_fRadialForce);
	ImGui::InputFloat(u8"애니메이션 포지션", &m_fAnimationPosition);

	if (ImGui::Button(u8"레디얼 On"))
	{
		Animation_RadialEventState Desc{};
		Desc.iType = 0;
		Desc.fForce = m_fRadialForce;
		Desc.fAinmPosition = m_fAnimationPosition;

		m_RadialEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}
	if (ImGui::Button(u8"레디얼 Off"))
	{
		Animation_RadialEventState Desc{};
		Desc.iType = 1;
		Desc.fForce = 0.f;
		Desc.fAinmPosition = m_fAnimationPosition;

		m_RadialEvents.emplace(m_AnimNameList[m_iAnimIndex], Desc);
	}

	if (ImGui::Button(u8"불러오기"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		RadialEvent_Load(strDirectory);
	}
	if (ImGui::Button(u8"삭제"))
	{
		auto iterator = m_RadialEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
		for (size_t i = 0; i < m_iRadialEventIndex; i++)
		{
			iterator++;
		}
		m_RadialEvents.erase(iterator);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"저장"))
	{
		string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];
		RadialEvent_Save(strDirectory);
	}

	ImGui::End();
}

void CImguiManager::DrawTimeline(ImDrawList* draw_list)
{
	ImVec2 vCanvas_Start = ImGui::GetCursorScreenPos();			// 시작 위치	
	ImVec2 vCanvas_Size = ImGui::GetContentRegionAvail();		// 크기	
	ImVec2 vCanvas_End = vCanvas_Start + vCanvas_Size;

	draw_list->AddRectFilled(vCanvas_Start, vCanvas_End, IM_COL32(50, 50, 50, 255));

	_float fCircleRadius = 5.f;

	auto lower_bound_iter = m_AnimationEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_AnimationEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	_float vCircleStartPosY = vCanvas_Start.y + fCircleRadius * 2.f;
	_float vCircleStartPosX = vCanvas_Start.x + fCircleRadius * 4.f;
	_float vPosY = 0.f;

	//콜라이더 활성화(노랑), 콜라이더 비활성화(주황), 사운드 활성화(초록), 이펙트 활성화(파랑)
	ImU32 CircleColor = IM_COL32_WHITE;

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		Animation_Event Value = (*lower_bound_iter).second;

		//vCanvas_Size
		auto Anims = m_Anims;

		_float fDuration = _float(*(Anims[m_iAnimIndex]->Get_Duration()));

		_float fRatio = Value.fAinmPosition / fDuration;

		vCanvas_Size.x* fRatio;

		if (Value.isSelected)
		{
			CircleColor = IM_COL32(255, 0, 0, 255);
		}
		else
		{
			switch (Value.iType)
			{
			case COLLIDER_ACTIVATION:
				CircleColor = IM_COL32(255, 255, 0, 255);
				break;
			case COLLIDER_DISABLE:
				CircleColor = IM_COL32(255, 127, 0, 255);
				break;
			case SOUND_ACTIVATION:
				CircleColor = IM_COL32(0, 255, 0, 255);
				break;
			}
		}


		vPosY = vCircleStartPosY + (Value.iType * fCircleRadius * 2.5f);

		draw_list->AddCircleFilled(ImVec2(vCanvas_Start.x + vCanvas_Size.x * fRatio, vPosY), fCircleRadius, CircleColor);
	}
}

void CImguiManager::DrawChannels()
{
	auto Anims = m_Anims;
	auto& Channels = Anims[m_iAnimIndex]->Get_Channels();

	m_ChannelNameList.clear();
	m_ChannelNameList.resize(Channels.size());

	ImGui::InputText("Channel Name", m_szSearchChannelName, sizeof(m_szSearchChannelName) * _MAX_PATH);

	_uint i = 0;

	vector<const char*> items;

	for (auto pChannel : Channels)
	{
		string strChannelName = pChannel->Get_Name();
		//string strName = m_pGameInstance->Extract_String(pBone->Get_Name(), '[', ']');

		m_ChannelNameList[i] = strChannelName;
		items.push_back(m_ChannelNameList[i].c_str());

		i++;
	}

	ImGui::InputText("Channel Name", m_szSearchChannelName, sizeof(m_szSearchChannelName) * _MAX_PATH);

	if (ImGui::Button(u8"채널(=뼈) 이름 검색하기"))
	{
		for (size_t i = 0; i < m_ChannelNameList.size(); i++)
		{
			if (m_ChannelNameList[i] == string(m_szSearchChannelName))
			{
				m_iChannelSelectedIndex = i;

				m_iBoneSelectedIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

				Gui_Select_Bone(m_iBoneSelectedIndex);
				break;
			}
		}
	}

	ImGui::ScrollToItem(ImGuiScrollFlags_AlwaysCenterY);
	if (ImGui::ListBox("##", &m_iChannelSelectedIndex, items.data(), items.size()))
	{
		m_iBoneSelectedIndex = Channels[m_iChannelSelectedIndex]->Get_BoneIndex();

		m_pRenderModel->Select_Bone(m_iBoneSelectedIndex);
	}
}

void CImguiManager::Connect_Model_Ref()
{
	m_pRenderModel = static_cast<CAnimModel*>(m_pGameInstance->Get_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), 0));
}

string CImguiManager::Get_FirstModel_Name()
{
	return m_ModelNameList.front();
}

void CImguiManager::Update_Model_Position()
{
	_vector vPosition = XMVectorSet(m_ModelPosition[0], m_ModelPosition[1], m_ModelPosition[2], 1.f);
	m_pRenderModel->Set_Position(vPosition);
}

void CImguiManager::Update_Model_RotationX()
{
	m_pRenderModel->Set_Rotation(0, XMConvertToRadians(m_ModelRotation[0]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_Edit")));
}

void CImguiManager::Update_Model_RotationY()
{
	m_pRenderModel->Set_Rotation(1, XMConvertToRadians(m_ModelRotation[1]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_Edit")));
}

void CImguiManager::Update_Model_RotationZ()
{
	m_pRenderModel->Set_Rotation(2, XMConvertToRadians(m_ModelRotation[2]), m_pGameInstance->Get_TimeDelta(TEXT("Timer_Edit")));
}

void CImguiManager::Update_Model_Scaled()
{
	m_pRenderModel->Set_Scaled(m_ModelScale, m_ModelScale, m_ModelScale);
}

void CImguiManager::Reset_Collider_Value()
{
	ZeroMemory(m_ColliderPosition, sizeof(float) * 3);
	ZeroMemory(m_ColliderExtents, sizeof(float) * 3);
	m_fColliderRadius = 0.f;
}

void CImguiManager::Clear_EffectStateMap()
{
	auto pEffectLayer = m_pGameInstance->Get_GameObjects(LEVEL_EDIT, TEXT("Layer_Effect"));

	for (auto& pEffect : pEffectLayer)
		pEffect->Set_Dead();

	m_EffectState.clear();
}

void CImguiManager::Create_Effect(string& strBoneName, string& strEffectName, wstring wstrLayer)
{
	auto& pBones = m_pRenderModel->Get_Bones();

	const _float4x4* pBoneMatrix = { nullptr };

	for (auto& pBone : pBones)
	{
		if (pBone->Compare_Name(strBoneName.c_str()))
			pBoneMatrix = pBone->Get_CombinedTransformationMatrix();
	}

	CEffect::EFFECT_DESC Desc{};
	Desc.pWorldMatrix = pBoneMatrix;

	if(wstrLayer == TEXT("Layer_Effect"))
		m_pGameInstance->Add_GameObject(LEVEL_EDIT, m_pGameInstance->StringToWstring(strEffectName), wstrLayer, &Desc);
	else if (wstrLayer == TEXT("Layer_Trail"))
	{
		m_pRenderModel->Create_Trail(strBoneName, m_pGameInstance->Clone_Object(m_pGameInstance->StringToWstring(strEffectName), &Desc));
	}

	m_EffectState.emplace(strBoneName, strEffectName);
}

void CImguiManager::Play_SoundEvent()
{
	if (m_Anims.size() < 1) return;
	if (m_AnimNameList.size() < 1) return;

	auto Anims = m_Anims;
	_float Duration = (_float)(*(Anims[m_iAnimIndex]->Get_Duration()));
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));

	auto lower_bound_iter = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_SoundEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		if (!lower_bound_iter->second.isPlay && CurrentPosition > lower_bound_iter->second.fAinmPosition)
		{
			lower_bound_iter->second.isPlay = true;
			m_pGameInstance->PlaySound_W(m_pGameInstance->StringToWstring(lower_bound_iter->second.strSoundFileName), static_cast<CHANNELID>(lower_bound_iter->second.iChannel), lower_bound_iter->second.fSoundVolume);
		}
	}

	if (Anims[m_iAnimIndex]->Get_Restrat())
		Reset_SoundEvent_Played();
}

void CImguiManager::Reset_SoundEvent_Played()
{
	auto lower_bound_iter = m_SoundEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_SoundEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		lower_bound_iter->second.isPlay = false;
	}
}

void CImguiManager::Setting_Collider_Value(_uint iBoneIndex)
{
	auto& pBoneSpheres = m_pRenderModel->Get_BoneSpheres();

	auto pSphere = pBoneSpheres[iBoneIndex];

	if (!pSphere->Created_Collider())
		return;

	// 선택한 뼈 인덱스를 뼈 리스트에 동기화
	m_iBoneSelectedIndex = iBoneIndex;

	int iType = pSphere->Get_Collider_Type();
	m_iColliderType = iType;

	_float3 vCenter = pSphere->Get_Collider_Center();
	memcpy(&m_ColliderPosition, &vCenter, sizeof(_float3));

	switch (iType)
	{
	case CCollider::COLLIDER_AABB:
	{
		_float3* vExtents = (_float3*)pSphere->Get_Collider_Value();
		memcpy(&m_ColliderExtents, vExtents, sizeof(_float3));
	}
	case CCollider::COLLIDER_OBB:
	{
		_float3* vExtents = (_float3*)pSphere->Get_Collider_Value();
		memcpy(&m_ColliderExtents, vExtents, sizeof(_float3));
	}
	case CCollider::COLLIDER_SPHERE:
	{
		_float* fRadius = (_float*)pSphere->Get_Collider_Value();
		memcpy(&m_fColliderRadius, fRadius, sizeof(_float));
	}
	}
}


void CImguiManager::All_Save()
{
	string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

	fs::path path(strDirectory);
	if (!exists(path))
		fs::create_directory(path);

	/* 모델 매쉬 정보 저장하기 */
	AlphaMesh_Save(strDirectory);

	/* 애니메이션 루프 정보 저장하기 */
	AnimationLoop_Save(strDirectory);

	/* 애니메이션 이벤트 저장하기 */
	AnimationEvent_Save(strDirectory);

	/* 충돌체 생성 정보 저장하기 */
	ColliderState_Save(strDirectory);

	/* 이펙트 생성 정보 저장하기 */
	EffectState_Save(strDirectory);

	/* 림라이트 이벤트 정보 저장하기 */
	RimEvent_Save(strDirectory);

	/* 트레일 이벤트 저장하기 */
	TrailEvent_Save(strDirectory);

	/* 레디얼 이벤트 저장하기 */
	RadialEvent_Save(strDirectory);
}

void CImguiManager::AlphaMesh_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_AlphaMeshes.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iMeshMapSize = m_AddedMeshes.size();

	out.write((char*)&iMeshMapSize, sizeof(_uint));

	// 매쉬 읽어오는 인덱스 순서가 바뀔 일은 없나?
	// 없다고 가정하고 인덱스만 저장하기
	//string strModelName = m_ModelNameList[m_iModelSelectedIndex];

	for (auto& iter : m_AddedMeshes)
	{
		out.write((char*)&(iter.first), sizeof(_uint));
	}

	out.close();
}

void CImguiManager::AnimationLoop_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_LoopAnimations.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iMapSize = m_AddedAnims.size();

	// 추후 코드 수정 필요
	for (const auto& iter : m_AddedAnims)
	{
		_uint iAnimIndex = iter.first;

		auto Anims = m_pRenderModel->Get_Animations();


		// 이름이 같은 애니메이션을 찾아서 key값으로 저장해준다.
		_uint i = 0;
		for (auto& pAnim : Anims)
		{
			string animationName = m_pGameInstance->Extract_String(pAnim->Get_AnimName(), '[', ']');

			if (animationName == iter.second)
			{
				iAnimIndex = i;
				break;
			}

			i++;
		}

		// 현재 애니메이션에 없는건 제거하는 코드 (그만큼 갯수줄임)
		if (i >= Anims.size())
			iMapSize--;
	}

	// iMapSize를 이진 형식으로 저장
	out.write(reinterpret_cast<const char*>(&iMapSize), sizeof(iMapSize));

	for (const auto& iter : m_AddedAnims)
	{
		// 키를 이진 형식으로 저장
		_uint iAnimIndex = iter.first;

		auto Anims = m_pRenderModel->Get_Animations();


		// 이름이 같은 애니메이션을 찾아서 key값으로 저장해준다.
		_uint i = 0;
		for (auto& pAnim : Anims)
		{
			string animationName = m_pGameInstance->Extract_String(pAnim->Get_AnimName(), '[', ']');

			if (animationName == iter.second)
			{
				iAnimIndex = i;
				break;
			}

			i++;
		}

		if (i >= Anims.size()) continue;
		

		out.write(reinterpret_cast<const char*>(&iAnimIndex), sizeof(iAnimIndex));

		// 문자열 길이와 문자열을 이진 형식으로 저장
		const string& strAnimName = iter.second;
		_uint length = strAnimName.size();
		out.write(reinterpret_cast<const char*>(&length), sizeof(length));
		out.write(strAnimName.c_str(), length);
	}

	out.close();
}

void CImguiManager::AnimationEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_AnimationEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iEventMapSize = m_AnimationEvents.size();

	out << iEventMapSize;

	// first: 애니메이션 이름, second: 이벤트정보
	// 어떤 애니메이션에 대한 값들인지를 저장함 (애니메이션 이름을 key로 쓴다)
	for (auto& iter : m_AnimationEvents)
	{
		string strAnimName = iter.first;
		out << strAnimName << endl;

		Animation_Event Desc{};
		Desc.iType = iter.second.iType;
		Desc.fAinmPosition = iter.second.fAinmPosition;
		Desc.strChannelName = iter.second.strChannelName;
		Desc.iBoneIndex = iter.second.iBoneIndex;

		out << Desc.iType << endl;
		out << Desc.fAinmPosition << endl;
		out << Desc.strChannelName << endl;
		out << Desc.iBoneIndex << endl;
	}

	out.close();
}

void CImguiManager::ColliderState_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_Colliders.dat";

	ofstream out(strDirectory, ios::binary);

	auto pBoneSpheres = m_pRenderModel->Get_BoneSpheres();

	_uint iColliderCount = { 0 };

	// 여기서 i는 뼈의 인덱스를 key값으로 쓰려고 구하는 것
	_uint i = { 0 };
	map<_uint, CModelBoneSphere*> Spheres;

	for (auto& pSphere : pBoneSpheres)
	{
		if (pSphere->Created_Collider())
		{
			Spheres.emplace(i, pSphere);
		}

		i++;
	}

	iColliderCount = Spheres.size();

	// 총 몇개의 콜라이더를 읽어올 것인지 작성한다
	out.write((char*)&iColliderCount, sizeof(_uint));

	// 위에서 저장해둔 본스피어에서 콜라이더 정보를 꺼내온다.
	for (auto& pSphere : Spheres)
	{
		CCollider* pCollider = static_cast<CCollider*>(pSphere.second->Get_Component(TEXT("Com_Collider")));

		//읽어온 콜라이더의 타입에 따라 정보를 읽어서 저장한다 (무슨 타입이던 무조건 2줄씩 쓸 것이긴하나, 
		// Sphere 일 경우 _float 단독이고, 나머지는 _float3이므로 로드 시 주의 필요
		CCollider::TYPE eType = pCollider->Get_Type();

		out.write((char*)&pSphere.first, sizeof(_uint));
		// 이넘은 인트처럼 4바이트를 소모하므로, 인트 크기로 저장한다.
		out.write((char*)&eType, sizeof(_uint));

		switch (eType)
		{
		case Engine::CCollider::COLLIDER_AABB:
		{
			BoundingBox* pBounding = static_cast<BoundingBox*>(pCollider->Get_OriginDesc());
			out.write((char*)&pBounding->Center, sizeof(_float3));
			out.write((char*)&pBounding->Extents, sizeof(_float3));
		}
		break;
		case Engine::CCollider::COLLIDER_OBB:
		{
			//BoundingOrientedBox 구조체에는 Orientation(_float4) 값이 있긴 하나, 
			// 따로 조절하지 않을 것이므로 저장하지 않는다.
			BoundingOrientedBox* pBounding = static_cast<BoundingOrientedBox*>(pCollider->Get_OriginDesc());
			out.write((char*)&pBounding->Center, sizeof(_float3));
			out.write((char*)&pBounding->Extents, sizeof(_float3));
		}
		break;
		case Engine::CCollider::COLLIDER_SPHERE:
		{
			BoundingSphere* pBounding = static_cast<BoundingSphere*>(pCollider->Get_OriginDesc());
			out.write((char*)&pBounding->Center, sizeof(_float3));
			out.write((char*)&pBounding->Radius, sizeof(_float));
		}
		break;
		}

		int iColliderActionType = m_AddedColliders.at(pSphere.first).iColliderActionType;
		int iColliderPartType = m_AddedColliders.at(pSphere.first).iColliderPartType;

		out.write((char*)&iColliderActionType, sizeof(int));
		out.write((char*)&iColliderPartType, sizeof(int));
		out.write((char*)&m_AddedColliders.at(pSphere.first).isAlways, sizeof(_bool));
	}

	out.close();
}

void CImguiManager::EffectState_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_EffectState.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumEffectState = m_EffectState.size();
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	out << iNumEffectState;

	for (auto& Effect : m_EffectState)
	{
		out << Effect.first << endl;
		out << Effect.second << endl;
	}

	out.close();
}

void CImguiManager::RimEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_RimEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumRimEvents = m_RimLightEvents.size();
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	out << iNumRimEvents;

	for (auto& pair : m_RimLightEvents)
	{
		out << pair.first << endl;
		out << pair.second.iType << endl;
		out << pair.second.strMeshName << endl;
		out << pair.second.fAinmPosition << endl;
	}

	out.close();
}

void CImguiManager::TrailEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_TrailEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumTrailEvents = m_TrailEvents.size();
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	out << iNumTrailEvents;

	for (auto& pair : m_TrailEvents)
	{
		out << pair.first << endl;
		out << pair.second.iType << endl;
		out << pair.second.strBonelName << endl;
		out << pair.second.strTrailProtoName << endl;
		out << pair.second.iBoneIndex << endl;
		out << pair.second.fAinmPosition << endl;
	}

	out.close();
}

void CImguiManager::SoundEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + (m_isCutSceneEvents ? "_CutSoundEvents.dat" : "_SoundEvents.dat");

	ofstream out(strDirectory, ios::binary);

	_uint iNumEvents = m_SoundEvents.size();
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	out << iNumEvents;

	/*
		_uint iChannel;
		_float fAinmPosition;
		string strSoundFileName;
	*/

	for (auto& pair : m_SoundEvents)
	{
		out << pair.first << endl;
		out << pair.second.iChannel << endl;
		out << pair.second.fAinmPosition << endl;
		out << pair.second.fSoundVolume << endl;
		out << pair.second.strSoundFileName << endl;
	}

	out.close();
}

void CImguiManager::FaceEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_FaceEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumEvents = m_FaceEvents.size();
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	out << iNumEvents;

	/*
		_uint iType;					//0번이 on 1번이 off 2번이 change
		_float fAinmPosition;
		_uint iFaceAnimIndex;
	*/

	for (auto& pair : m_FaceEvents)
	{
		out << pair.first << endl;
		out << pair.second.iType << endl;
		out << pair.second.fAinmPosition << endl;
		out << pair.second.iFaceAnimIndex << endl;
	}

	out.close();
}

void CImguiManager::BloodEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_BloodEffectEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumEvents = m_BloodEvents.size();
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	out << iNumEvents;

	/*
		_float fAinmPosition;
		_uint iBoneIndex;
		string strBonelName;
		_uint iBloodEffectType;		// 출력시킬 이펙트의 타입
		_bool isLoop;		// 출력시킬 이펙트의 타입
	*/

	for (auto& pair : m_BloodEvents)
	{
		out << pair.first << endl;
		out << pair.second.fAinmPosition << endl;
		out << pair.second.iBoneIndex << endl;
		out << pair.second.strBonelName << endl;
		out << pair.second.iBloodEffectType << endl;
		out << pair.second.isLoop << endl;
		out << pair.second.isOn << endl;
	}

	out.close();
}

void CImguiManager::RadialEvent_Save(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_RadialEvents.dat";

	ofstream out(strDirectory, ios::binary);

	_uint iNumEvents = m_RadialEvents.size();
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	out << iNumEvents;

	for (auto& pair : m_RadialEvents)
	{
		out << pair.first << endl;
		out << pair.second.iType << endl;
		out << pair.second.fAinmPosition << endl;
		out << pair.second.fForce << endl;
	}

	out.close();
}

void CImguiManager::All_Load()
{
	if (!m_isOnToolWindows)
	{
		MSG_BOX("Please Open the Windows");
		return;
	}

	string strDirectory = "../../Client/Bin/DataFiles/Character/" + m_ModelNameList[m_iModelSelectedIndex];

	AlphaMesh_Load(strDirectory);
	AnimationLoop_Load(strDirectory);
	AnimationEvent_Load(strDirectory);
	ColliderState_Load(strDirectory);
	EffectState_Load(strDirectory);
	RimEvent_Load(strDirectory);
	TrailEvent_Load(strDirectory);
	RadialEvent_Load(strDirectory);
}

void CImguiManager::AlphaMesh_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_AlphaMeshes.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("AlphaMeshes.dat 개방 실패");
		return;
	}

	_uint iMeshMapSize;

	in.read((char*)&iMeshMapSize, sizeof(_uint));

	m_AddedMeshes.clear();

	for (size_t i = 0; i < iMeshMapSize; i++)
	{
		_uint iMeshIndex = { 0 };
		in.read((char*)&iMeshIndex, sizeof(_uint));
		m_AddedMeshes.emplace(i, m_MeshNameList[iMeshIndex]);
	}
	in.close();
}

void CImguiManager::AnimationLoop_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_LoopAnimations.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("LoopAnimations.dat 개방 실패");
		return;
	}

	m_AddedAnims.clear();

	m_AnimNameList.clear();
	m_AnimNameList.resize(m_Anims.size());

	_uint iAnimMapSize;
	in.read(reinterpret_cast<char*>(&iAnimMapSize), sizeof(iAnimMapSize));
	
	for (size_t i = 0; i < iAnimMapSize; ++i)
	{
		// 키를 이진 형식으로 읽어옴
		_uint iAnimIndex = 0;
		in.read(reinterpret_cast<char*>(&iAnimIndex), sizeof(iAnimIndex));

		// 문자열 길이와 문자열을 이진 형식으로 읽어옴
		_uint length = 0;
		in.read(reinterpret_cast<char*>(&length), sizeof(length));

		string strAnimName(length, ' ');
		in.read(&strAnimName[0], length);

		m_AddedAnims.emplace(iAnimIndex, strAnimName);
	}

	in.close();
}

void CImguiManager::AnimationEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_AnimationEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("AnimationEvents 파일 개방 실패");
		return;
	}

	m_AnimationEvents.clear();

	_uint iEventMapSize = { 0 };

	in >> iEventMapSize;

	for (size_t i = 0; i < iEventMapSize; i++)
	{
		string strAnimName = "";
		in >> strAnimName;

		Animation_Event Desc{};
		in >> Desc.iType;
		in >> Desc.fAinmPosition;
		in >> Desc.strChannelName;
		in >> Desc.iBoneIndex;

		m_AnimationEvents.emplace(strAnimName, Desc);
	}

	in.close();
}

void CImguiManager::ColliderState_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_Colliders.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("Colliders 개방 실패");
		return;
	}

	m_AddedColliders.clear();

	_uint iColliderCount = { 0 };

	in.read((char*)&iColliderCount, sizeof(_uint));

	for (size_t i = 0; i < iColliderCount; i++)
	{
		_uint iBoneIndex = { 0 };
		_uint iColliderType = { 0 };
		in.read((char*)&iBoneIndex, sizeof(_uint));
		in.read((char*)&iColliderType, sizeof(_uint));

		_float3 vCenter;
		in.read((char*)&vCenter, sizeof(_float3));

		_float3 vExtents;
		_float fRadius;

		void* pValue = nullptr;

		switch (iColliderType)
		{
		case Engine::CCollider::COLLIDER_AABB:
		{
			in.read((char*)&vExtents, sizeof(_float3));

			pValue = &vExtents;
		}
		break;
		case Engine::CCollider::COLLIDER_OBB:
		{
			in.read((char*)&vExtents, sizeof(_float3));
			pValue = &vExtents;
		}
		break;
		case Engine::CCollider::COLLIDER_SPHERE:
		{
			in.read((char*)&fRadius, sizeof(_float));
			pValue = &fRadius;
		}
		break;
		}

		_bool isAlways = { false };
		int iColliderPartType, iColliderActionType;
		iColliderPartType = iColliderActionType = 0;

		in.read((char*)&iColliderActionType, sizeof(int));
		in.read((char*)&iColliderPartType, sizeof(int));
		in.read((char*)&isAlways, sizeof(_bool));

		if (!m_pRenderModel->Created_BoneCollider(iBoneIndex))
		{
			m_AddedColliders.emplace(iBoneIndex, Collider_State{ m_BoneNameList[iBoneIndex], iColliderActionType, iColliderPartType, isAlways });
			m_pRenderModel->Create_BoneCollider(iColliderType, iBoneIndex, vCenter, pValue);
		}

	}

	in.close();
}

void CImguiManager::EffectState_Load(string strPath)
{
	// 기존에 저장해둔 정보 클리어
	Clear_EffectStateMap();

	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_EffectState.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("EffectState 파일 개방 실패");
		return;
	}

	_uint iNumEffectState = { 0 };
	in >> iNumEffectState;

	for (size_t i = 0; i < iNumEffectState; i++)
	{
		string strBoneName = "";
		in >> strBoneName;

		string strEffectName = "";
		in >> strEffectName;

		Create_Effect(strBoneName, strEffectName);
	}



	in.close();
}

void CImguiManager::RimEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_RimEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("RimEvents 파일 개방 실패");
		return;
	}

	m_RimLightEvents.clear();

	_uint iNumRimEvents{ 0 };
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	in >> iNumRimEvents;

	Animation_RimLightState RimEvent{};
	string key;

	for (size_t i = 0; i < iNumRimEvents; i++)
	{
		in >> key;
		in >> RimEvent.iType;
		in >> RimEvent.strMeshName;
		in >> RimEvent.fAinmPosition;

		m_RimLightEvents.emplace(key, RimEvent);
	}

	in.close();
}

void CImguiManager::TrailEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_TrailEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("TrailEvents 파일 개방 실패");
		return;
	}

	m_TrailEvents.clear();

	_uint iNumTrailEvents{ 0 };
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	in >> iNumTrailEvents;

	Animation_TrailState TrailEvent{};
	string key;

	for (size_t i = 0; i < iNumTrailEvents; i++)
	{
		in >> key;
		in >> TrailEvent.iType;
		in >> TrailEvent.strBonelName;
		in >> TrailEvent.strTrailProtoName;
		in >> TrailEvent.iBoneIndex;
		in >> TrailEvent.fAinmPosition;
		
		m_TrailEvents.emplace(key, TrailEvent);

		if (TrailEvent.iType == 0)
		{
			if(m_EffectState.find(TrailEvent.strBonelName) == m_EffectState.end())
				Create_Effect(TrailEvent.strBonelName, m_SelectedEffectName, TEXT("Layer_Effect"));
		}

	}

	in.close();
}

void CImguiManager::SoundEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + (m_isCutSceneEvents ? "_CutSoundEvents.dat" : "_SoundEvents.dat");

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("SoundEvents 파일 개방 실패");
		return;
	}

	m_SoundEvents.clear();

	_uint iNumEvents{ 0 };
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	in >> iNumEvents;

	Animation_SoundEventState Event{};
	string key;

	/*
		CHANNELID eChannel;
		_float fAinmPosition;
		string strSoundFileName;
	*/
	for (size_t i = 0; i < iNumEvents; i++)
	{
		in >> key;
		in >> Event.iChannel;
		in >> Event.fAinmPosition;
		in >> Event.fSoundVolume;
		in >> Event.strSoundFileName;

		m_SoundEvents.emplace(key, Event);
	}

	in.close();
}

void CImguiManager::FaceEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_FaceEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("FaceEvents 파일 개방 실패");
		return;
	}

	m_FaceEvents.clear();

	_uint iNumEvents{ 0 };
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	in >> iNumEvents;

	Animation_FaceEventState Event{};
	string key;

	/*
		_uint iType;					//0번이 on 1번이 off 2번이 change
		_float fAinmPosition;
		_uint iFaceAnimIndex;
	*/
	for (size_t i = 0; i < iNumEvents; i++)
	{
		in >> key;
		in >> Event.iType;
		in >> Event.fAinmPosition;
		in >> Event.iFaceAnimIndex;

		m_FaceEvents.emplace(key, Event);
	}

	in.close();
}

void CImguiManager::BloodEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_BloodEffectEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("BloodEffectEvents 파일 개방 실패");
		return;
	}

	m_BloodEvents.clear();

	_uint iNumEvents{ 0 };
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	in >> iNumEvents;

	Animation_BloodEventState Event{};
	string key;

	/*
		_float fAinmPosition;
		_uint iBoneIndex;
		string strBonelName;
		_uint iBloodEffectType;		// 출력시킬 이펙트의 타입
		_bool isLoop;				// 이펙트 루프여부
	*/
	for (size_t i = 0; i < iNumEvents; i++)
	{
		in >> key;
		in >> Event.fAinmPosition;
		in >> Event.iBoneIndex;
		in >> Event.strBonelName;
		in >> Event.iBloodEffectType;
		in >> Event.isLoop;
		in >> Event.isOn;

		m_BloodEvents.emplace(key, Event);
	}

	in.close();
}

void CImguiManager::RadialEvent_Load(string strPath)
{
	string strDirectory = strPath;
	strDirectory += "/" + m_ModelNameList[m_iModelSelectedIndex] + "_RadialEvents.dat";

	ifstream in(strDirectory, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("RadialEvents 파일 개방 실패");
		return;
	}

	m_RadialEvents.clear();

	_uint iNumEvents{ 0 };
	// 총 몇개의 이펙트를 읽어올 것인지 작성한다
	in >> iNumEvents;

	Animation_RadialEventState Event{};
	string key;

	for (size_t i = 0; i < iNumEvents; i++)
	{
		in >> key;
		in >> Event.iType;
		in >> Event.fAinmPosition;
		in >> Event.fForce;

		m_RadialEvents.emplace(key, Event);
	}

	in.close();
}

void CImguiManager::Gui_Select_Bone(_uint iBoneIndex)
{
	Reset_Collider_Value();
	Setting_Collider_Value(iBoneIndex);
	m_pRenderModel->Select_Bone(iBoneIndex);
	m_iBoneSelectedIndex = iBoneIndex;
}

void CImguiManager::Setting_AnimationList()
{
	if (nullptr == m_pRenderModel) return;

	switch (m_iModelType)
	{
	case PLAYER:
	{
		m_Anims = m_Anims;
		break;
	}
	case ENEMY:
	{
		auto pAnimCom = m_pRenderModel->Get_AnimComponent();
		m_Anims = pAnimCom->Get_Animations();
		break;
	}
	}
}

void CImguiManager::Setting_RimLight()
{
	auto lower_bound_iter = m_RimLightEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_RimLightEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (lower_bound_iter != m_RimLightEvents.end() && (*lower_bound_iter).first != m_AnimNameList[m_iAnimIndex])
		return;

	auto Anims = m_Anims;
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		if ((*lower_bound_iter).second.fAinmPosition < CurrentPosition)
		{
			if ((*lower_bound_iter).second.iType == 0)
				m_pRenderModel->Set_RimMeshName((*lower_bound_iter).second.strMeshName);
			else
				m_pRenderModel->Set_RimMeshName("");
		}
	}
}

void CImguiManager::Setting_Trail()
{
	auto lower_bound_iter = m_TrailEvents.lower_bound(m_AnimNameList[m_iAnimIndex]);
	auto upper_bound_iter = m_TrailEvents.upper_bound(m_AnimNameList[m_iAnimIndex]);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (lower_bound_iter != m_TrailEvents.end() && (*lower_bound_iter).first != m_AnimNameList[m_iAnimIndex])
		return;

	auto Anims = m_Anims;
	_float CurrentPosition = (_float)(*(Anims[m_iAnimIndex]->Get_CurrentPosition()));

	for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
	{
		if ((*lower_bound_iter).second.fAinmPosition < CurrentPosition)
		{
			if ((*lower_bound_iter).second.iType == 0)
				m_pRenderModel->Trail_On((*lower_bound_iter).second.strBonelName, true);
			else
				m_pRenderModel->Trail_On((*lower_bound_iter).second.strBonelName, false);
		}
	}
}

void CImguiManager::Change_Model()
{
	m_iAnimIndex = 0;
	m_iSearchAnimIndex = 0;
	m_iAddedAnimSelectedIndex = 0;
	m_iBoneSelectedIndex = 0;
	m_iColliderSelectedIndex = 0;
	m_iEventBoneIndex = 0;
	m_iChannelSelectedIndex = 0;
	m_iEventSelectedIndex = 0;
	m_iMeshSelectedIndex = 0;
	m_iAddedMeshSelectedIndex = 0;

	wstring strModelName = m_pGameInstance->StringToWstring(m_ModelNameList[m_iModelSelectedIndex]);
	wstring strAnimName = TEXT("");

	if (TEXT("Kiryu") == strModelName)
	{
		if (SYNC_COM == m_iModelType)
		{
			strAnimName = TEXT("CutSceneAnim_ForPlayer");
		}
		else
		{
			strAnimName = strModelName;
		}

	}
	else if (TEXT("Jimu") == strModelName || TEXT("Kuze") == strModelName || TEXT("Kuze_Fight") == strModelName
		|| TEXT("YakuzaA") == strModelName || TEXT("YakuzaB") == strModelName || TEXT("YakuzaC") == strModelName || TEXT("YakuzaD") == strModelName
		|| TEXT("Yoneda") == strModelName)
	{
		if (SYNC_COM == m_iModelType)
		{
			strAnimName = TEXT("SyncAnim");
		}
		else
		{

			strAnimName = TEXT("Common");
		}
	}
	else if (TEXT("Bike") == strModelName || TEXT("Heli") == strModelName || TEXT("Van") == strModelName || TEXT("Sedan") == strModelName)
	{
		strAnimName = TEXT("ReactorHighwayAnim");
	}

	m_pRenderModel->Change_Model(strModelName, strAnimName);
}

void CImguiManager::Setting_InitialData(string strFolderType)
{
	Setting_ModelList(strFolderType);

	string strDirPath = "../../Client/Bin/DataFiles/Particle/";
	m_pGameInstance->Get_DirectoryName(strDirPath, m_EffectTypeList);

	for (size_t i = 0; i < m_EffectTypeList.size(); i++)
	{
		vector<string> tempFileNames;
		m_pGameInstance->Get_FileNames(strDirPath + m_EffectTypeList[i], tempFileNames);

		for (size_t j = 0; j < tempFileNames.size(); j++)
		{
			m_EffectFiles.emplace(i, tempFileNames[j]);
		}
	}
}

void CImguiManager::Setting_ModelList(string strFolderType)
{
	m_ModelNameList.clear();

	string strDirPath = "../../Client/Bin/Resources/Models/Anim/" + strFolderType;

	m_pGameInstance->Get_DirectoryName(strDirPath, m_ModelNameList);
	g_wstrModelName = m_pGameInstance->StringToWstring(m_ModelNameList.front());
}

CImguiManager* CImguiManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImguiManager* pInstance = new CImguiManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Engine::Safe_Release(pInstance);
		MSG_BOX("CImguiManager Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CImguiManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}