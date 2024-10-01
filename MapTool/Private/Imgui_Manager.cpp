#include "../Public/Imgui_Manager.h"
#include "GameInstance.h"
#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include <Windows.h>
#include <string.h>
#include "ObjPlace_Manager.h"
#include "Terrain_Manager.h"
#include "Camera_Manager.h"
#include "Trigger_Manager.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
    :
    m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pNavigationMgr{ CNavigation_Manager::GetInstance() }
    , m_pObjPlace_Manager { CObjPlace_Manager::GetInstance() }
    , m_pLightTool_Mgr { CLightTool_Mgr::GetInstance() }
    , m_pCameraToolMgr { CCamera_Manager::GetInstance() }
    , m_pColliderMgr{ CCollider_Manager::GetInstance() }
    , m_pTriggerMgr{ CTrigger_Manager::GetInstance() }
    , m_pTriggerObjMgr { CTriggerObj_Manager::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pNavigationMgr);
    Safe_AddRef(m_pObjPlace_Manager);
    Safe_AddRef(m_pLightTool_Mgr);
    Safe_AddRef(m_pCameraToolMgr);
    Safe_AddRef(m_pColliderMgr);
    Safe_AddRef(m_pTriggerMgr);
    Safe_AddRef(m_pTriggerObjMgr);
}

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    // Win32 초기화 호출
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    /* IMGUI 초기화 */
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    /* Navigation 초기화 */
    m_pNavigationMgr->Initialize(m_pDevice, m_pContext);
    m_pObjPlace_Manager->Initialize();
    m_pLightTool_Mgr->Initialize();
    m_pTriggerObjMgr->Initialize();

    return S_OK;
}

void CImgui_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
    CObjPlace_Manager::GetInstance()->Tick(fTimeDelta);

    m_pNavigationMgr->Tick(fTimeDelta);
    m_pCameraToolMgr->Tick(fTimeDelta);
    m_pColliderMgr->Tick(fTimeDelta);
    m_pTriggerObjMgr->Tick(fTimeDelta);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //매 업데이트마다 돌아감
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame(); //new와 같은것 선언해주기

    ImGuiIO& io = ImGui::GetIO();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin(u8"IMGUI 메인화면");

    if (ImGui::Button(u8"땅크기수정"))
        m_bTerrain_IMGUI = true;

    if (ImGui::Button(u8"오브젝트설치"))
        m_bObject_Place_IMGUI = true;

    if (ImGui::Button(u8"네비게이션 TOOL"))
        m_bNaviTool_IMGUI = true;

    if (ImGui::Button(u8"라이트 TOOL"))
    {
        m_bLightMgr_IMGUI = true;
        m_pLightTool_Mgr->Set_LightObj_Render(true);
    }

    if (ImGui::Button(u8"카메라 TOOL"))
        m_bCameraMgr_IMGUI = true;

    if (ImGui::Button(u8"콜라이더 TOOL"))
        m_bColliderMgr_imgui = true;

    if (ImGui::Button(u8"트리거 TOOL"))
        m_bTriggerMgr_IMGUI = true;

    if (ImGui::Button(u8"트리거 오브젝트배치 TOOL"))
        m_bTriggerObjMgr_IMGUI = true;
       

    ImGui::End();

#pragma region IMGUI UI
    if (m_bTerrain_IMGUI)
    {
        Set_Terrain_IMGUI();
    }

    if (m_bObject_Place_IMGUI)
    {
        Show_MapObj_Place_IMGUI();
        Show_Object_List();
        m_pObjPlace_Manager->Add_Decal_IMGUI();
        m_pObjPlace_Manager->Add_ObjectCollider_IMGUI();
        m_eWrieID = OBJPLACE;
    }

    if (m_bNaviTool_IMGUI)
    {
        Set_NaviTool_IMGUI();
        m_eWrieID = NAVIOBJ;
    }

    if (m_bLightMgr_IMGUI)
    {
        Show_LightTool_IMGUI();
        m_eWrieID = LIGHT;
    }

    if (m_bCameraMgr_IMGUI)
    {
        Show_CameraTool_IMGUI();
    }


    if (m_bColliderMgr_imgui)
    {
        Show_Collider_IMGUI();
        m_pColliderMgr->Add_Collider_IMGUI();
        m_eWrieID = COLLIDER;
    }

    if (m_bTriggerMgr_IMGUI)
    {
        m_pTriggerMgr->Show_FileName();
        Show_Trigger_IMGUI();
        m_eWrieID = TRIGGER;
    }

    if (m_bTriggerObjMgr_IMGUI)
    {
        Show_TriggerMapObj_Place_IMGUI();
        m_pTriggerObjMgr->Show_Installed_GameObjectsList();
        m_pTriggerObjMgr->Add_ObjectCollider_IMGUI();
        m_eWrieID = TRIGGEROBJ;
    }

#pragma endregion
    ImGui::EndFrame();


    m_pLightTool_Mgr->Tick(fTimeDelta);
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
    /* camera object랑 같이 기록되면 안돼서 카메라 할때는 꺼두기 */
    //if (false == m_bCameraMgr_IMGUI)
    //{

    //}
    m_pObjPlace_Manager->Late_Tick(fTimeDelta);
    m_pColliderMgr->Late_Tick(fTimeDelta);

    m_pLightTool_Mgr->Late_Tick(fTimeDelta);

    m_pCameraToolMgr->Late_Tick(fTimeDelta);

    m_pTriggerMgr->Late_Tick(fTimeDelta);

    m_pTriggerObjMgr->Late_Tick(fTimeDelta);

    m_pNavigationMgr->Late_Tick(fTimeDelta);
}

void CImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_pNavigationMgr->Render();
}


void CImgui_Manager::Set_Terrain_IMGUI()
{
    ImGui::Begin(u8"Terrain");
    ImGui::Text(u8"Terrain 크기 수정");

    static int inumx, inumz;
    ImGui::InputInt(u8"크기x", &inumx);
    ImGui::InputInt(u8"크기z", &inumz);

    if (ImGui::Button(u8"Terrain 수정"))
    {
        CTerrain_Manager::GetInstance()->Change_LandScale(inumx, inumz);
    }

    if (ImGui::Button("Close"))
        m_bTerrain_IMGUI = false;
    ImGui::End();
}



void CImgui_Manager::Show_Object_List()
{
    m_pObjPlace_Manager->Show_Installed_GameObjectsList();
}

void CImgui_Manager::Set_NaviTool_IMGUI()
{
    ImGui::Begin(u8"Navigation 수정");

    m_pNavigationMgr->Load_Cell_IMGUI();

    m_pNavigationMgr->Show_Cells_IMGUI();

    m_pNavigationMgr->Make_Route();

    static bool    bCanAdd;

    
    if (m_pGameInstance->GetKeyState(DIK_X) == AWAY)
    {
        bCanAdd = true;
    }


    if (CGameInstance::GetInstance()->GetMouseState(DIM_LB) == AWAY && true == bCanAdd)
    {
        _bool		isPick;
        _vector		vTargetPos = CGameInstance::GetInstance()->Picking(&isPick);

        m_pNavigationMgr->Make_Point(vTargetPos);
        bCanAdd = false;
    }

    if (ImGui::Button("Close"))
        m_bNaviTool_IMGUI = false;
    ImGui::End();
}

void CImgui_Manager::Show_MapObj_Place_IMGUI()
{
    ImGui::Begin(u8"레이어 선택");

    m_pObjPlace_Manager->Set_Map_Object();

    if (ImGui::Button("Close"))
    {
        m_bObject_Place_IMGUI = false;
    }

    ImGui::End();
}

void CImgui_Manager::Show_LightTool_IMGUI()
{
    /* 파일 리스트 보여주기 */
    m_pLightTool_Mgr->Show_FileName();

    ImGui::Begin(u8"라이트 설치");

    m_pLightTool_Mgr->Show_Add_Light_IMGUI();

    if (ImGui::Button("Close"))
    {
        m_bLightMgr_IMGUI = false;
        m_pLightTool_Mgr->Set_LightObj_Render(false);
    }

    ImGui::End();
}

void CImgui_Manager::Show_CameraTool_IMGUI()
{
    /* 파일 리스트 보여주기 */
    m_pCameraToolMgr->Show_FileName();

    ImGui::Begin(u8"카메라 설치");
    
    m_pCameraToolMgr->Install_Camera_IMGUI();

    if (ImGui::Button("Close"))
    {
        m_bCameraMgr_IMGUI = false;
    }

    ImGui::End();


}

void CImgui_Manager::Show_Collider_IMGUI()
{
    ImGui::Begin(u8"레이어 선택");

    m_pColliderMgr->Show_FileName();


    if (ImGui::Button(u8"닫기"))
    {
        m_bColliderMgr_imgui = false;
    }

    ImGui::End();
}

void CImgui_Manager::Show_Trigger_IMGUI()
{
    ImGui::Begin(u8"트리거 파일 선택");

    m_pTriggerMgr->Show_Add_Trigger_IMGUI();

    if (ImGui::Button("Close"))
        m_bTriggerMgr_IMGUI = false;

    ImGui::End();
}

void CImgui_Manager::Show_TriggerMapObj_Place_IMGUI()
{
    ImGui::Begin(u8"레이어 선택");

    m_pTriggerObjMgr->Set_Map_Object();

    if (ImGui::Button("Close"))
    {
        m_bTriggerObjMgr_IMGUI = false;
    }

    ImGui::End();
}


void CImgui_Manager::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();


    Safe_Release(m_pObjPlace_Manager);
    CObjPlace_Manager::DestroyInstance();

    Safe_Release(m_pNavigationMgr);
    CNavigation_Manager::DestroyInstance();

    Safe_Release(m_pLightTool_Mgr);
    CLightTool_Mgr::DestroyInstance();

    Safe_Release(m_pCameraToolMgr);
    CCamera_Manager::DestroyInstance();

    Safe_Release(m_pColliderMgr);
    CCollider_Manager::DestroyInstance();

    Safe_Release(m_pTriggerMgr);
    CTrigger_Manager::DestroyInstance();

    Safe_Release(m_pTriggerObjMgr);
    CTriggerObj_Manager::DestroyInstance();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pGameInstance);

}