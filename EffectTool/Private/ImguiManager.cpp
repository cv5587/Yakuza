#include "ImguiManager.h"
#include "GameInstance.h"

#pragma region "객체 원형"
#include "Particle_Point.h"
#include "TRailEffect.h"
#include "Particle_Mesh.h"
#pragma endregion

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
bool useWindow = false;

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f };


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
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\malgun.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesKorean());


	TextureTags.push_back(TEXT("Prototype_Component_Texture_Sphere"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_Trail"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_Test"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_HitSpark"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_HitBase"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_SmokeBase"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_GuardParticle"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_GuardSmoke"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_GuardDist"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_Money"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_Coin"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_AuraAnim"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_WaveFace"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_AuraBase"));

	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD0"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD1"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD2"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD3"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD4"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD5"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD6"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD7"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodD8"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_LiquidD"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimAD"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimBD"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimCD"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimDD"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimED"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimFD"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_ExpAnimA"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_ExpAnimB"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_FireAnim"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_CarBackLight"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BrokenGlass0"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BrokenGlass1"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BrokenGlass2"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BrokenGlass3"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_BloodAnim"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_DestroyerFlud"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_FireRoundA"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_FireRoundB"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_FireRoundC"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_RoopSmoke"));

	TextureTags.push_back(TEXT("Prototype_Component_Texture_KuzeTrail"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_KuzeHand"));
	TextureTags.push_back(TEXT("Prototype_Component_Texture_KuzeHandFire"));


	ToneTextureTags.push_back(TEXT("Prototype_Component_Texture_AuraTone"));
	ToneTextureTags.push_back(TEXT("Prototype_Component_Texture_AuraToneRush"));
	ToneTextureTags.push_back(TEXT("Prototype_Component_Texture_AuraToneDestroy"));
	ToneTextureTags.push_back(TEXT("Prototype_Component_Texture_BloodTone"));

	MeshTags.push_back(TEXT("Prototype_Component_Model_ParticleSphere"));
	MeshTags.push_back(TEXT("Prototype_Component_Model_ParticleMoney"));
	MeshTags.push_back(TEXT("Prototype_Component_Model_ParticleBlood"));

	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN0"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN1"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN2"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN3"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN4"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN5"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN6"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN7"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_BloodN8"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_LiquidN"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimAN"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimBN"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimCN"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimDN"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimEN"));
	NormalTags.push_back(TEXT("Prototype_Component_Texture_LiquidAnimFN"));


	if (nullptr != pArg)
	{

	}
	m_EffectDesc.vStartPos = { 0.f, 0.f, 0.f, 1.f };
	m_EffectDesc.fRotate = { 0.f};
	m_EffectDesc.fLifeAlpha = { 1.f, 1.f};
	m_EffectDesc.eType = 0;
	m_EffectDesc.ParticleTag = { TEXT("") };
	m_EffectDesc.fStartTime = { 0.f };
	m_EffectDesc.vStartColor = { 1.0f , 1.0f ,1.0f ,1.0f};
	m_EffectDesc.vEndColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_EffectDesc.iShaderPass = { 0 };
	m_EffectDesc.TextureTag = TextureTags[0];

	m_EffectDesc.BufferInstance.iNumInstance = 1;
	m_EffectDesc.BufferInstance.isLoop = true;
	m_EffectDesc.BufferInstance.vLifeTime = _float2(1.f, 1.f);
	m_EffectDesc.BufferInstance.vOffsetPos = _float3(0.f, 0.f, 0.f);
	m_EffectDesc.BufferInstance.vPivotPos = _float3(0.f, 0.f, 0.f);
	m_EffectDesc.BufferInstance.vRange = _float3(0.f, 0.f, 0.f);
	m_EffectDesc.BufferInstance.vSize = _float2(1.f, 1.f);
	m_EffectDesc.BufferInstance.vSpeed = _float2(0.f, 0.f);
	m_EffectDesc.BufferInstance.vRectSize = _float2(1.0f, 1.0f);
	m_EffectDesc.BufferInstance.fRadius = 1.f;

	


	m_TrailDesc.vStartPos = { 0.f, 0.f, 0.f, 1.f };
	m_TrailDesc.eType = 1;
	m_TrailDesc.ParticleTag = { TEXT("") };
	m_TrailDesc.fStartTime = { 0.f };
	m_TrailDesc.vStartColor = { 0.0f , 0.0f ,0.0f ,1.0f };
	m_TrailDesc.vEndColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_TrailDesc.iShaderPass = { 0 };
	m_TrailDesc.fSpeedPecSec = 5.0f;
	m_TrailDesc.TextureTag = TextureTags[0];

	m_TrailDesc.Trail_Desc.iMaxTrail = 10;
	m_TrailDesc.Trail_Desc.vInitPosA = _float3{ 0.0f , 0.0f , 0.0f };
	m_TrailDesc.Trail_Desc.vInitPosB = _float3{0.0f , 1.0f, 0.0f};	



	return S_OK;
}



void CImguiManager::Tick(const _float& fTimeDelta)
{
	m_fParticleTime += fTimeDelta;
	if (m_fParticleTime >= m_fMaxTime)
	{
		Reset_Particle();
		m_fParticleTime = 0.f;
	}


	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);

	Mode_Select(fTimeDelta);

	switch (m_iMode)
	{
	case MODE_PARTICLE :
	{
		Create_Tick(fTimeDelta);
		Editor_Tick(fTimeDelta);
		Guizmo_Tick(fTimeDelta);
		Timeline_Tick(fTimeDelta);
		if (!m_EditParticle.empty())
		{

			for (auto& iter : m_EditParticle)
			{
				iter->Tick(fTimeDelta);
				iter->Late_Tick(fTimeDelta);
			}
		}
	}
		break;
	case MODE_TRAIL:
	{
		CreateTrail_Tick(fTimeDelta);
		EditorTrail_Tick(fTimeDelta);
		Guizmo_Tick(fTimeDelta);
		if (!m_EditTrail.empty())
		{

			for (auto& iter : m_EditTrail)
			{
				iter->Tick(fTimeDelta);
				iter->Late_Tick(fTimeDelta);
			}
		}
	}
		break;

	case MODE_AURA:
	{
		CreateAura_Tick(fTimeDelta);
		EditorAura_Tick(fTimeDelta);
		Guizmo_Tick(fTimeDelta);
		Timeline_Tick(fTimeDelta);
		if (!m_EditAura.empty())
		{

			for (auto& iter : m_EditAura)
			{
				iter->Tick(fTimeDelta);
				iter->Late_Tick(fTimeDelta);
			}
		}
	}
	break;
	case MODE_MESH:
	{
		CreateMesh_Tick(fTimeDelta);
		EditorMesh_Tick(fTimeDelta);
		Guizmo_Tick(fTimeDelta);
		Timeline_Tick(fTimeDelta);
		if (!m_EditMesh.empty())
		{

			for (auto& iter : m_EditMesh)
			{
				iter->Tick(fTimeDelta);
				iter->Late_Tick(fTimeDelta);
			}
		}
	}
	break;
	default:
		break;
	}

}



HRESULT CImguiManager::Render()
{
	ImGui::Render();

	return S_OK;
}

void CImguiManager::EditTransform(_float* cameraView, _float* cameraProjection, _float* matrix)
{
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	if (ImGui::IsKeyPressed(ImGuiKey_T))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	ImGui::SameLine();

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;

	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	
	//m_EffectDesc.vStartPos = _float4(matrix[12], matrix[13], matrix[14], matrix[15]);
	//dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex])->Set_StartPos(m_EffectDesc.vStartPos);
	//_float4x4 worldmatrix;
	//memcpy(&worldmatrix, matrix, sizeof(_float4x4));
	//m_EditParticle[m_iCurEditIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&worldmatrix));
	
	switch (m_iMode)
	{
	case MODE_PARTICLE:
		//dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex])->Set_StartPos(_float4(matrix[12], matrix[13], matrix[14], matrix[15]));
		break;
	case MODE_TRAIL:
		//dynamic_cast<CEffect*>(m_EditTrail[m_iCurEditIndex])->Set_StartPos(_float4(matrix[12], matrix[13], matrix[14], matrix[15]));
		break;
	}
	
	
	if (useWindow)
	{
		ImGui::PopStyleColor(1);
	}
}

void CImguiManager::Guizmo(_float fTimeDelta)
{


	if(m_bGuizmo)
	{
		_float* cameraView = (_float*)m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
		_float* cameraProjection = (_float*)m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

		if (-1 != m_iCurEditIndex)	
		{
			_float* objectWorld{};
			switch (m_iMode)
			{
			case MODE_PARTICLE:
			{
				objectWorld = (_float*)dynamic_cast<CTransform*>(m_EditParticle[m_iCurEditIndex]->Get_TransformCom())->Get_WorldFloat4x4();
			}
				break;
			case MODE_TRAIL:
			{
				objectWorld = (_float*)dynamic_cast<CTransform*>(m_EditTrail[m_iCurEditIndex]->Get_TransformCom())->Get_WorldFloat4x4();
			}
				break;
			case MODE_AURA:
			{
				objectWorld = (_float*)dynamic_cast<CTransform*>(m_EditAura[m_iCurEditIndex]->Get_TransformCom())->Get_WorldFloat4x4();
			}
			break;
			case MODE_MESH:
			{
				objectWorld = (_float*)dynamic_cast<CTransform*>(m_EditMesh[m_iCurEditIndex]->Get_TransformCom())->Get_WorldFloat4x4();
			}
			break;
			default:
				break;
			}
			EditTransform(cameraView, cameraProjection, objectWorld);
			ImGuizmo::IsUsing();
		}
	}

}

HRESULT CImguiManager::Mode_Select(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(150, 130), ImGuiCond_None);

	ImGui::Begin("Mode_Select");

	if (ImGui::Button("Particle Mode"))
	{
		m_iMode = MODE_PARTICLE;
		if (!m_EditParticle.empty())
			m_iCurEditIndex = 0;
		else
			m_iCurEditIndex = -1;
	}
	if (ImGui::Button("Trail Mode"))
	{
		m_iMode = MODE_TRAIL;
		if (!m_EditTrail.empty())
			m_iCurEditIndex = 0;
		else
			m_iCurEditIndex = -1;
	}
	if (ImGui::Button("Aura Mode"))
	{
		m_iMode = MODE_AURA;
		if (!m_EditAura.empty())
			m_iCurEditIndex = 0;
		else
			m_iCurEditIndex = -1;
	}
	if (ImGui::Button("Mesh Mode"))
	{
		m_iMode = MODE_MESH;
		if (!m_EditMesh.empty())
			m_iCurEditIndex = 0;
		else
			m_iCurEditIndex = -1;
	}
	ImGui::End();

	return S_OK;
}

HRESULT CImguiManager::Create_Particle()
{
	if (-1 == m_iCurEditIndex)
		m_iCurEditIndex = 0;

	CParticle_Point::PARTICLE_POINT_DESC EffectDesc{};
	EffectDesc.BufferInstance.iNumInstance = 1;
	EffectDesc.BufferInstance.isLoop = true;
	EffectDesc.BufferInstance.vLifeTime = _float2(1.f, 1.f);
	EffectDesc.BufferInstance.vOffsetPos = _float3(0.f, 0.f, 0.f);
	EffectDesc.BufferInstance.vPivotPos = _float3(0.f, 0.f, 0.f);
	EffectDesc.BufferInstance.vRange = _float3(0.f, 0.f, 0.f);
	EffectDesc.BufferInstance.vSize = _float2(1.f, 1.f);
	EffectDesc.BufferInstance.vSpeed = _float2(1.f, 1.f);
	EffectDesc.BufferInstance.vRectSize = _float2(1.0f, 1.0f);
	EffectDesc.BufferInstance.fRadius = 1.f;	

	EffectDesc.vStartPos = { 0.f, 0.f, 0.f, 1.f };
	EffectDesc.fRotate = { 0.f};
	EffectDesc.fLifeAlpha = { 0.f, 0.f};
	EffectDesc.eType = CEffect::TYPE_POINT;
	EffectDesc.ParticleTag = m_pGameInstance->StringToWstring(text_input_buffer);
	EffectDesc.fStartTime = { 0.f };
	EffectDesc.iShaderPass = { 0 };
	EffectDesc.TextureTag = TextureTags[0];



	CGameObject* pGameParticle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Particle_Point"), &EffectDesc);
	if (nullptr == pGameParticle)
		return E_FAIL;

	if(m_bSpread)
		dynamic_cast<CEffect*>(pGameParticle)->Edit_Action(CEffect::ACTION_SPREAD);
	if (m_bSizeup)
		dynamic_cast<CEffect*>(pGameParticle)->Edit_Action(CEffect::ACTION_SIZEUP);
	if (m_bSizedown)
		dynamic_cast<CEffect*>(pGameParticle)->Edit_Action(CEffect::ACTION_SIZEDOWN);

	m_EditParticle.push_back(pGameParticle);
	m_iCurEditIndex = m_EditParticle.size() - 1;
	return S_OK;
}

HRESULT CImguiManager::Create_Trail()
{
	if (-1 == m_iCurEditIndex)
		m_iCurEditIndex = 0;

	CTRailEffect::TRAIL_DESC EffectDesc{};


	EffectDesc.Trail_Desc.iMaxTrail = 10;	
	EffectDesc.Trail_Desc.vInitPosA = _float3{ 0.0f , 0.0f , 0.0f };	
	EffectDesc.Trail_Desc.vInitPosB = _float3{ 0.0f , 1.0f, 0.0f };	
	EffectDesc.fSpeedPecSec = 5.f;
	EffectDesc.vStartPos = { 0.f, 0.f, 0.f, 1.f };
	EffectDesc.eType = CEffect::TYPE_TRAIL;
	EffectDesc.ParticleTag = m_pGameInstance->StringToWstring(m_TrailTag);
	EffectDesc.fStartTime = { 0.f };
	EffectDesc.iShaderPass = { 0 };
	EffectDesc.TextureTag = TextureTags[0];


	CGameObject* pGameParticle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TrailEffect"), &EffectDesc);
	if (nullptr == pGameParticle)
		return E_FAIL;

	m_EditTrail.push_back(pGameParticle);
	m_iCurEditIndex = m_EditTrail.size() - 1;
	return S_OK;
}

HRESULT CImguiManager::Create_Aura()
{
	if (-1 == m_iCurEditIndex)
		m_iCurEditIndex = 0;

	CAura::AURA_DESC AuraDesc{};


	AuraDesc.BufferInstance.iNumInstance = 1;
	AuraDesc.BufferInstance.isLoop = true;
	AuraDesc.BufferInstance.vLifeTime = _float2(1.f, 1.f);
	AuraDesc.BufferInstance.vOffsetPos = _float3(0.f, 0.f, 0.f);
	AuraDesc.BufferInstance.vPivotPos = _float3(0.f, 0.f, 0.f);
	AuraDesc.BufferInstance.vRange = _float3(0.f, 0.f, 0.f);
	AuraDesc.BufferInstance.vSize = _float2(1.f, 1.f);
	AuraDesc.BufferInstance.vSpeed = _float2(1.f, 1.f);
	AuraDesc.BufferInstance.vRectSize = _float2(1.0f, 1.0f);
	AuraDesc.BufferInstance.fRadius = 1.f;

	AuraDesc.vStartPos = { 0.f, 0.f, 0.f, 1.f };
	AuraDesc.fRotate = { 0.f };
	AuraDesc.fLifeAlpha = { 0.f, 0.f };
	AuraDesc.eType = CEffect::TYPE_POINT;
	AuraDesc.ParticleTag = m_pGameInstance->StringToWstring(m_AuraTag);
	AuraDesc.fStartTime = { 0.f };
	AuraDesc.iShaderPass = { 0 };
	AuraDesc.TextureTag = TextureTags[0];
	AuraDesc.ToneTextureTag = ToneTextureTags[0];

	CGameObject* pGameParticle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Aura"), &AuraDesc);
	if (nullptr == pGameParticle)
		return E_FAIL;

	m_EditAura.push_back(pGameParticle);
	m_iCurEditIndex = m_EditAura.size() - 1;

	return S_OK;
}

void CImguiManager::CreateTrail_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(1030, 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_None);

	ImGui::Begin("Trail_Create");


	if (ImGui::InputText("TrailTag", m_TrailTag, IM_ARRAYSIZE(m_TrailTag)))
	{
		m_TrailDesc.ParticleTag = m_pGameInstance->StringToWstring(m_TrailTag);
	}

	if (ImGui::Button("Create Trail"))
	{
		Create_Trail();	
	}
	if (ImGui::Button("Delete Trail"))
	{
		if (!m_EditTrail.empty())
		{
			Safe_Release(m_EditTrail.back());
			m_EditTrail.pop_back();
			if (m_EditTrail.empty())
				m_iCurEditIndex = -1;
			else
				m_iCurEditIndex = 0;
		}
	}
	if (ImGui::Button(u8"파티클 모두저장"))
	{
		AllEffect_Save();
	}
	Load_Selctor();


	ImGui::End();
}

void CImguiManager::CreateAura_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(1030, 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_None);

	ImGui::Begin("Aura_Create");


	if (ImGui::InputText("AuraTag", m_AuraTag, IM_ARRAYSIZE(m_AuraTag)))
	{
		m_AuraDesc.ParticleTag = m_pGameInstance->StringToWstring(m_AuraTag);//톤 만 넣기.
	}

	if (ImGui::Button("Create Aura"))
	{
		Create_Aura();
	}
	if (ImGui::Button("Delete Aura"))
	{
		if (!m_EditAura.empty())
		{
			Safe_Release(m_EditAura.back());
			m_EditAura.pop_back();
			if (m_EditAura.empty())
				m_iCurEditIndex = -1;
			else
				m_iCurEditIndex = 0;
		}
	}
	if (ImGui::Button(u8"파티클 모두저장"))
	{
		AllEffect_Save();
	}
	Load_Selctor();


	ImGui::End();
}

void CImguiManager::EditorAura_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(980, 200), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(300, 520), ImGuiCond_None);

	ImGui::Begin("Aura_Edit");

	_int iSize = m_EditAura.size();
	if (ImGui::BeginListBox("listBox"))
	{
		for (int i = 0; i < iSize; i++)
		{
			// 리스트박스 아이템의 텍스트
			const bool isSelected = (m_iCurEditIndex == i);
			char Label[256] = {};
			strcpy_s(Label, m_pGameInstance->WstringToString(dynamic_cast<CEffect*>(m_EditAura[i])->Get_Tag()).c_str());
			if (ImGui::Selectable(Label, isSelected))
			{
				m_iCurEditIndex = i; // 선택된 아이템 업데이트	
			}

			// 선택된 아이템이 보이도록 스크롤
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	if (-1 != m_iCurEditIndex)
	{
		Load_Desc(m_iCurEditIndex);
	}

	_bool bChange = false;
	ImGui::Text(to_string(m_iCurEditIndex).c_str());


	if (ImGui::RadioButton("FLUID", &m_AuraDesc.iShaderPass, 0))
		bChange = true;
	if (ImGui::RadioButton("Start", &m_AuraDesc.iShaderPass, 1))
		bChange = true;
	if (ImGui::RadioButton("Anim", &m_AuraDesc.iShaderPass, 2))
		bChange = true;
	if (ImGui::RadioButton("ToneAnim", &m_AuraDesc.iShaderPass, 3))
		bChange = true;

	if (ImGui::Checkbox("Attach", &m_AuraDesc.BufferInstance.isAttach))
	{
		bChange = true;
	}


	if (ImGui::Checkbox("Spread", &m_bSpread))
	{
		if (-1 != m_iCurEditIndex)
		{
			CEffect* pParticle = dynamic_cast<CEffect*>(m_EditAura[m_iCurEditIndex]);
			pParticle->Edit_Action(CEffect::ACTION_SPREAD);
		}
	}
	ImGui::SameLine();

	if (ImGui::Checkbox("SizeUP", &m_bSizeup))
	{
		if (-1 != m_iCurEditIndex)
		{
			CEffect* pParticle = dynamic_cast<CEffect*>(m_EditAura[m_iCurEditIndex]);
			pParticle->Edit_Action(CEffect::ACTION_SIZEUP);
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("SizeDown", &m_bSizedown))
	{
		if (-1 != m_iCurEditIndex)
		{
			CEffect* pParticle = dynamic_cast<CEffect*>(m_EditAura[m_iCurEditIndex]);
			pParticle->Edit_Action(CEffect::ACTION_SIZEDOWN);
		}
	}

	if (ImGui::Checkbox("useRadius", &m_AuraDesc.BufferInstance.bRadius))
	{
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("isLoop", &m_AuraDesc.BufferInstance.isLoop))
	{
		bChange = true;
	}

	static char strAuraReName[MAX_PATH]{};

	ImGui::InputText("ParticleTag", strAuraReName, MAX_PATH);
	if (ImGui::Button(u8"변경하기"))
	{
		bChange = true;
		string tag = strAuraReName;
		m_AuraDesc.ParticleTag = m_pGameInstance->StringToWstring(tag);
	}

	if (ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &m_AuraDesc.BufferInstance.iNumInstance))
	{
		bChange = true;
	}


	_float* Temp = (_float*)&m_AuraDesc.BufferInstance.vOffsetPos;
	if (ImGui::InputFloat3("OffsetPos", Temp))
	{
		memcpy(&m_AuraDesc.BufferInstance.vOffsetPos, Temp, sizeof(_float3));
		bChange = true;
	}

	Temp = (_float*)&m_AuraDesc.BufferInstance.vPivotPos;
	if (ImGui::InputFloat3("PivotPos", Temp))
	{
		memcpy(&m_AuraDesc.BufferInstance.vPivotPos, Temp, sizeof(_float3));
		bChange = true;
	}

	if (!m_AuraDesc.BufferInstance.bRadius)
	{

		Temp = (_float*)&m_AuraDesc.BufferInstance.vRange;
		if (ImGui::InputFloat3("Range", Temp))
		{
			memcpy(&m_AuraDesc.BufferInstance.vRange, Temp, sizeof(_float3));
			bChange = true;
		}
	}
	else
	{
		Temp = (_float*)&m_AuraDesc.BufferInstance.fRadius;
		if (ImGui::InputFloat("Radius", Temp))
		{
			memcpy(&m_AuraDesc.BufferInstance.fRadius, Temp, sizeof(_float));
			bChange = true;
		}
	}

	Temp = (_float*)&m_AuraDesc.BufferInstance.vSize;
	if (ImGui::InputFloat2("Size", Temp))
	{
		memcpy(&m_AuraDesc.BufferInstance.vSize, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_AuraDesc.BufferInstance.vRectSize;
	if (ImGui::InputFloat2("RectSize", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_AuraDesc.BufferInstance.vRectSize, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_AuraDesc.BufferInstance.vSpeed;
	if (ImGui::InputFloat2("Speed", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_AuraDesc.BufferInstance.vSpeed, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_AuraDesc.BufferInstance.vLifeTime;
	if (ImGui::InputFloat2("LifeTime", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_AuraDesc.BufferInstance.vLifeTime, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_AuraDesc.fLifeAlpha;
	if (ImGui::InputFloat2("LifeAlpha", Temp))
	{
		memcpy(&m_AuraDesc.fLifeAlpha, Temp, sizeof(_float2));
		bChange = true;
	}


	Temp = (_float*)&m_AuraDesc.fUVCount;
	if (ImGui::InputFloat2("UVCount", Temp))
	{
		memcpy(&m_AuraDesc.fUVCount, Temp, sizeof(_float2));
		bChange = true;
	}


	Temp = (_float*)&m_AuraDesc.fToneUVCount;
	if (ImGui::InputFloat2("fToneUVCount", Temp))
	{
		memcpy(&m_AuraDesc.fToneUVCount, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_AuraDesc.fFlowPow;
	if (ImGui::InputFloat("fFlowPow", Temp))
	{
		memcpy(&m_AuraDesc.fFlowPow, Temp, sizeof(_float));
		bChange = true;
	}
	Temp = (_float*)&m_AuraDesc.fFlowSpeed;
	if (ImGui::InputFloat("fFlowSpeed", Temp))
	{
		memcpy(&m_AuraDesc.fFlowSpeed, Temp, sizeof(_float));
		bChange = true;
	}

	File_Selctor(&bChange);
	Tone_File_Selector(&bChange);
	if (2 == m_AuraDesc.iShaderPass)
	{
		static bool alpha_preview = true;
		static bool alpha_half_preview = false;
		static bool drag_and_drop = true;
		static bool options_menu = true;
		static bool hdr = false;
		ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

		if (ImGui::ColorEdit4("MyColor##2f", (float*)&m_AuraDesc.vStartColor, ImGuiColorEditFlags_Float | misc_flags))
		{
			bChange = true;
		}

		static bool alpha_preview1 = true;
		static bool alpha_half_preview1 = false;
		static bool drag_and_drop1 = true;
		static bool options_menu1 = true;
		static bool hdr1 = false;
		ImGuiColorEditFlags misc_flags1 = (hdr1 ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop1 ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview1 ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview1 ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu1 ? 0 : ImGuiColorEditFlags_NoOptions);

		if (ImGui::ColorEdit4("MyColor##", (float*)&m_AuraDesc.vEndColor, ImGuiColorEditFlags_Float | misc_flags1))
		{
			bChange = true;
		}

	}


	if (bChange)
	{
		Edit_Particle(m_iCurEditIndex);
	}
	ImGui::End();



}

void CImguiManager::Tone_File_Selector(_bool* bChange)
{
	_int iSize = ToneTextureTags.size();
	if (ImGui::BeginListBox("ToneBox"))
	{
		for (int i = 0; i < iSize; i++)
		{
			// 리스트박스 아이템의 텍스트
			const bool isSelected = (m_iCurToneTexture == i);
			char Label[256] = {};
			strcpy_s(Label, m_pGameInstance->WstringToString(ToneTextureTags[i]).c_str());
			if (ImGui::Selectable(Label, isSelected))
			{
				m_iCurToneTexture = i; // 선택된 아이템 업데이트	
				*bChange = true;
			}


			// 선택된 아이템이 보이도록 스크롤
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();

			}
		}
		ImGui::EndListBox();
	}


	if (-1 != m_iCurEditIndex)
	{
		m_AuraDesc.ToneTextureTag = ToneTextureTags[m_iCurToneTexture];
	}

}

HRESULT CImguiManager::Create_Mesh()
{

	if (-1 == m_iCurEditIndex)
		m_iCurEditIndex = 0;


	CParticle_Mesh::PARTICLE_MESH_DESC MeshDesc{};

	MeshDesc.BufferInstance.iNumInstance = 1;
	MeshDesc.BufferInstance.isLoop = true;
	MeshDesc.BufferInstance.vLifeTime = _float2(1.f, 1.f);
	MeshDesc.BufferInstance.vOffsetPos = _float3(0.f, 0.f, 0.f);
	MeshDesc.BufferInstance.vPivotPos = _float3(0.f, 0.f, 0.f);
	MeshDesc.BufferInstance.vRange = _float3(0.f, 0.f, 0.f);
	MeshDesc.BufferInstance.vSize = _float2(1.f, 1.f);
	MeshDesc.BufferInstance.vSpeed = _float2(1.f, 1.f);
	MeshDesc.BufferInstance.vRectSize = _float2(1.0f, 1.0f);
	MeshDesc.BufferInstance.fRadius = 1.f;

	MeshDesc.vStartPos = { 0.f, 0.f, 0.f, 1.f };
	MeshDesc.fRotate = { 0.f };
	MeshDesc.fLifeAlpha = { 0.f, 0.f };
	MeshDesc.eType = CEffect::TYPE_MESHEFFECT;
	MeshDesc.ParticleTag = m_pGameInstance->StringToWstring(text_input_buffer);
	MeshDesc.fStartTime = { 0.f };
	MeshDesc.iShaderPass = { 0 };
	MeshDesc.TextureTag = TextureTags[0];

	MeshDesc.strModelTag = TEXT("Prototype_Component_Model_ParticleSphere");



	CGameObject* pGameParticle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Particle_Mesh"), &MeshDesc);
	if (nullptr == pGameParticle)
		return E_FAIL;

	if (m_bSpread)
		dynamic_cast<CEffect*>(pGameParticle)->Edit_Action(CEffect::ACTION_SPREAD);
	if (m_bSizeup)
		dynamic_cast<CEffect*>(pGameParticle)->Edit_Action(CEffect::ACTION_SIZEUP);
	if (m_bSizedown)
		dynamic_cast<CEffect*>(pGameParticle)->Edit_Action(CEffect::ACTION_SIZEDOWN);

	m_EditMesh.push_back(pGameParticle);
	m_iCurEditIndex = m_EditMesh.size() - 1;
	return S_OK;
}

void CImguiManager::CreateMesh_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(980, 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_None);

	ImGui::Begin("Particle_Mesh_Create");


	if (ImGui::InputText("ParticleTag", text_input_buffer, IM_ARRAYSIZE(text_input_buffer)))
	{
		m_EffectDesc.ParticleTag = m_pGameInstance->StringToWstring(text_input_buffer);
	}

	if (ImGui::Button("Create Particle Mesh"))
	{
		if ('\0' != text_input_buffer[0])
		{
			Create_Mesh();
		}
	}
	if (ImGui::Button("Delete Particle"))
	{
		if (!m_EditMesh.empty())
		{
			Safe_Release(m_EditMesh.back());
			m_EditMesh.pop_back();
			if (m_EditMesh.empty())
				m_iCurEditIndex = -1;
			else
				m_iCurEditIndex = 0;
		}
	}
	if (ImGui::Button(u8"파티클 모두저장"))
	{
		AllEffect_Save();
	}
	Load_Selctor();


	ImGui::End();
}

void CImguiManager::EditorMesh_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(980, 200), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(300, 520), ImGuiCond_None);

	ImGui::Begin("Mesh_Edit");

	_int iSize = m_EditMesh.size();
	if (ImGui::BeginListBox("listBox"))
	{
		for (int i = 0; i < iSize; i++)
		{
			// 리스트박스 아이템의 텍스트
			const bool isSelected = (m_iCurEditIndex == i);
			char Label[256] = {};
			strcpy_s(Label, m_pGameInstance->WstringToString(dynamic_cast<CEffect*>(m_EditMesh[i])->Get_Tag()).c_str());
			if (ImGui::Selectable(Label, isSelected))
			{
				m_iCurEditIndex = i; // 선택된 아이템 업데이트	
				m_MeshDesc.ParticleTag = dynamic_cast<CEffect*>(m_EditMesh[i])->Get_Tag();
			}

			// 선택된 아이템이 보이도록 스크롤
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	if (-1 != m_iCurEditIndex)
	{
		Load_Desc(m_iCurEditIndex);
	}

	_bool bChange = false;
	ImGui::Text(to_string(m_iCurEditIndex).c_str());


	if (ImGui::RadioButton("MESH", &m_MeshDesc.iShaderPass, MESHPASS_MESH))
	{
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("MESHEFFECT", &m_MeshDesc.iShaderPass, MESHPASS_MESHEFFECT))
	{
		bChange = true;
	}






	static char strReName[MAX_PATH]{};

	ImGui::InputText("ParticleTag", strReName, MAX_PATH);
	if (ImGui::Button(u8"변경하기"))
	{
		bChange = true;
		string tag = strReName;
		m_MeshDesc.ParticleTag = m_pGameInstance->StringToWstring(tag);
	}

	if (ImGui::Checkbox("Nobill", &m_bNobillboard))
	{
		if (-1 != m_iCurEditIndex)
		{
			CEffect* pParticle = dynamic_cast<CEffect*>(m_EditMesh[m_iCurEditIndex]);
			pParticle->Edit_Action(CEffect::ACTION_NOBILLBOARD);
		}
	}
	ImGui::SameLine();

	if (ImGui::Checkbox("SizeUP", &m_bSizeup))
	{
		if (-1 != m_iCurEditIndex)
		{
			CEffect* pParticle = dynamic_cast<CEffect*>(m_EditMesh[m_iCurEditIndex]);
			pParticle->Edit_Action(CEffect::ACTION_SIZEUP);
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("SizeDown", &m_bSizedown))
	{
		if (-1 != m_iCurEditIndex)
		{
			CEffect* pParticle = dynamic_cast<CEffect*>(m_EditMesh[m_iCurEditIndex]);
			pParticle->Edit_Action(CEffect::ACTION_SIZEDOWN);
		}
	}




	if (ImGui::Checkbox("useRadius", &m_MeshDesc.BufferInstance.bRadius))
	{
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("isLoop", &m_MeshDesc.BufferInstance.isLoop))
	{
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Attach", &m_MeshDesc.BufferInstance.isAttach))
	{
		bChange = true;
	}


	if (ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &m_MeshDesc.BufferInstance.iNumInstance))
	{
		bChange = true;
	}


	_float* Temp = (_float*)&m_MeshDesc.BufferInstance.vOffsetPos;
	if (ImGui::InputFloat3("OffsetPos", Temp))
	{
		memcpy(&m_MeshDesc.BufferInstance.vOffsetPos, Temp, sizeof(_float3));
		bChange = true;
	}

	Temp = (_float*)&m_MeshDesc.BufferInstance.vPivotPos;
	if (ImGui::InputFloat3("PivotPos", Temp))
	{
		memcpy(&m_MeshDesc.BufferInstance.vPivotPos, Temp, sizeof(_float3));
		bChange = true;
	}

	if (!m_MeshDesc.BufferInstance.bRadius)
	{

		Temp = (_float*)&m_MeshDesc.BufferInstance.vRange;
		if (ImGui::InputFloat3("Range", Temp))
		{
			memcpy(&m_MeshDesc.BufferInstance.vRange, Temp, sizeof(_float3));
			bChange = true;
		}
	}
	else
	{
		Temp = (_float*)&m_MeshDesc.BufferInstance.fRadius;
		if (ImGui::InputFloat("Radius", Temp))
		{
			memcpy(&m_MeshDesc.BufferInstance.fRadius, Temp, sizeof(_float));
			bChange = true;
		}
	}

	Temp = (_float*)&m_MeshDesc.BufferInstance.vSize;
	if (ImGui::InputFloat2("Size", Temp))
	{
		memcpy(&m_MeshDesc.BufferInstance.vSize, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_MeshDesc.BufferInstance.vRectSize;
	if (ImGui::InputFloat2("RectSize", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_MeshDesc.BufferInstance.vRectSize, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_MeshDesc.BufferInstance.vLifeTime;
	if (ImGui::InputFloat2("LifeTime", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_MeshDesc.BufferInstance.vLifeTime, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_MeshDesc.fLifeAlpha;
	if (ImGui::InputFloat2("LifeAlpha", Temp))
	{
		memcpy(&m_MeshDesc.fLifeAlpha, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_MeshDesc.fStartTime;
	if (ImGui::InputFloat("StartTime", Temp))
	{
		memcpy(&m_MeshDesc.fStartTime, Temp, sizeof(_float));
		bChange = true;
	}

	Temp = (_float*)&m_MeshDesc.BufferInstance.LowStartRot;
	if (ImGui::InputFloat3("LowStartRot", Temp))
	{
		if (Temp[0] > m_MeshDesc.BufferInstance.HighStartRot.x)
			m_MeshDesc.BufferInstance.HighStartRot.x = Temp[0];
		if (Temp[1] > m_MeshDesc.BufferInstance.HighStartRot.y)
			m_MeshDesc.BufferInstance.HighStartRot.y = Temp[1];
		if (Temp[2] > m_MeshDesc.BufferInstance.HighStartRot.z)
			m_MeshDesc.BufferInstance.HighStartRot.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.LowStartRot, Temp, sizeof(_float3));
		bChange = true;
	}

	Temp = (_float*)&m_MeshDesc.BufferInstance.HighStartRot;
	if (ImGui::InputFloat3("HighStartRot", Temp))
	{
		if (Temp[0] < m_MeshDesc.BufferInstance.LowStartRot.x)
			m_MeshDesc.BufferInstance.LowStartRot.x = Temp[0];
		if (Temp[1] < m_MeshDesc.BufferInstance.LowStartRot.y)
			m_MeshDesc.BufferInstance.LowStartRot.y = Temp[1];
		if (Temp[2] < m_MeshDesc.BufferInstance.LowStartRot.z)
			m_MeshDesc.BufferInstance.LowStartRot.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.HighStartRot, Temp, sizeof(_float3));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.LowAngleVelocity;
	if (ImGui::InputFloat3("LowAngleVelocity", Temp))
	{
		if (Temp[0] > m_MeshDesc.BufferInstance.HighAngleVelocity.x)
			m_MeshDesc.BufferInstance.HighAngleVelocity.x = Temp[0];
		if (Temp[1] > m_MeshDesc.BufferInstance.HighAngleVelocity.y)
			m_MeshDesc.BufferInstance.HighAngleVelocity.y = Temp[1];
		if (Temp[2] > m_MeshDesc.BufferInstance.HighAngleVelocity.z)
			m_MeshDesc.BufferInstance.HighAngleVelocity.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.LowAngleVelocity, Temp, sizeof(_float3));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.HighAngleVelocity;
	if (ImGui::InputFloat3("HighAngleVelocity", Temp))
	{
		if (Temp[0] < m_MeshDesc.BufferInstance.LowAngleVelocity.x)
			m_MeshDesc.BufferInstance.LowAngleVelocity.x = Temp[0];
		if (Temp[1] < m_MeshDesc.BufferInstance.LowAngleVelocity.y)
			m_MeshDesc.BufferInstance.LowAngleVelocity.y = Temp[1];
		if (Temp[2] < m_MeshDesc.BufferInstance.LowAngleVelocity.z)
			m_MeshDesc.BufferInstance.LowAngleVelocity.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.HighAngleVelocity, Temp, sizeof(_float3));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.GravityScale;
	if (ImGui::InputFloat("GravityScale", Temp))
	{
		memcpy(&m_MeshDesc.BufferInstance.GravityScale, Temp, sizeof(_float));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.CrossArea;
	if (ImGui::InputFloat("CrossArea", Temp))
	{
		memcpy(&m_MeshDesc.BufferInstance.CrossArea, Temp, sizeof(_float));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.vMinSpeed;
	if (ImGui::InputFloat3("vMinSpeed", Temp))
	{
		if (Temp[0] > m_MeshDesc.BufferInstance.vMaxSpeed.x)
			m_MeshDesc.BufferInstance.vMaxSpeed.x = Temp[0];
		if (Temp[1] > m_MeshDesc.BufferInstance.vMaxSpeed.y)
			m_MeshDesc.BufferInstance.vMaxSpeed.y = Temp[1];
		if (Temp[2] > m_MeshDesc.BufferInstance.vMaxSpeed.z)
			m_MeshDesc.BufferInstance.vMaxSpeed.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.vMinSpeed, Temp, sizeof(_float3));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.vMaxSpeed;
	if (ImGui::InputFloat3("vMaxSpeed", Temp))
	{
		if (Temp[0] < m_MeshDesc.BufferInstance.vMinSpeed.x)
			m_MeshDesc.BufferInstance.vMinSpeed.x = Temp[0];
		if (Temp[1] < m_MeshDesc.BufferInstance.vMinSpeed.y)
			m_MeshDesc.BufferInstance.vMinSpeed.y = Temp[1];
		if (Temp[2] < m_MeshDesc.BufferInstance.vMinSpeed.z)
			m_MeshDesc.BufferInstance.vMinSpeed.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.vMaxSpeed, Temp, sizeof(_float3));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.fWeight;
	if (ImGui::InputFloat2("fWeight", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];

		memcpy(&m_MeshDesc.BufferInstance.fWeight, Temp, sizeof(_float2));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.vMinFrequency;
	if (ImGui::InputFloat3("vMinFrequency", Temp))
	{
		if (Temp[0] > m_MeshDesc.BufferInstance.vMaxFrequency.x)
			m_MeshDesc.BufferInstance.vMaxFrequency.x = Temp[0];
		if (Temp[1] > m_MeshDesc.BufferInstance.vMaxFrequency.y)
			m_MeshDesc.BufferInstance.vMaxFrequency.y = Temp[1];
		if (Temp[2] > m_MeshDesc.BufferInstance.vMaxFrequency.z)
			m_MeshDesc.BufferInstance.vMaxFrequency.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.vMinFrequency, Temp, sizeof(_float3));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.vMaxFrequency;
	if (ImGui::InputFloat3("vMaxFrequency", Temp))
	{
		if (Temp[0] < m_MeshDesc.BufferInstance.vMinFrequency.x)
			m_MeshDesc.BufferInstance.vMinFrequency.x = Temp[0];
		if (Temp[1] < m_MeshDesc.BufferInstance.vMinFrequency.y)
			m_MeshDesc.BufferInstance.vMinFrequency.y = Temp[1];
		if (Temp[2] < m_MeshDesc.BufferInstance.vMinFrequency.z)
			m_MeshDesc.BufferInstance.vMinFrequency.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.vMaxFrequency, Temp, sizeof(_float3));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.vMinAmplitude;
	if (ImGui::InputFloat3("vMinAmplitude", Temp))
	{
		if (Temp[0] > m_MeshDesc.BufferInstance.vMaxAmplitude.x)
			m_MeshDesc.BufferInstance.vMaxAmplitude.x = Temp[0];
		if (Temp[1] > m_MeshDesc.BufferInstance.vMaxAmplitude.y)
			m_MeshDesc.BufferInstance.vMaxAmplitude.y = Temp[1];
		if (Temp[2] > m_MeshDesc.BufferInstance.vMaxAmplitude.z)
			m_MeshDesc.BufferInstance.vMaxAmplitude.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.vMinAmplitude, Temp, sizeof(_float3));
		bChange = true;
	}
	Temp = (_float*)&m_MeshDesc.BufferInstance.vMaxAmplitude;
	if (ImGui::InputFloat3("vMaxAmplitude", Temp))
	{
		if (Temp[0] < m_MeshDesc.BufferInstance.vMinAmplitude.x)
			m_MeshDesc.BufferInstance.vMinAmplitude.x = Temp[0];
		if (Temp[1] < m_MeshDesc.BufferInstance.vMinAmplitude.y)
			m_MeshDesc.BufferInstance.vMinAmplitude.y = Temp[1];
		if (Temp[2] < m_MeshDesc.BufferInstance.vMinAmplitude.z)
			m_MeshDesc.BufferInstance.vMinAmplitude.z = Temp[2];
		memcpy(&m_MeshDesc.BufferInstance.vMaxAmplitude, Temp, sizeof(_float3));
		bChange = true;
	}



	File_Selctor(&bChange);
	Mesh_File_Selctor(&bChange);

	if (bChange)
	{
		Edit_Particle(m_iCurEditIndex);
	}
	ImGui::End();
}

void CImguiManager::Mesh_File_Selctor(_bool* bChange)
{

	_int iSize = MeshTags.size();
	if (ImGui::BeginListBox("MeshBox"))
	{
		for (int i = 0; i < iSize; i++)
		{
			// 리스트박스 아이템의 텍스트
			const bool isSelected = (m_iCurMesh == i);
			char Label[256] = {};
			strcpy_s(Label, m_pGameInstance->WstringToString(MeshTags[i]).c_str());
			if (ImGui::Selectable(Label, isSelected))
			{
				m_iCurMesh = i; // 선택된 아이템 업데이트	
				*bChange = true;
			}


			// 선택된 아이템이 보이도록 스크롤
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();

			}
		}
		ImGui::EndListBox();
	}

	if (-1 != m_iCurEditIndex)
	{
		m_MeshDesc.strModelTag = MeshTags[m_iCurMesh];	
	}
}

void CImguiManager::EditorTrail_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(980, 200), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(300, 520), ImGuiCond_None);

	ImGui::Begin("Trail_Edit");




	_int iSize = m_EditTrail.size();
	if (ImGui::BeginListBox("listBox"))
	{
		for (int i = 0; i < iSize; i++)
		{
			// 리스트박스 아이템의 텍스트
			const bool isSelected = (m_iCurEditIndex == i);
			char Label[256] = {};
			strcpy_s(Label, m_pGameInstance->WstringToString(dynamic_cast<CEffect*>(m_EditTrail[i])->Get_Tag()).c_str());
			if (ImGui::Selectable(Label, isSelected))
			{
				m_iCurEditIndex = i; // 선택된 아이템 업데이트	
				m_TrailDesc.ParticleTag = dynamic_cast<CEffect*>(m_EditTrail[i])->Get_Tag();
			}

			// 선택된 아이템이 보이도록 스크롤
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	if (-1 != m_iCurEditIndex)
	{
		Load_Desc(m_iCurEditIndex);	
	}

	_bool bChange = false;
	ImGui::Text(to_string(m_iCurEditIndex).c_str());	

	static char strReName[MAX_PATH]{};

	ImGui::InputText("ParticleTag", strReName, MAX_PATH);
	if (ImGui::Button(u8"변경하기"))
	{
		bChange = true;
		string tag = strReName;
		m_TrailDesc.ParticleTag = m_pGameInstance->StringToWstring(tag);
	}


	_int* Temp = (_int*)&m_TrailDesc.Trail_Desc.iMaxTrail;
	if (ImGui::InputInt("MaxTrail", Temp))
	{
		memcpy(&m_TrailDesc.Trail_Desc.iMaxTrail, Temp, sizeof(_int));
		bChange = true;	
	}

	_float* PosTemp = (_float*)&m_TrailDesc.vStartPos;
	if (ImGui::InputFloat3("StartPos", PosTemp))
	{
		memcpy(&m_TrailDesc.vStartPos, PosTemp, sizeof(_float3));	
		bChange = true;	
	}

	PosTemp = (_float*)&m_TrailDesc.Trail_Desc.vInitPosA;
	if (ImGui::InputFloat3("PosA", PosTemp))
	{
		memcpy(&m_TrailDesc.Trail_Desc.vInitPosA, PosTemp, sizeof(_float3));
		bChange = true;
	}

	PosTemp = (_float*)&m_TrailDesc.Trail_Desc.vInitPosB;
	if (ImGui::InputFloat3("PosB", PosTemp))
	{
		memcpy(&m_TrailDesc.Trail_Desc.vInitPosB, PosTemp, sizeof(_float3));
		bChange = true;
	}

	File_Selctor(&bChange);

	if (bChange)
	{
		Edit_Particle(m_iCurEditIndex);
	}
	ImGui::End();

}


HRESULT CImguiManager::AllEffect_Save()
{
	switch (m_iMode)
	{
	case Client::CImguiManager::MODE_PARTICLE:
	{
		string strDirectory = "../../Client/Bin/DataFiles/Particle/Point";

		fs::path path(strDirectory);	
		if (!exists(path))	
			fs::create_directory(path);	

		for (auto iter : m_EditParticle)
			dynamic_cast<CParticle_Point*>(iter)->Save_Data(strDirectory);

	}
		break;
	case Client::CImguiManager::MODE_TRAIL:
	{
		string strDirectory = "../../Client/Bin/DataFiles/Particle/Trail";

		fs::path path(strDirectory);
		if (!exists(path))
			fs::create_directory(path);

		for (auto iter : m_EditTrail)
			dynamic_cast<CTRailEffect*>(iter)->Save_Data(strDirectory);

	}
		break;
	case Client::CImguiManager::MODE_AURA:
	{
		string strDirectory = "../../Client/Bin/DataFiles/Particle/Aura";

		fs::path path(strDirectory);
		if (!exists(path))
			fs::create_directory(path);

		for (auto iter : m_EditAura)
			dynamic_cast<CAura*>(iter)->Save_Data(strDirectory);

	}
	break;
	case Client::CImguiManager::MODE_MESH:
	{
		string strDirectory = "../../Client/Bin/DataFiles/Particle/Mesh";

		fs::path path(strDirectory);
		if (!exists(path))
			fs::create_directory(path);

		for (auto iter : m_EditMesh)
			dynamic_cast<CParticle_Mesh*>(iter)->Save_Data(strDirectory);

	}
	break;
	case Client::CImguiManager::MODE_END:
		break;
	default:
		break;
	}
	

	return S_OK;
}

HRESULT CImguiManager::AllEffect_Load()
{

	switch (m_iMode)
	{
	case Client::CImguiManager::MODE_PARTICLE:
	{
		string strDirectory = "../../Client/Bin/DataFiles/Particle/Point";

		fs::path path(strDirectory);
		if (!exists(path))
			fs::create_directory(path);

		for (auto iter : m_EditParticle)
			dynamic_cast<CParticle_Point*>(iter)->Load_Data(strDirectory);
	}
	break;
	case Client::CImguiManager::MODE_TRAIL:
	{
		string strDirectory = "../../Client/Bin/DataFiles/Particle/Trail";

		fs::path path(strDirectory);
		if (!exists(path))
			fs::create_directory(path);

		for (auto iter : m_EditTrail)
			dynamic_cast<CTRailEffect*>(iter)->Load_Data(strDirectory);

	}
	break;
	case Client::CImguiManager::MODE_AURA:
	{
		string strDirectory = "../../Client/Bin/DataFiles/Particle/Aura";

		fs::path path(strDirectory);
		if (!exists(path))
			fs::create_directory(path);

		for (auto iter : m_EditAura)
			dynamic_cast<CTRailEffect*>(iter)->Load_Data(strDirectory);

	}
	break;
	case Client::CImguiManager::MODE_END:
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CImguiManager::Edit_Particle(_uint Index)
{

	switch (m_iMode)
	{
	case MODE_PARTICLE:
	{

		if (m_EditParticle.size() <= Index)
			return E_FAIL;
		else
		{
			Safe_Release(m_EditParticle[Index]);
		}

		CParticle_Point::PARTICLE_POINT_DESC EffectDesc{};
		EffectDesc.BufferInstance.iNumInstance = m_EffectDesc.BufferInstance.iNumInstance;
		EffectDesc.BufferInstance.isLoop = m_EffectDesc.BufferInstance.isLoop;
		EffectDesc.BufferInstance.vLifeTime = m_EffectDesc.BufferInstance.vLifeTime;
		EffectDesc.BufferInstance.vOffsetPos = m_EffectDesc.BufferInstance.vOffsetPos;
		EffectDesc.BufferInstance.vPivotPos = m_EffectDesc.BufferInstance.vPivotPos;
		EffectDesc.BufferInstance.vRange = m_EffectDesc.BufferInstance.vRange;
		EffectDesc.BufferInstance.vSize = m_EffectDesc.BufferInstance.vSize;
		EffectDesc.BufferInstance.vSpeed = m_EffectDesc.BufferInstance.vSpeed;
		EffectDesc.BufferInstance.vRectSize = m_EffectDesc.BufferInstance.vRectSize;
		EffectDesc.BufferInstance.fRadius = m_EffectDesc.BufferInstance.fRadius;
		EffectDesc.BufferInstance.bRadius = m_EffectDesc.BufferInstance.bRadius;
		EffectDesc.BufferInstance.LowStartRot = m_EffectDesc.BufferInstance.LowStartRot;
		EffectDesc.BufferInstance.HighStartRot = m_EffectDesc.BufferInstance.HighStartRot;
		EffectDesc.BufferInstance.LowAngleVelocity = m_EffectDesc.BufferInstance.LowAngleVelocity;
		EffectDesc.BufferInstance.HighAngleVelocity = m_EffectDesc.BufferInstance.HighAngleVelocity;
		EffectDesc.BufferInstance.GravityScale = m_EffectDesc.BufferInstance.GravityScale;
		EffectDesc.BufferInstance.isBillboard = m_EffectDesc.BufferInstance.isBillboard;
		EffectDesc.BufferInstance.CrossArea = m_EffectDesc.BufferInstance.CrossArea;
		EffectDesc.BufferInstance.isAttach = m_EffectDesc.BufferInstance.isAttach;
		EffectDesc.BufferInstance.fDelay = m_EffectDesc.BufferInstance.fDelay;

		EffectDesc.vStartPos = m_EffectDesc.vStartPos;
		EffectDesc.fRotate = m_EffectDesc.fRotate;
		EffectDesc.fLifeAlpha = m_EffectDesc.fLifeAlpha;
		EffectDesc.eType = m_EffectDesc.eType;
		EffectDesc.ParticleTag = m_EffectDesc.ParticleTag;
		EffectDesc.fStartTime = m_EffectDesc.fStartTime;
		EffectDesc.vStartColor = m_EffectDesc.vStartColor;
		EffectDesc.vEndColor = m_EffectDesc.vEndColor;
		EffectDesc.iShaderPass = m_EffectDesc.iShaderPass;
		

		EffectDesc.TextureTag = m_EffectDesc.TextureTag;
		EffectDesc.fDistortion = m_EffectDesc.fDistortion;

		EffectDesc.isNormal = m_EffectDesc.isNormal;
		EffectDesc.NormalTag = m_EffectDesc.NormalTag;
		EffectDesc.fUVCount = m_EffectDesc.fUVCount;

		m_EditParticle[Index] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Particle_Point"), &EffectDesc);
		if (nullptr == m_EditParticle[Index])
			return E_FAIL;

		if (m_bSpread)
			dynamic_cast<CEffect*>(m_EditParticle[Index])->Edit_Action(CEffect::ACTION_SPREAD);
		if (m_bSizeup)
			dynamic_cast<CEffect*>(m_EditParticle[Index])->Edit_Action(CEffect::ACTION_SIZEUP);
		if (m_bSizedown)
			dynamic_cast<CEffect*>(m_EditParticle[Index])->Edit_Action(CEffect::ACTION_SIZEDOWN);
		if (m_bNobillboard)
			dynamic_cast<CEffect*>(m_EditParticle[Index])->Edit_Action(CEffect::ACTION_NOBILLBOARD);
		if (m_bFallSpread)
			dynamic_cast<CEffect*>(m_EditParticle[Index])->Edit_Action(CEffect::ACTION_FALLSPREAD);
		if (m_bBloodSpread)
			dynamic_cast<CEffect*>(m_EditParticle[Index])->Edit_Action(CEffect::ACTION_BLOOD);
		if(m_bIntervalSpread)
			dynamic_cast<CEffect*>(m_EditParticle[Index])->Edit_Action(CEffect::ACTION_INTERVAL);

	}
		break;
	case MODE_TRAIL:
	{

		if (m_EditTrail.size() <= Index)
			return E_FAIL;
		else
		{
			Safe_Release(m_EditTrail[Index]);
		}

		CTRailEffect::TRAIL_DESC TrailDesc{};

		TrailDesc.Trail_Desc.iMaxTrail = m_TrailDesc.Trail_Desc.iMaxTrail;
		TrailDesc.Trail_Desc.vInitPosA = m_TrailDesc.Trail_Desc.vInitPosA;
		TrailDesc.Trail_Desc.vInitPosB = m_TrailDesc.Trail_Desc.vInitPosB;
		TrailDesc.fSpeedPecSec = m_TrailDesc.fSpeedPecSec;

		TrailDesc.vStartPos = m_TrailDesc.vStartPos;
		TrailDesc.eType = CEffect::TYPE_TRAIL;	
		TrailDesc.ParticleTag = m_TrailDesc.ParticleTag;
		TrailDesc.iShaderPass = m_TrailDesc.iShaderPass;
		TrailDesc.TextureTag = m_TrailDesc.TextureTag;

		m_EditTrail[Index] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TrailEffect"), &TrailDesc);
		if (nullptr == m_EditTrail[Index])
			return E_FAIL;

	}
		break;
	case MODE_AURA:
	{

		if (m_EditAura.size() <= Index)
			return E_FAIL;
		else
		{
			Safe_Release(m_EditAura[Index]);
		}

		CAura::AURA_DESC AuraDesc{};

		AuraDesc.BufferInstance.iNumInstance = m_AuraDesc.BufferInstance.iNumInstance;
		AuraDesc.BufferInstance.isLoop = m_AuraDesc.BufferInstance.isLoop;
		AuraDesc.BufferInstance.vLifeTime = m_AuraDesc.BufferInstance.vLifeTime;
		AuraDesc.BufferInstance.vOffsetPos = m_AuraDesc.BufferInstance.vOffsetPos;
		AuraDesc.BufferInstance.vPivotPos = m_AuraDesc.BufferInstance.vPivotPos;
		AuraDesc.BufferInstance.vRange = m_AuraDesc.BufferInstance.vRange;
		AuraDesc.BufferInstance.vSize = m_AuraDesc.BufferInstance.vSize;
		AuraDesc.BufferInstance.vSpeed = m_AuraDesc.BufferInstance.vSpeed;
		AuraDesc.BufferInstance.vRectSize = m_AuraDesc.BufferInstance.vRectSize;
		AuraDesc.BufferInstance.fRadius = m_AuraDesc.BufferInstance.fRadius;
		AuraDesc.BufferInstance.bRadius = m_AuraDesc.BufferInstance.bRadius;
		AuraDesc.BufferInstance.isAttach = m_AuraDesc.BufferInstance.isAttach;

		AuraDesc.vStartPos = m_AuraDesc.vStartPos;
		AuraDesc.fRotate = m_AuraDesc.fRotate;
		AuraDesc.fLifeAlpha = m_AuraDesc.fLifeAlpha;
		AuraDesc.eType = m_AuraDesc.eType;
		AuraDesc.ParticleTag = m_AuraDesc.ParticleTag;
		AuraDesc.fStartTime = m_AuraDesc.fStartTime;
		AuraDesc.vStartColor = m_AuraDesc.vStartColor;
		AuraDesc.vEndColor = m_AuraDesc.vEndColor;
		AuraDesc.iShaderPass = m_AuraDesc.iShaderPass;
		AuraDesc.TextureTag = m_AuraDesc.TextureTag;
		AuraDesc.ToneTextureTag = m_AuraDesc.ToneTextureTag;
		AuraDesc.fUVCount = m_AuraDesc.fUVCount;
		AuraDesc.fToneUVCount = m_AuraDesc.fToneUVCount;
		AuraDesc.fFlowPow = m_AuraDesc.fFlowPow;
		AuraDesc.fFlowSpeed = m_AuraDesc.fFlowSpeed;


		m_EditAura[Index] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Aura"), &AuraDesc);
		if (nullptr == m_EditAura[Index])
			return E_FAIL;

		if (m_bSpread)
			dynamic_cast<CEffect*>(m_EditAura[Index])->Edit_Action(CEffect::ACTION_SPREAD);
		if (m_bSizeup)
			dynamic_cast<CEffect*>(m_EditAura[Index])->Edit_Action(CEffect::ACTION_SIZEUP);
		if (m_bSizedown)
			dynamic_cast<CEffect*>(m_EditAura[Index])->Edit_Action(CEffect::ACTION_SIZEDOWN);
	}
		break;
	case MODE_MESH:
	{

		if (m_EditMesh.size() <= Index)
			return E_FAIL;
		else
		{
			Safe_Release(m_EditMesh[Index]);
		}

		CParticle_Mesh::PARTICLE_MESH_DESC MeshDesc{};

		memcpy(&MeshDesc.BufferInstance, &m_MeshDesc.BufferInstance, sizeof(CVIBuffer_Instance::INSTANCE_DESC));

		MeshDesc.vStartPos = m_MeshDesc.vStartPos;
		MeshDesc.fRotate = m_MeshDesc.fRotate;
		MeshDesc.fLifeAlpha = m_MeshDesc.fLifeAlpha;
		MeshDesc.eType = m_MeshDesc.eType;
		MeshDesc.ParticleTag = m_MeshDesc.ParticleTag;
		MeshDesc.fStartTime = m_MeshDesc.fStartTime;
		MeshDesc.vStartColor = m_MeshDesc.vStartColor;
		MeshDesc.vEndColor = m_MeshDesc.vEndColor;
		MeshDesc.iShaderPass = m_MeshDesc.iShaderPass;


		MeshDesc.TextureTag = m_MeshDesc.TextureTag;
		MeshDesc.fDistortion = m_MeshDesc.fDistortion;


		MeshDesc.strModelTag = m_MeshDesc.strModelTag;

		m_EditMesh[Index] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Particle_Mesh"), &MeshDesc);
		if (nullptr == m_EditMesh[Index])
			return E_FAIL;

		if (m_bSpread)
			dynamic_cast<CEffect*>(m_EditMesh[Index])->Edit_Action(CEffect::ACTION_SPREAD);
		if (m_bSizeup)
			dynamic_cast<CEffect*>(m_EditMesh[Index])->Edit_Action(CEffect::ACTION_SIZEUP);
		if (m_bSizedown)
			dynamic_cast<CEffect*>(m_EditMesh[Index])->Edit_Action(CEffect::ACTION_SIZEDOWN);
		if (m_bNobillboard)
			dynamic_cast<CEffect*>(m_EditMesh[Index])->Edit_Action(CEffect::ACTION_NOBILLBOARD);
	}
	break;
	default:
		break;
	}

	//m_iCurEditIndex = Index;

	return S_OK;
}

HRESULT CImguiManager::Load_Desc(_uint Index)
{
	CEffect* pEffect = nullptr;

	switch (m_iMode)
	{
	case MODE_PARTICLE:
	{
		pEffect =dynamic_cast<CEffect*>(m_EditParticle[Index]);
		CVIBuffer_Instance::INSTANCE_DESC* pDesc = static_cast<CVIBuffer_Instance::INSTANCE_DESC*>(pEffect->Get_Instance());

		m_EffectDesc.BufferInstance = *pDesc;

		m_EffectDesc.eType = pEffect->Get_Type();
		m_EffectDesc.vStartPos = pEffect->Get_StartPos();
		m_EffectDesc.fRotate = pEffect->Get_Rotate();
		m_EffectDesc.fLifeAlpha = pEffect->Get_LifeAlpha();
		m_EffectDesc.fStartTime = *pEffect->Get_pStartTime();
		m_EffectDesc.ParticleTag = pEffect->Get_Tag();
		m_EffectDesc.vStartColor = pEffect->Get_SColor();
		m_EffectDesc.vEndColor = pEffect->Get_EColor();
		m_EffectDesc.iShaderPass = pEffect->Get_ShaderPass();
		m_EffectDesc.TextureTag = pEffect->Get_TextureTag();
		m_EffectDesc.fDistortion = pEffect->Get_fDistortion();

		m_EffectDesc.isNormal = pEffect->Get_isNormal();
		m_EffectDesc.NormalTag = pEffect->Get_NormalTag();
		m_EffectDesc.fUVCount = dynamic_cast<CParticle_Point*>(pEffect)->Get_UVCount();

		for (size_t i = 0; i < TextureTags.size(); i++)
		{
			if (m_EffectDesc.TextureTag == TextureTags[i])
			{
				m_iCurTexture = i;
				break;
			}
		}

		if (m_EffectDesc.isNormal)
		{
			for (size_t i = 0; i < NormalTags.size(); i++)
			{
				if (m_EffectDesc.NormalTag == NormalTags[i])
				{
					m_iCurNormalTexture = i;
					break;
				}
			}
		}


		_uint CheckAction = pEffect->Get_Action();

		if (CheckAction & pEffect->iAction[CEffect::ACTION_SPREAD])
			m_bSpread = true;
		else
			m_bSpread = false;


		if (CheckAction & pEffect->iAction[CEffect::ACTION_SIZEUP])
			m_bSizeup = true;
		else
			m_bSizeup = false;

		if (CheckAction & pEffect->iAction[CEffect::ACTION_SIZEDOWN])
			m_bSizedown = true;
		else
			m_bSizedown = false;

		if (CheckAction & pEffect->iAction[CEffect::ACTION_NOBILLBOARD])
			m_bNobillboard = true;
		else
			m_bNobillboard = false;

		if (CheckAction & pEffect->iAction[CEffect::ACTION_FALLSPREAD])
			m_bFallSpread = true;
		else
			m_bFallSpread = false;

		if (CheckAction & pEffect->iAction[CEffect::ACTION_BLOOD])	
			m_bBloodSpread = true;
		else
			m_bBloodSpread = false;

		if (CheckAction & pEffect->iAction[CEffect::ACTION_INTERVAL])
			m_bIntervalSpread = true;
		else
			m_bIntervalSpread = false;

		
	}
		break;
	case MODE_TRAIL:
	{
		pEffect = dynamic_cast<CEffect*>(m_EditTrail[Index]);
		CVIBuffer_Trail::VIBUFFER_TRAIL_DESC* pDesc = static_cast<CVIBuffer_Trail::VIBUFFER_TRAIL_DESC*>(pEffect->Get_Instance());

		m_TrailDesc.fSpeedPecSec = 5.0f;

		m_TrailDesc.eType = pEffect->Get_Type();
		m_TrailDesc.vStartPos = pEffect->Get_StartPos();//이게 곧 트레일 작용위치

		m_TrailDesc.ParticleTag = pEffect->Get_Tag();
		m_TrailDesc.iShaderPass = pEffect->Get_ShaderPass();
		m_TrailDesc.TextureTag = pEffect->Get_TextureTag();

		m_TrailDesc.Trail_Desc.iMaxTrail = pDesc->iMaxTrail;
		m_TrailDesc.Trail_Desc.vInitPosA = pDesc->vInitPosA;
		m_TrailDesc.Trail_Desc.vInitPosB = pDesc->vInitPosB;

		for (size_t i = 0; i < TextureTags.size(); i++)
		{
			if (m_TrailDesc.TextureTag == TextureTags[i])
			{
				m_iCurTexture = i;
				break;
			}
		}


	}
		break;
	case MODE_AURA:
	{
		CAura* pAuraEffect = dynamic_cast<CAura*>(m_EditAura[Index]);
		CVIBuffer_Instance::INSTANCE_DESC* pDesc = static_cast<CVIBuffer_Instance::INSTANCE_DESC*>(pAuraEffect->Get_Instance());


		m_AuraDesc.BufferInstance = *pDesc;


		m_AuraDesc.eType = pAuraEffect->Get_Type();
		m_AuraDesc.vStartPos = pAuraEffect->Get_StartPos();
		m_AuraDesc.fRotate = pAuraEffect->Get_Rotate();
		m_AuraDesc.fLifeAlpha = pAuraEffect->Get_LifeAlpha();
		m_AuraDesc.fStartTime = *pAuraEffect->Get_pStartTime();
		m_AuraDesc.ParticleTag = pAuraEffect->Get_Tag();
		m_AuraDesc.vStartColor = pAuraEffect->Get_SColor();
		m_AuraDesc.vEndColor = pAuraEffect->Get_EColor();
		m_AuraDesc.iShaderPass = pAuraEffect->Get_ShaderPass();
		m_AuraDesc.TextureTag = pAuraEffect->Get_TextureTag();
		m_AuraDesc.ToneTextureTag = pAuraEffect->Get_ToneTextureTag();	
		m_AuraDesc.fUVCount = pAuraEffect->Get_UVCount();
		m_AuraDesc.fToneUVCount = pAuraEffect->Get_ToneUVCount();
		m_AuraDesc.fFlowPow = pAuraEffect->Get_FlowPow();
		m_AuraDesc.fFlowSpeed = pAuraEffect->Get_FlowSpeed();



		_uint CheckAction = pAuraEffect->Get_Action();


		for (size_t i = 0; i < TextureTags.size(); i++)
		{
			if (m_AuraDesc.TextureTag == TextureTags[i])
			{
				m_iCurTexture = i;
				break;
			}
		}

		for (size_t i = 0; i < ToneTextureTags.size(); i++)
		{
			if (m_AuraDesc.ToneTextureTag == ToneTextureTags[i])
			{
				m_iCurToneTexture = i;
				break;
			}
		}


		if (CheckAction & pAuraEffect->iAction[CEffect::ACTION_SPREAD])
			m_bSpread = true;
		else
			m_bSpread = false;



		if (CheckAction & pAuraEffect->iAction[CEffect::ACTION_SIZEUP])
			m_bSizeup = true;
		else
			m_bSizeup = false;

		if (CheckAction & pAuraEffect->iAction[CEffect::ACTION_SIZEDOWN])
			m_bSizedown = true;
		else
			m_bSizedown = false;



	}
	break;
	case MODE_MESH:
	{
		pEffect = dynamic_cast<CEffect*>(m_EditMesh[Index]);
		CVIBuffer_Instance::INSTANCE_DESC* pDesc = static_cast<CVIBuffer_Instance::INSTANCE_DESC*>(pEffect->Get_Instance());

		memcpy(&m_MeshDesc.BufferInstance, pDesc, sizeof(CVIBuffer_Instance::INSTANCE_DESC));
		//m_MeshDesc.BufferInstance = *pDesc;

		m_MeshDesc.eType = pEffect->Get_Type();
		m_MeshDesc.vStartPos = pEffect->Get_StartPos();
		m_MeshDesc.fRotate = pEffect->Get_Rotate();
		m_MeshDesc.fLifeAlpha = pEffect->Get_LifeAlpha();
		m_MeshDesc.fStartTime = *pEffect->Get_pStartTime();
		m_MeshDesc.ParticleTag = pEffect->Get_Tag();
		m_MeshDesc.vStartColor = pEffect->Get_SColor();
		m_MeshDesc.vEndColor = pEffect->Get_EColor();
		m_MeshDesc.iShaderPass = pEffect->Get_ShaderPass();
		m_MeshDesc.TextureTag = pEffect->Get_TextureTag();
		m_MeshDesc.fDistortion = pEffect->Get_fDistortion();

		m_MeshDesc.strModelTag = dynamic_cast<CParticle_Mesh*>(pEffect)->Get_ModelTag();

		for (size_t i = 0; i < TextureTags.size(); i++)
		{
			if (m_MeshDesc.TextureTag == TextureTags[i])
			{
				m_iCurTexture = i;
				break;
			}
		}

		for (size_t i = 0; i < MeshTags.size(); i++)
		{
			if (m_MeshDesc.strModelTag == MeshTags[i])
			{
				m_iCurMesh = i;
				break;
			}
		}

		_uint CheckAction = pEffect->Get_Action();

		if (CheckAction & pEffect->iAction[CEffect::ACTION_SPREAD])
			m_bSpread = true;
		else
			m_bSpread = false;


		if (CheckAction & pEffect->iAction[CEffect::ACTION_SIZEUP])
			m_bSizeup = true;
		else
			m_bSizeup = false;

		if (CheckAction & pEffect->iAction[CEffect::ACTION_SIZEDOWN])
			m_bSizedown = true;
		else
			m_bSizedown = false;

		if (CheckAction & pEffect->iAction[CEffect::ACTION_NOBILLBOARD])
			m_bNobillboard = true;
		else
			m_bNobillboard = false;



	}
	break;
	default:
		break;
	}



	return S_OK;
}

void CImguiManager::File_Selctor(_bool* bChange)
{

	_int iSize = TextureTags.size();
	if (ImGui::BeginListBox("texturebox"))
	{
		for (int i = 0; i < iSize; i++)
		{
			// 리스트박스 아이템의 텍스트
			const bool isSelected = (m_iCurTexture == i);
			char Label[256] = {};
			strcpy_s(Label, m_pGameInstance->WstringToString(TextureTags[i]).c_str());	
			if (ImGui::Selectable(Label, isSelected))
			{
				m_iCurTexture = i; // 선택된 아이템 업데이트	
				*bChange = true;	
			}


			// 선택된 아이템이 보이도록 스크롤
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();

			}
		}
		ImGui::EndListBox();
	}


	if (-1 != m_iCurEditIndex)
	{
		switch (m_iMode)
		{
		case Client::CImguiManager::MODE_PARTICLE:
		{
			m_EffectDesc.TextureTag = TextureTags[m_iCurTexture];	
			
		}
			break;
		case Client::CImguiManager::MODE_TRAIL:
		{
			m_TrailDesc.TextureTag = TextureTags[m_iCurTexture];	

		}
			break;
		case Client::CImguiManager::MODE_AURA:
		{
			m_AuraDesc.TextureTag = TextureTags[m_iCurTexture];

		}
		break;
		case Client::CImguiManager::MODE_MESH:
		{
			m_MeshDesc.TextureTag = TextureTags[m_iCurTexture];

		}
		break;
		case Client::CImguiManager::MODE_END:
			break;
		default:
			break;
		}
	}

}

void CImguiManager::Load_Selctor()
{
	switch (m_iMode)
	{
	case Client::CImguiManager::MODE_PARTICLE:
	{
		if (ImGui::Button(u8"로드 파티클"))
		{
			IGFD::FileDialogConfig config;
			config.path = "../../Client/Bin/DataFiles/Particle/Point";
			ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureKey", "Choose File", ".dat", config);
		}
		if (ImGuiFileDialog::Instance()->Display("ChooseTextureKey")) {
			if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
				// action

				string Tag;
				_int dotPos = fileName.find_last_of(".");
				Tag = fileName.substr(0, dotPos);

				
				CGameObject* pGameParticle = m_pGameInstance->Clone_Object(m_pGameInstance->StringToWstring(Tag),nullptr);
				if (nullptr == pGameParticle)
					MSG_BOX("생성 실패");

				m_EditParticle.push_back(pGameParticle);
				m_iCurEditIndex = m_EditParticle.size() - 1;
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}
	}
		break;
	case Client::CImguiManager::MODE_TRAIL:
	{
		if (ImGui::Button(u8"로드 파티클생성"))
		{
			IGFD::FileDialogConfig config;
			config.path = "../../Client/Bin/DataFiles/Particle/Trail";
			ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureKey", "Choose File", ".dat", config);
		}
		if (ImGuiFileDialog::Instance()->Display("ChooseTextureKey")) {
			if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
				// action


				string Tag;
				_int dotPos = fileName.find_last_of(".");
				Tag = fileName.substr(0, dotPos);


				CGameObject* pGameParticle = m_pGameInstance->Clone_Object(m_pGameInstance->StringToWstring(Tag), nullptr);
				if (nullptr == pGameParticle)
					MSG_BOX("생성 실패");

				m_EditTrail.push_back(pGameParticle);
				m_iCurEditIndex = m_EditTrail.size() - 1;
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}
	}
		break;
	case Client::CImguiManager::MODE_AURA:
	{
		if (ImGui::Button(u8"로드 파티클생성"))
		{
			IGFD::FileDialogConfig config;
			config.path = "../../Client/Bin/DataFiles/Particle/Aura";
			ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureKey", "Choose File", ".dat", config);
		}
		if (ImGuiFileDialog::Instance()->Display("ChooseTextureKey")) {
			if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
				// action


				string Tag;
				_int dotPos = fileName.find_last_of(".");
				Tag = fileName.substr(0, dotPos);


				CGameObject* pGameParticle = m_pGameInstance->Clone_Object(m_pGameInstance->StringToWstring(Tag), nullptr);
				if (nullptr == pGameParticle)
					MSG_BOX("생성 실패");

				m_EditAura.push_back(pGameParticle);
				m_iCurEditIndex = m_EditAura.size() - 1;
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}
	}
	break;
	case Client::CImguiManager::MODE_MESH:
	{
		if (ImGui::Button(u8"로드 파티클생성"))
		{
			IGFD::FileDialogConfig config;
			config.path = "../../Client/Bin/DataFiles/Particle/Mesh";
			ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureKey", "Choose File", ".dat", config);
		}
		if (ImGuiFileDialog::Instance()->Display("ChooseTextureKey")) {
			if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
				// action


				string Tag;
				_int dotPos = fileName.find_last_of(".");
				Tag = fileName.substr(0, dotPos);


				CGameObject* pGameParticle = m_pGameInstance->Clone_Object(m_pGameInstance->StringToWstring(Tag), nullptr);
				if (nullptr == pGameParticle)
					MSG_BOX("생성 실패");

				m_EditMesh.push_back(pGameParticle);
				m_iCurEditIndex = m_EditMesh.size() - 1;
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}
	}
	break;
	case Client::CImguiManager::MODE_END:
		break;
	default:
		break;
	}
	
}

void CImguiManager::Create_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(980, 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_None);

	ImGui::Begin("Particle_Create");


	if (ImGui::InputText("ParticleTag", text_input_buffer, IM_ARRAYSIZE(text_input_buffer)))
	{
		m_EffectDesc.ParticleTag = m_pGameInstance->StringToWstring(text_input_buffer);
	}

	if (ImGui::Button("Create Particle"))
	{
		Create_Particle();
	}
	if (ImGui::Button("Delete Particle"))
	{
		if (!m_EditParticle.empty())
		{
			Safe_Release(m_EditParticle.back());
			m_EditParticle.pop_back();
			if (m_EditParticle.empty())
				m_iCurEditIndex = -1;
			else
				m_iCurEditIndex = 0;
		}
	}
	if (ImGui::Button(u8"파티클 모두저장"))
	{
		AllEffect_Save();
	}
	Load_Selctor();


	ImGui::End();
}

void CImguiManager::Editor_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(880, 200), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(400, 520), ImGuiCond_None);

	ImGui::Begin("Particle_Edit");


		_int iSize = m_EditParticle.size();
		if (ImGui::BeginListBox("listBox"))
		{
			for (int i = 0; i < iSize; i++)
			{
				// 리스트박스 아이템의 텍스트
				const bool isSelected = (m_iCurEditIndex == i);
				char Label[256] = {};
				strcpy_s(Label, m_pGameInstance->WstringToString(dynamic_cast<CEffect*>(m_EditParticle[i])->Get_Tag()).c_str());
				if (ImGui::Selectable(Label, isSelected))
				{
					m_iCurEditIndex = i; // 선택된 아이템 업데이트	
				}

	
				// 선택된 아이템이 보이도록 스크롤
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndListBox();	
		}

	
	if (-1 != m_iCurEditIndex)
	{
		//1->0
		m_iCurEditIndex = m_iCurEditIndex;
		Load_Desc(m_iCurEditIndex);
	}


	_bool bChange = false;
	ImGui::Text(to_string(m_iCurEditIndex).c_str());

	if (ImGui::RadioButton("Point",(_int*) & m_EffectDesc.eType, CEffect::TYPE::TYPE_POINT))
		bChange = true;
	ImGui::SameLine();
	if (ImGui::RadioButton("Glow",(_int*) & m_EffectDesc.eType, CEffect::TYPE::TYPE_GLOW))
		bChange = true;
	ImGui::SameLine();
	if (ImGui::Checkbox("Attach", &m_EffectDesc.BufferInstance.isAttach))
		bChange = true;
	


	if (ImGui::RadioButton("NOCOLOR", &m_EffectDesc.iShaderPass, PASS_NOCOLOR))
	{
		if (m_bNobillboard)
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_NOBILLBOARD);
				m_EffectDesc.BufferInstance.isBillboard = false;
			}
			m_bNobillboard = false;
		}
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("COLOR", &m_EffectDesc.iShaderPass, PASS_COLOR))
	{
		if (m_bNobillboard)
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_NOBILLBOARD);
				m_EffectDesc.BufferInstance.isBillboard = false;
			}
			m_bNobillboard = false;
		}
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("ROTATE", &m_EffectDesc.iShaderPass, PASS_ROTATE))
	{
		if (m_bNobillboard)
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_NOBILLBOARD);
				m_EffectDesc.BufferInstance.isBillboard = false;
			}
			m_bNobillboard = false;
		}
		bChange = true;
	}
	if (ImGui::RadioButton("ROTANIM", &m_EffectDesc.iShaderPass, PASS_ROTANIM))
	{
		if (m_bNobillboard)
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_NOBILLBOARD);
				m_EffectDesc.BufferInstance.isBillboard = false;
			}
			m_bNobillboard = false;
		}
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("ANIM", &m_EffectDesc.iShaderPass, PASS_ANIM))
	{
		if (m_bNobillboard)
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_NOBILLBOARD);
				m_EffectDesc.BufferInstance.isBillboard = false;
			}
			m_bNobillboard = false;
		}
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("NOBILLDISTORTION", &m_EffectDesc.iShaderPass, PASS_NOBILLDISTORTION))
	{
		if (m_bNobillboard)
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_NOBILLBOARD);
				m_EffectDesc.BufferInstance.isBillboard = false;
			}
			m_bNobillboard = false;
		}
		bChange = true;
	}

	if (ImGui::RadioButton("NOBILLBOARD", &m_EffectDesc.iShaderPass, PASS_NOBILLBOARD))
	{
		if (m_bSpread)
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_SPREAD);
				m_EffectDesc.BufferInstance.isBillboard = true;
			}
			m_bSpread = false;
		}
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("DISTORTION", &m_EffectDesc.iShaderPass, PASS_DISTORTION))
	{
		if (m_bSpread)
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_SPREAD);	
				m_EffectDesc.BufferInstance.isBillboard = true;
			}
			m_bSpread = false;
		}
		bChange = true;
	}
	if (ImGui::RadioButton("FALLSPREAD", &m_EffectDesc.iShaderPass, PASS_FALLSPREAD))
	{
		m_EffectDesc.BufferInstance.isBillboard = true;
		bChange = true;
	}



	
	if (PASS_NOBILLBOARD == m_EffectDesc.iShaderPass)
	{
		if (ImGui::Checkbox("NoBillboard", &m_bNobillboard))
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_NOBILLBOARD);
			}
		}
	}
	else if (PASS_FALLSPREAD != m_EffectDesc.iShaderPass)
	{
		if (ImGui::Checkbox("Spread", &m_bSpread))
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_SPREAD);
			}
		}
	}


	if (PASS_FALLSPREAD == m_EffectDesc.iShaderPass)
	{
		if (ImGui::Checkbox("FallSpread", &m_bFallSpread))
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_FALLSPREAD);
			}
		}
		if (ImGui::Checkbox("BloodSpread", &m_bBloodSpread))
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_BLOOD);
			}
		}
	}

	if (PASS_COLOR == m_EffectDesc.iShaderPass)
	{

		if (ImGui::Checkbox("IntervalSpread", &m_bIntervalSpread))
		{
			if (-1 != m_iCurEditIndex)
			{
				CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
				pParticle->Edit_Action(CEffect::ACTION_INTERVAL);
			}
		}
	}

	ImGui::SameLine();
	if (ImGui::Checkbox("SizeUP", &m_bSizeup))
	{
		if (-1 != m_iCurEditIndex)
		{
			CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
			pParticle->Edit_Action(CEffect::ACTION_SIZEUP);
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("SizeDOWN", &m_bSizedown))
	{
		if (-1 != m_iCurEditIndex)
		{
			CEffect* pParticle = dynamic_cast<CEffect*>(m_EditParticle[m_iCurEditIndex]);
			pParticle->Edit_Action(CEffect::ACTION_SIZEDOWN);
		}
	}




	if (ImGui::Checkbox("Loop", &m_EffectDesc.BufferInstance.isLoop))
	{
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("useRadius", &m_EffectDesc.BufferInstance.bRadius))
	{
		bChange = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("isNormal", &m_EffectDesc.isNormal))
	{
		bChange = true;
	}

	static char strReName[MAX_PATH]{};

	ImGui::InputText("ParticleTag", strReName, MAX_PATH);
	if (ImGui::Button(u8"변경하기"))	
	{
		bChange = true;
		string tag = strReName;
		m_EffectDesc.ParticleTag = m_pGameInstance->StringToWstring(tag);
	}


	if (ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &m_EffectDesc.BufferInstance.iNumInstance))
	{
		bChange = true;
	}
	

	_float* Temp = (_float*)&m_EffectDesc.BufferInstance.vOffsetPos;
	if (ImGui::InputFloat3("OffsetPos", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.vOffsetPos, Temp, sizeof(_float3));
		bChange = true;
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vPivotPos;
	if (ImGui::InputFloat3("PivotPos", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.vPivotPos, Temp, sizeof(_float3));
		bChange = true;
	}

	if(!m_EffectDesc.BufferInstance.bRadius)
	{

		Temp = (_float*)&m_EffectDesc.BufferInstance.vRange;
		if (ImGui::InputFloat3("Range", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.vRange, Temp, sizeof(_float3));
			bChange = true;
		}
	}
	else
	{
		Temp = (_float*)&m_EffectDesc.BufferInstance.fRadius;
		if (ImGui::InputFloat("Radius", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.fRadius, Temp, sizeof(_float));
			bChange = true;
		}
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vSize;
	if (ImGui::InputFloat2("Size", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.vSize, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vRectSize;
	if (ImGui::InputFloat2("RectSize", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_EffectDesc.BufferInstance.vRectSize, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vSpeed;
	if (ImGui::InputFloat2("Speed", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_EffectDesc.BufferInstance.vSpeed, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.vLifeTime;
	if (ImGui::InputFloat2("LifeTime", Temp))
	{
		if (Temp[0] > Temp[1])
			Temp[1] = Temp[0];
		memcpy(&m_EffectDesc.BufferInstance.vLifeTime, Temp, sizeof(_float2));
		bChange = true;
	}

	Temp = (_float*)&m_EffectDesc.fStartTime;
	if (ImGui::InputFloat("StartTime", Temp))
	{
		memcpy(&m_EffectDesc.fStartTime, Temp, sizeof(_float));
		bChange = true;
	}

	Temp = (_float*)&m_EffectDesc.BufferInstance.fDelay;
	if (ImGui::InputFloat("fDelay", Temp))
	{
		memcpy(&m_EffectDesc.BufferInstance.fDelay, Temp, sizeof(_float));
		bChange = true;
	}

	switch (m_EffectDesc.iShaderPass)
	{
	case PASS_NOCOLOR:
	{


		_float* Temp = (_float*)&m_EffectDesc.fLifeAlpha;
		if (ImGui::InputFloat2("LifeAlpha", Temp))
		{
			memcpy(&m_EffectDesc.fLifeAlpha, Temp, sizeof(_float2));
			bChange = true;
		}

	}
		break;
	case PASS_COLOR:
	{
		Color_Palette();
	}
		break;
	case PASS_ROTATE:
	{
		Color_Palette();
		_float* PosTemp = (_float*)&m_TrailDesc.fRotate;
		if (ImGui::InputFloat("Rotate", PosTemp))
		{
			memcpy(&m_TrailDesc.fRotate, PosTemp, sizeof(_float));
			bChange = true;
		}
	}
	break;
	case PASS_ROTANIM:
	{
		Color_Palette();
		_float* PosTemp = (_float*)&m_TrailDesc.fRotate;
		if (ImGui::InputFloat("Rotate", PosTemp))
		{
			memcpy(&m_TrailDesc.fRotate, PosTemp, sizeof(_float));
			bChange = true;
		}
	}
	break;
	case PASS_NOBILLDISTORTION:
	{
		_float* Temp = (_float*)&m_EffectDesc.fDistortion;
		if (ImGui::InputFloat("DistortionWeight", Temp))
		{
			memcpy(&m_EffectDesc.fDistortion, Temp, sizeof(_float));
			bChange = true;
		}


		Temp = (_float*)&m_EffectDesc.fLifeAlpha;
		if (ImGui::InputFloat2("LifeAlpha", Temp))
		{
			memcpy(&m_EffectDesc.fLifeAlpha, Temp, sizeof(_float2));
			bChange = true;
		}
		break;
	}
	case PASS_ANIM:
	{
		Color_Palette();
	}
	break;
	case PASS_NOBILLBOARD:
	{
		Temp = (_float*)&m_EffectDesc.fLifeAlpha;
		if (ImGui::InputFloat2("LifeAlpha", Temp))
		{
			memcpy(&m_EffectDesc.fLifeAlpha, Temp, sizeof(_float2));
			bChange = true;
		}

		Temp = (_float*)&m_EffectDesc.BufferInstance.LowStartRot;
		if (ImGui::InputFloat3("LowStartRot", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.LowStartRot, Temp, sizeof(_float3));
			bChange = true;
		}

		Temp = (_float*)&m_EffectDesc.BufferInstance.HighStartRot;
		if (ImGui::InputFloat3("HighStartRot", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.HighStartRot, Temp, sizeof(_float3));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.LowAngleVelocity;
		if (ImGui::InputFloat3("LowAngleVelocity", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.LowAngleVelocity, Temp, sizeof(_float3));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.HighAngleVelocity;
		if (ImGui::InputFloat3("HighAngleVelocity", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.HighAngleVelocity, Temp, sizeof(_float3));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.GravityScale;
		if (ImGui::InputFloat("GravityScale", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.GravityScale, Temp, sizeof(_float));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.CrossArea;
		if (ImGui::InputFloat("CrossArea", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.CrossArea, Temp, sizeof(_float));
			bChange = true;
		}
		break;
	}

	case PASS_DISTORTION:
	{
		_float* Temp = (_float*)&m_EffectDesc.fDistortion;
		if (ImGui::InputFloat("DistortionWeight", Temp))
		{
			memcpy(&m_EffectDesc.fDistortion, Temp, sizeof(_float));
			bChange = true;
		}


		Temp = (_float*)&m_EffectDesc.fLifeAlpha;
		if (ImGui::InputFloat2("LifeAlpha", Temp))
		{
			memcpy(&m_EffectDesc.fLifeAlpha, Temp, sizeof(_float2));
			bChange = true;
		}
		break;
	}
	case PASS_FALLSPREAD:
	{

		Temp = (_float*)&m_EffectDesc.fLifeAlpha;
		if (ImGui::InputFloat2("LifeAlpha", Temp))
		{
			memcpy(&m_EffectDesc.fLifeAlpha, Temp, sizeof(_float2));
			bChange = true;
		}
		Color_Palette();

		Temp = (_float*)&m_EffectDesc.BufferInstance.LowStartRot;
		if (ImGui::InputFloat3("LowStartRot", Temp))
		{
			if (Temp[0] > m_EffectDesc.BufferInstance.HighStartRot.x)
				m_EffectDesc.BufferInstance.HighStartRot.x = Temp[0];
			if (Temp[1] > m_EffectDesc.BufferInstance.HighStartRot.y)
				m_EffectDesc.BufferInstance.HighStartRot.y = Temp[1];
			if (Temp[2] > m_EffectDesc.BufferInstance.HighStartRot.z)
				m_EffectDesc.BufferInstance.HighStartRot.z = Temp[2];
			memcpy(&m_EffectDesc.BufferInstance.LowStartRot, Temp, sizeof(_float3));
			bChange = true;
		}

		Temp = (_float*)&m_EffectDesc.BufferInstance.HighStartRot;
		if (ImGui::InputFloat3("HighStartRot", Temp))
		{
			if (Temp[0] < m_EffectDesc.BufferInstance.LowStartRot.x)
				m_EffectDesc.BufferInstance.LowStartRot.x = Temp[0];
			if (Temp[1] < m_EffectDesc.BufferInstance.LowStartRot.y)
				m_EffectDesc.BufferInstance.LowStartRot.y = Temp[1];
			if (Temp[2] < m_EffectDesc.BufferInstance.LowStartRot.z)
				m_EffectDesc.BufferInstance.LowStartRot.z = Temp[2];
			memcpy(&m_EffectDesc.BufferInstance.HighStartRot, Temp, sizeof(_float3));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.LowAngleVelocity;
		if (ImGui::InputFloat3("LowAngleVelocity", Temp))
		{
			if (Temp[0] > m_EffectDesc.BufferInstance.HighAngleVelocity.x)
				m_EffectDesc.BufferInstance.HighAngleVelocity.x = Temp[0];
			if (Temp[1] > m_EffectDesc.BufferInstance.HighAngleVelocity.y)
				m_EffectDesc.BufferInstance.HighAngleVelocity.y = Temp[1];
			if (Temp[2] > m_EffectDesc.BufferInstance.HighAngleVelocity.z)
				m_EffectDesc.BufferInstance.HighAngleVelocity.z = Temp[2];
			memcpy(&m_EffectDesc.BufferInstance.LowAngleVelocity, Temp, sizeof(_float3));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.HighAngleVelocity;
		if (ImGui::InputFloat3("HighAngleVelocity", Temp))
		{
			if (Temp[0] < m_EffectDesc.BufferInstance.LowAngleVelocity.x)
				m_EffectDesc.BufferInstance.LowAngleVelocity.x = Temp[0];
			if (Temp[1] < m_EffectDesc.BufferInstance.LowAngleVelocity.y)
				m_EffectDesc.BufferInstance.LowAngleVelocity.y = Temp[1];
			if (Temp[2] < m_EffectDesc.BufferInstance.LowAngleVelocity.z)
				m_EffectDesc.BufferInstance.LowAngleVelocity.z = Temp[2];
			memcpy(&m_EffectDesc.BufferInstance.HighAngleVelocity, Temp, sizeof(_float3));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.GravityScale;
		if (ImGui::InputFloat("GravityScale", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.GravityScale, Temp, sizeof(_float));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.CrossArea;
		if (ImGui::InputFloat("CrossArea", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.CrossArea, Temp, sizeof(_float));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.fUVCount;
		if (ImGui::InputFloat2("UVCount", Temp))
		{
			memcpy(&m_EffectDesc.fUVCount, Temp, sizeof(_float2));
			bChange = true;
		}
		Temp = (_float*)&m_EffectDesc.BufferInstance.fDelay;
		if (ImGui::InputFloat("Delay", Temp))
		{
			memcpy(&m_EffectDesc.BufferInstance.fDelay, Temp, sizeof(_float));
			bChange = true;
		}
		break;
	}

	default:
		break;
	}


	File_Selctor(&bChange);

		if (m_EffectDesc.isNormal)
		{
			_int iNormalSize = NormalTags.size();
			if (ImGui::BeginListBox("NormalBox"))
			{
				for (int i = 0; i < iNormalSize; i++)
				{
					// 리스트박스 아이템의 텍스트
					const bool isSelected = (m_iCurNormalTexture == i);
					char Label[256] = {};
					strcpy_s(Label, m_pGameInstance->WstringToString(NormalTags[i]).c_str());
					if (ImGui::Selectable(Label, isSelected))
					{
						m_iCurNormalTexture = i; // 선택된 아이템 업데이트	
						bChange = true;
					}


					// 선택된 아이템이 보이도록 스크롤
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();

					}
				}
				ImGui::EndListBox();
			}

			if (-1 != m_iCurEditIndex)
			{
				m_EffectDesc.NormalTag = NormalTags[m_iCurNormalTexture];
			}
		}

	



	if (bChange)
	{
		Edit_Particle(m_iCurEditIndex);
	}
	ImGui::End();


}

void CImguiManager::Guizmo_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(700, 550), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(250, 170), ImGuiCond_None);


	ImGui::Begin("Editor");
	
	ImGui::Checkbox("Edit Guizmo", &m_bGuizmo);

	Guizmo(fTimeDelta);

	ImGui::End();
}

bool CImguiManager::IsMouseHoveringCircle(float mouse_x, float mouse_y, float circle_x, float circle_y, float radius)
{
	float dx = mouse_x - circle_x;
	float dy = mouse_y - circle_y;
	return (dx * dx + dy * dy) <= (radius * radius);
}
void CImguiManager::UpdateCirclePosition()
{
	// 마우스 입력 받기
	ImGuiIO& io = ImGui::GetIO();
	float mouse_x = io.MousePos.x;
	float mouse_y = io.MousePos.y;

	//// 마우스 클릭 시 드래그 시작
	//if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	//{
	//	if (IsMouseHoveringCircle(mouse_x, mouse_y, circle_pos_x, circle_pos_y, circle_radius))
	//	{
	//		m_bDragging = true;
	//		drag_start_mouse_x = mouse_x;
	//		drag_start_circle_x = circle_pos_x;
	//	}
	//}

	//// 마우스 버튼을 떼면 드래그 종료
	//if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	//{
	//	m_bDragging = false;
	//}

	//// 드래그 중일 때 원의 x 위치 업데이트
	//if (m_bDragging)
	//{
	//	float delta_x = mouse_x - drag_start_mouse_x;
	//	circle_pos_x = drag_start_circle_x + delta_x;
	//}
}

void CImguiManager::Color_Palette()
{
	_bool bChange = false;
	
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;
	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	if (ImGui::ColorEdit4("MyColor##2f", (float*)&m_EffectDesc.vStartColor, ImGuiColorEditFlags_Float | misc_flags))
	{
		bChange = true; 
	}

	static bool alpha_preview1 = true;
	static bool alpha_half_preview1 = false;
	static bool drag_and_drop1 = true;
	static bool options_menu1 = true;
	static bool hdr1 = false;
	ImGuiColorEditFlags misc_flags1 = (hdr1 ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop1 ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview1 ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview1 ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu1 ? 0 : ImGuiColorEditFlags_NoOptions);

	if (ImGui::ColorEdit4("MyColor##", (float*)&m_EffectDesc.vEndColor, ImGuiColorEditFlags_Float | misc_flags1))
	{
		bChange = true;
	}

	if (bChange)
		Edit_Particle(m_iCurEditIndex);


}

void CImguiManager::Reset_Particle()
{

	for (size_t i = 0; i < m_EditParticle.size(); i++)
	{

		dynamic_cast<CEffect*>(m_EditParticle[i]);

		void* pVoidpDesc = dynamic_cast<CParticle_Point*>(m_EditParticle[i])->Get_Instance();	
		CVIBuffer_Instance::INSTANCE_DESC* pDesc = static_cast<CVIBuffer_Instance::INSTANCE_DESC*>(pVoidpDesc);	

		if(!pDesc->isLoop)	
		{
			Load_Desc(i);
			Edit_Particle(i);
		}
	}

	for (size_t i = 0; i < m_EditAura.size(); i++)
	{


		void* pVoidpDesc = dynamic_cast<CAura*>(m_EditAura[i])->Get_Instance();
		CVIBuffer_Instance::INSTANCE_DESC* pDesc = static_cast<CVIBuffer_Instance::INSTANCE_DESC*>(pVoidpDesc);

		if (!pDesc->isLoop)
		{
			Load_Desc(i);
			Edit_Particle(i);
		}
	}

	for (size_t i = 0; i < m_EditMesh.size(); i++)
	{


		void* pVoidpDesc = dynamic_cast<CParticle_Mesh*>(m_EditMesh[i])->Get_Instance();


		CVIBuffer_Instance::INSTANCE_DESC* pDesc = static_cast<CVIBuffer_Instance::INSTANCE_DESC*>(pVoidpDesc);

		if (!pDesc->isLoop)
		{
			Load_Desc(i);
			Edit_Particle(i);
		}
	}
}

void CImguiManager::Timeline_Tick(_float fTimeDelta)
{
	ImGui::SetNextWindowPos(ImVec2(0, 550), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(700, 170), ImGuiCond_None);
	//TimelineItem items[] = { {0.0f, 1.0f, "Event 1"},{0.5f, 2.5f, "Event 2"},{3.0f, 4.0f, "Event 3"} };	//생성 타임, 소멸 타임(생성타임+라이프타임), 파티클 네이밍

	ImGui::Begin("Timeline");
	ImGui::InputFloat("MaxTime", &m_fMaxTime);
	// 타임라인 배경 그리기
	ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // 시작 위치	
	ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // 크기	
	if (canvas_size.y < 50.0f)
		canvas_size.y = 50.0f; // 최소 높이 설정
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255));

	float item_height = 10.0f; // 각 아이템의 높이
	float item_spacing = 5.0f; // 아이템 간의 간격


	/*
	Info.fStartTime = dynamic_cast<CEffect*>(pGameParticle)->Get_pStartTime();
	Info.fEndTime = &dynamic_cast<CEffect*>(pGameParticle)->Get_Instance()->vLifeTime.y;
	Info.Label = m_pGameInstance->WstringToString(dynamic_cast<CEffect*>(pGameParticle)->Get_Tag()).c_str();

	ParticleTime.push_back(Info);

	*/

	switch (m_iMode)
	{
	case MODE_PARTICLE:
		{
		// 타임라인 아이템 그리기
		for (int i = 0; i < m_EditParticle.size(); ++i) {
			CParticle_Point* item = dynamic_cast<CParticle_Point*>(m_EditParticle[i]);
			CVIBuffer_Instance::INSTANCE_DESC* pDesc = static_cast<CVIBuffer_Instance::INSTANCE_DESC*>(item->Get_Instance());

			string itemLabel = m_pGameInstance->WstringToString(item->Get_Tag());


			// 아이템의 시작 및 끝 위치 계산
			float start_pos_x = canvas_pos.x + (*item->Get_pStartTime() / m_fMaxTime) * canvas_size.x;
			float end_pos_x = canvas_pos.x + ((pDesc->vLifeTime.y + *item->Get_pStartTime()) / m_fMaxTime) * canvas_size.x;
			float item_pos_y = canvas_pos.y + i * (item_height + item_spacing);
			float middle_posx = start_pos_x + (end_pos_x - start_pos_x) * 0.5f;
			// 아이템 그리기
			draw_list->AddRectFilled(ImVec2(start_pos_x, item_pos_y), ImVec2(end_pos_x, item_pos_y + item_height), IM_COL32(255, 100, 100, 255));
			draw_list->AddCircleFilled(ImVec2(start_pos_x, item_pos_y + item_height * 0.5f), 10.0f, IM_COL32(255, 100, 100, 255));
			draw_list->AddCircleFilled(ImVec2(end_pos_x, item_pos_y + item_height * 0.5f), 10.0f, IM_COL32(255, 100, 100, 255));
			draw_list->AddText(ImVec2(middle_posx, item_pos_y), IM_COL32(255, 255, 255, 255), itemLabel.c_str());
		}

		// 캔버스의 크기를 조정하여 모든 아이템이 보이도록 함
		canvas_size.y = m_EditParticle.size() * (item_height + item_spacing);
		ImGui::Dummy(canvas_size); // 빈 공간을 차지하여 레이아웃 유지

			break;
		}
	case MODE_AURA:
	{
		// 타임라인 아이템 그리기
		for (int i = 0; i < m_EditAura.size(); ++i) {
			CAura* item = dynamic_cast<CAura*>(m_EditAura[i]);
			CVIBuffer_Instance::INSTANCE_DESC* pDesc = static_cast<CVIBuffer_Instance::INSTANCE_DESC*>(item->Get_Instance());

			string itemLabel = m_pGameInstance->WstringToString(item->Get_Tag());


			// 아이템의 시작 및 끝 위치 계산
			float start_pos_x = canvas_pos.x + (*item->Get_pStartTime() / m_fMaxTime) * canvas_size.x;
			float end_pos_x = canvas_pos.x + ((pDesc->vLifeTime.y + *item->Get_pStartTime()) / m_fMaxTime) * canvas_size.x;
			float item_pos_y = canvas_pos.y + i * (item_height + item_spacing);
			float middle_posx = start_pos_x + (end_pos_x - start_pos_x) * 0.5f;
			// 아이템 그리기
			draw_list->AddRectFilled(ImVec2(start_pos_x, item_pos_y), ImVec2(end_pos_x, item_pos_y + item_height), IM_COL32(255, 100, 100, 255));
			draw_list->AddCircleFilled(ImVec2(start_pos_x, item_pos_y + item_height * 0.5f), 10.0f, IM_COL32(255, 100, 100, 255));
			draw_list->AddCircleFilled(ImVec2(end_pos_x, item_pos_y + item_height * 0.5f), 10.0f, IM_COL32(255, 100, 100, 255));
			draw_list->AddText(ImVec2(middle_posx, item_pos_y), IM_COL32(255, 255, 255, 255), itemLabel.c_str());
		}

		// 캔버스의 크기를 조정하여 모든 아이템이 보이도록 함
		canvas_size.y = m_EditAura.size() * (item_height + item_spacing);
		ImGui::Dummy(canvas_size); // 빈 공간을 차지하여 레이아웃 유지
		break;

	}
			default:
		
		break;
	}
	

	ImGui::End();

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

	//ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);

	return pInstance;
}

void CImguiManager::Free()
{
	for (auto& iter : m_EditParticle)
		Safe_Release(iter);

	for (auto& iter : m_EditTrail)
		Safe_Release(iter);

	for (auto& iter : m_EditAura)
		Safe_Release(iter);

	for (auto& iter : m_EditMesh)
		Safe_Release(iter);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
