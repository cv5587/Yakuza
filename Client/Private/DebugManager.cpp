#ifdef _DEBUG

#include "DebugManager.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"
#include "SystemManager.h"
#include "UIManager.h"

#include "DebugCamera.h"
#include "Player.h"
#include "Level_Loading.h"

IMPLEMENT_SINGLETON(CDebugManager)

CDebugManager::CDebugManager()
    : m_pGameInstance { CGameInstance::GetInstance() },
    m_pFileTotalMgr{ CFileTotalMgr::GetInstance() },
    m_pSystemManager{ CSystemManager::GetInstance() },
    m_pUIManager { CUIManager::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pFileTotalMgr);
    Safe_AddRef(m_pSystemManager);
    Safe_AddRef(m_pUIManager);
}

HRESULT CDebugManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);


    //초기화
    m_fHDRLight = m_pGameInstance->Get_HDRLight();

    return S_OK;
}

void CDebugManager::Tick()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    /*_bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);*/

    Window_Debug();
}

HRESULT CDebugManager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CDebugManager::Window_Debug()
{
    ImGui::Begin("Debug");

    if (ImGui::CollapsingHeader("Player"))
    {
        //Time 제어
        ImGui::SeparatorText("Time");

        ImGui::Checkbox("Limit", &m_isLimit);
        ImGui::Checkbox("TimeDelta Stop", &m_isTimeStop);

        if (ImGui::InputFloat("TimeSpeed", &m_fSpeed))
            m_pGameInstance->Set_TimeSpeed(TEXT("Timer_60"), m_fSpeed);

        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

        _bool isAnimStart = pPlayer->isAnimStart();
        if (ImGui::Checkbox("Anim Start", &isAnimStart))
            pPlayer->Set_AnimStart(isAnimStart);

        ImGui::NewLine();

        ImGui::Checkbox("Player Pos Move (Picking)", &m_isPlayerMove);
        ImGui::Text("! Mouse RB Click !");
        if (m_isPlayerMove)
        {
            if (m_pGameInstance->GetMouseState(DIM_RB) == TAP)
            {
                _bool isPicking = { false };
                _vector vMovePos = m_pGameInstance->Picking(&isPicking);

                if (isPicking)
                {
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
                    pPlayer->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vMovePos);
                }
            }
        }

        ImGui::Text("Player Hit Action");

        auto& Anims = pPlayer->Get_CutSceneAnims();

        for (size_t i = 0; i < CPlayer::CUTSCENE_ANIMATION_END; i++)
        {
            if (ImGui::RadioButton(Anims.at((CPlayer::CUTSCENE_ANIMATION_TYPE)i).c_str(), m_iCutSceneAnimIndex == i))
                m_iCutSceneAnimIndex = i;
        }

        pPlayer->Set_CutSceneIndex((CPlayer::CUTSCENE_ANIMATION_TYPE)m_iCutSceneAnimIndex);
        
        //Camera 제어
        ImGui::SeparatorText("Camera");

        CDebugCamera* pCamera = dynamic_cast<CDebugCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), 0));
        _float fSensor = pCamera->Get_Sensor();
        if (ImGui::InputFloat("CameraSensor", &fSensor))
            pCamera->Set_Sensor(fSensor);

        CTransform* pCameraTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Com_Transform")));

        _float fSpeed = pCameraTransform->Get_Speed();
        if (ImGui::InputFloat("CameraSpeed", &fSpeed))
            pCameraTransform->Set_Speed(fSpeed);

        //Camera 제어
        ImGui::SeparatorText("Render");

        _bool isRender = pPlayer->Get_ObjectRender();
        if (ImGui::Checkbox("PlayerRender", &isRender))
            pPlayer->Set_ObjectRender(isRender);

       //림 제어
        _float2 UV = pPlayer->Get_TopUV();
        if (ImGui::SliderFloat2("TOP", (_float*)&UV, 0.f, 1.f))
            pPlayer->Set_TopUV(UV);

         UV = pPlayer->Get_BotUV();
        if (ImGui::SliderFloat2("BOT", (_float*)&UV, 0.f, 1.f))
            pPlayer->Set_BotUV(UV);

         UV = pPlayer->Get_PartUV();
        if (ImGui::SliderFloat2("PART", (_float*)&UV, 0.f, 1.f))
            pPlayer->Set_PartUV(UV);

    }

    if (ImGui::CollapsingHeader("Deferred Shader"))
    {
        ImGui::SeparatorText("Shader ON/OFF");
        _bool isHDR = m_pGameInstance->isHDR();
        if (ImGui::Checkbox("HDR", &isHDR))
            m_pGameInstance->Set_HDR(isHDR);

        _bool isSSAO = m_pGameInstance->isSSAO();
        if (ImGui::Checkbox("SSAO", &isSSAO))
            m_pGameInstance->Set_SSAO(isSSAO);

        _bool isPBR = m_pGameInstance->isPBR();
        if (ImGui::Checkbox("PBR", &isPBR))
            m_pGameInstance->Set_PBR((isPBR));

        _bool isBOF = m_pGameInstance->isBOF();
        if (ImGui::Checkbox("BOF", &isBOF))
            m_pGameInstance->Set_BOF(isBOF);

        _bool isRadialBlur = m_pGameInstance->isRadialBlur();
        if (ImGui::Checkbox("RadialBlur", &isRadialBlur))
            m_pGameInstance->Set_RadialBlur(isRadialBlur);

        _bool isInvertColor = m_pGameInstance->isInvertColor();
        if (ImGui::Checkbox("InvertColor", &isInvertColor))
            m_pGameInstance->Set_InvertColor(isInvertColor);

        _bool isInvertSaturationColor = m_pGameInstance->isInvertSaturationColor();
        if (ImGui::Checkbox("InvertSaturationColor", &isInvertSaturationColor))
            m_pGameInstance->Set_InvertSaturationColor(isInvertSaturationColor);

        _bool isVignette = m_pGameInstance->isVignette();
        if (ImGui::Checkbox("Vignette", &isVignette))
            m_pGameInstance->Set_Vignette(isVignette);

        _bool isShadow = m_pGameInstance->isShadow();
        if (ImGui::Checkbox("Shadow", &isShadow))
            m_pGameInstance->Set_Shadow(isShadow);
        
        ImGui::SeparatorText("HDR");
        if (ImGui::InputFloat("HDR Light", &m_fHDRLight, 0.f))
            m_pGameInstance->Set_HDRLight(m_fHDRLight);

        ImGui::SeparatorText("SSAO");
        _float fRadiuse = m_pGameInstance->Get_SSAORadiuse();

        if (ImGui::SliderFloat("Radiuse", &fRadiuse, 0.001f, 1.f))
            m_pGameInstance->Set_SSAORadiuse(fRadiuse);

        _float fBlur = m_pGameInstance->Get_SSAOBlur();
        if (ImGui::SliderFloat("Blur", &fBlur, 0.1f, 10.f))
            m_pGameInstance->Set_SSAOBlur(fBlur);

        _float fBias = m_pGameInstance->Get_SSAOBias();
        if (ImGui::SliderFloat("Bias", &fBias, 0.0001f, 1.f))
            m_pGameInstance->Set_SSAOBias(fBias);

        ImGui::SeparatorText("InvertColor");
        _float fDuration = m_pGameInstance->Get_InvertColorDuration();
        if (ImGui::SliderFloat("InvertDuration", &fDuration, 0.01f, 1.f))
            m_pGameInstance->Set_InvertColorDuration(fDuration);

        ImGui::SeparatorText("InvertSaturationColor");
        _float fFactor = m_pGameInstance->Get_InvertSaturationFactor();
        if (ImGui::SliderFloat("InvertSaturationFactor", &fFactor, 0.01f, 1.f))
            m_pGameInstance->Set_InvertSaturationFactor(fFactor);

        ImGui::SeparatorText("RadialBlur");
        _float fPower = m_pGameInstance->Get_RadialPower();
        if (ImGui::SliderFloat("Power", &fPower, 0.01f, 1.f))
            m_pGameInstance->Set_RadialPower(fPower);

        _int iSample = m_pGameInstance->Get_RadialSample();
        if (ImGui::SliderInt("Sample", &iSample, 1, 30))
            m_pGameInstance->Set_RadialSample(iSample);

        ImGui::SeparatorText("Vignette");
        _float fIntensity = m_pGameInstance->Get_VignetteIntensity();
        if (ImGui::SliderFloat("Intensity", &fIntensity, 0.01f, 1.f))
            m_pGameInstance->Set_VignetteIntensity(fIntensity);

        _float fSmootness = m_pGameInstance->Get_VignetteSmootness();
        if (ImGui::SliderFloat("Smootness", &fSmootness, 0.01f, 1.f))
            m_pGameInstance->Set_VignetteSmootness(fSmootness);

        ImGui::SeparatorText("AdjustColor");
        _float fBrightness = m_pGameInstance->Get_AdjectBrightness();
        if (ImGui::SliderFloat("Brightness", &fBrightness, 0.f, 10.f))
            m_pGameInstance->Set_AdjectBrightness(fBrightness);

        _float fContrast = m_pGameInstance->Get_AdjectContrast();
        if (ImGui::SliderFloat("contrast", &fContrast, 0.f, 10.f))
            m_pGameInstance->Set_AdjectContrast(fContrast);

        _float fSaturation = m_pGameInstance->Get_AdjectSaturation();
        if (ImGui::SliderFloat("Saturation", &fSaturation, 0.f, 10.f))
            m_pGameInstance->Set_AdjectSaturation(fSaturation);

        _float4 vTine = m_pGameInstance->Get_AdjectTint();
        if (ImGui::ColorEdit4("vTine", (float*)&vTine, ImGuiColorEditFlags_Float))
            m_pGameInstance->Set_AdjectTint(vTine);

        //그림자 제어
        ImGui::NewLine();
        ImGui::SeparatorText("Shadow");
        _float4 vShadowPos = m_pGameInstance->Get_ShadowLightDir();

        if (ImGui::InputFloat4("ShadowViewPos", (_float*)&vShadowPos))
        {
            m_pGameInstance->Set_ShadowLightDir(vShadowPos);
        }
    }

    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::SeparatorText("Light");

        if (ImGui::InputInt("Light Number", &m_iLightPass, 0))
            m_pFileTotalMgr->Set_Lights_In_Client(m_iLightPass);
    }

    if (ImGui::CollapsingHeader("Level"))
    {
        ImGui::SeparatorText("Level Change");

        if (ImGui::Button("Level_Office1"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_OFFICE_1F));
        if (ImGui::Button("Level_Office2"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_OFFICE_2F));
        if (ImGui::Button("Level_Office_Boss"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_OFFICE_BOSS));
        if (ImGui::Button("Level_Dogimazo"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO));
        if (ImGui::Button("Level_Dogimazo_Staire"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_STAIRS));
        if (ImGui::Button("Level_Dogimazo_Lobby"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_LOBBY));
        if (ImGui::Button("Level_Dogimazo_Boss"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DOGIMAZO_BOSS));
        if (ImGui::Button("Level_Street"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STREET));
        if (ImGui::Button("Level_CarChase"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CARCHASE));
        if (ImGui::Button("Level_Karaoke"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KARAOKE));
        if (ImGui::Button("Level_Karaoke_start"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KARAOKE_START));
        if (ImGui::Button("Level_Karaoke_end"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KARAOKE_END));
        if (ImGui::Button("Level_Nishiki"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_NISHIKIWALK));
        if (ImGui::Button("Level_TOKOStreet"))
            m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOKOSTREET));
    }

    if (ImGui::CollapsingHeader("Effect  / UI"))
    {
        ImGui::SeparatorText("RimLight");
        
        _bool isRimLight = m_pGameInstance->isRimLight();

        if (ImGui::Checkbox("RimLight ON/OFF", &isRimLight))
            m_pGameInstance->Set_RimLight(isRimLight);

        ImGui::SeparatorText("Particle");
        if (ImGui::Button("pang"))
        {
            if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), TEXT("Layer_Particle"), nullptr)))
                MSG_BOX("pang!");
        }

        ImGui::SeparatorText("UI");
        _bool isRender = m_pUIManager->Get_Render();
        if (ImGui::Checkbox("Render ON/OFF", &isRender))
            m_pUIManager->Set_Render(isRender);
    }
    ImGui::End();
}

void CDebugManager::Release_Debug()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CDebugManager::DestroyInstance();
}

void CDebugManager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pFileTotalMgr);
    Safe_Release(m_pSystemManager);

    Safe_Release(m_pUIManager);
}

#endif // _DEBUG