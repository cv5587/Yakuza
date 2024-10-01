#include "../Default/framework.h"

#include "MainApp.h"
#include "GameInstance.h"
#include "Object_Manager.h"
#include "Level_Loading.h"
#include "Background.h"

#include "Imgui_Manager.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#pragma endregion	

CMainApp::CMainApp() :
	m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pIMGUI_Manager {CImgui_Manager::GetInstance()},
	m_pObjectManager { CObject_Manager::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pIMGUI_Manager);
	Safe_AddRef(m_pObjectManager);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.hInst = g_hInst;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Font()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_RUNMAP)))
		return E_FAIL;

	if (FAILED(m_pIMGUI_Manager->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG

	m_pGameInstance->Tick(fTimeDelta);

	m_pIMGUI_Manager->Tick(fTimeDelta);
	m_pObjectManager->Tick(fTimeDelta);

	m_pIMGUI_Manager->Late_Tick(fTimeDelta);
	m_pObjectManager->Late_Tick(fTimeDelta);
}

HRESULT CMainApp::Render()
{
#ifdef _DEBUG

	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumRender);

		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}
#endif // _DEBUG


	/* 그린다. */
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 0.f));
	m_pGameInstance->Clear_DepthStencil_View();
	
	
	//m_pGameInstance->Draw();
	m_pIMGUI_Manager->Render();
	m_pObjectManager->Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#ifdef _DEBUG
	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));
#endif //DEBUG
	m_pGameInstance->Present();

	//m_pGameInstance->Draw();
	//CImgui_Manager::GetInstance()->Render();
	//m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_nanum24"), TEXT("../../Client/Bin/Resources/Fonts/nanum24.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../../Client/Bin/Resources/Fonts/nanum28.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_nanum30"), TEXT("../../Client/Bin/Resources/Fonts/nanum30.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_nanum36"), TEXT("../../Client/Bin/Resources/Fonts/nanum36.spritefont"))))
		return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_GameObject_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxUI*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxUI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

    return S_OK;
}

CMainApp* CMainApp::Create()
{
    CMainApp* pInstance = new CMainApp();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CMainApp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	/* 레퍼런스 카운트를 0으로만든다. */
	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();

	/* 여기서 instance release 해줌 */
	Safe_Release(m_pIMGUI_Manager);
	CImgui_Manager::GetInstance()->Release_Imgui();

	Safe_Release(m_pObjectManager);
	CObject_Manager::GetInstance()->Release_ObjectManager();
}
