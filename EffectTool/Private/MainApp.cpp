#include "../Default/framework.h"

#include "MainApp.h"
#include "GameInstance.h"

#include "ImguiManager.h"

#include "Level_Loading.h"
#include "Background.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#pragma endregion	

CMainApp::CMainApp() :
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
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

	if (FAILED(Open_Level(LEVEL_TEST)))
		return E_FAIL;

	if (FAILED(Ready_Manager()))
		return E_FAIL;
	
	//m_pGameInstance->Set_DebugView(true);
	
	return S_OK;
}

void CMainApp::Tick(const _float& fTimeDelta)
{
#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG



	m_pGameInstance->Tick(fTimeDelta);
	m_pGuimanager->Tick(fTimeDelta);
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
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));

	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

	m_pGuimanager->Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#ifdef _DEBUG
	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));
#endif // _DEBUG

	m_pGameInstance->Present();

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
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/nanum28.spritefont"))))
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

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Default0.dds")))))
		return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Manager()
{
	m_pGuimanager = CImguiManager::Create(m_pDevice, m_pContext);
	if (nullptr == m_pGuimanager)
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

	Safe_Release(m_pGuimanager);
}
