// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "framework.h"
#include "Client.h"
#include "MainApp.h"
#include "GameInstance.h"

#ifdef _DEBUG
#include "DebugManager.h"
#endif // _DEBUG


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
HWND	g_hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;

    

    CMainApp* pMainApp = CMainApp::Create();
    if (nullptr == pMainApp)
        return FALSE;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (nullptr == pGameInstance)
        return FALSE;
    Safe_AddRef(pGameInstance);

    if (FAILED(pGameInstance->Ready_Timer(TEXT("Timer_Default"))))
        return FALSE;
    if (FAILED(pGameInstance->Ready_Timer(TEXT("Timer_60"))))                  // 플레이어를 제외하고 대부분 사용중
        return FALSE;
    if (FAILED(pGameInstance->Ready_Timer(TEXT("Timer_Player"))))               // 플레이어만 사용중
        return FALSE;
    if (FAILED(pGameInstance->Ready_Timer(TEXT("Timer_Game"))))               // 게임캐릭터들과 별개로 실행되는 게임 자체의 타이머
        return FALSE;

#ifdef _DEBUG
    
    CDebugManager* pDebugManager = CDebugManager::GetInstance();
    if (nullptr == pDebugManager)
        return FALSE;
    Safe_AddRef(pDebugManager);
#endif // _DEBUG

    _float		fTimeAcc = { 0.0f };

    // 기본 메시지 루프입니다:
    while (TRUE)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        //_CrtSetBreakAlloc(4996512); // 추가

        pGameInstance->Update_TimeDelta(TEXT("Timer_Default"));

        fTimeAcc += pGameInstance->Get_TimeDelta(TEXT("Timer_Default"));
#ifdef _DEBUG
        if (fTimeAcc > 1.f / 60.0f)
        {
            pGameInstance->Update_TimeDelta(TEXT("Timer_60"));
            pGameInstance->Update_TimeDelta(TEXT("Timer_Player"));
            pGameInstance->Update_TimeDelta(TEXT("Timer_Game"));
            _float fTimeDelta = pGameInstance->Get_TimeDelta(TEXT("Timer_60"));

            if (pDebugManager->isDebug() && pDebugManager->isTimeStop())
            {
                fTimeDelta = 0.f;
                // 디버깅용으로 타임델타 멈출 때 플레이어가 사용하는 타임델타도 멈춤
                //pGameInstance->Set_TimeSpeed(TEXT("Timer_Player"), 0.f);
            }
#else
        if (fTimeAcc > 1.f / 60.0f)
        {
            pGameInstance->Update_TimeDelta(TEXT("Timer_60"));
            pGameInstance->Update_TimeDelta(TEXT("Timer_Player"));
            pGameInstance->Update_TimeDelta(TEXT("Timer_Game"));
            _float fTimeDelta = pGameInstance->Get_TimeDelta(TEXT("Timer_60"));
#endif // _DEBUG
            pMainApp->Tick(fTimeDelta);
            pMainApp->Render();



            fTimeAcc = 0.f;
        }
    }


#ifdef _DEBUG
    Safe_Release(pDebugManager);
#endif // _DEBUG
    Safe_Release(pGameInstance);
    Safe_Release(pMainApp);

    return (int) msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT		rcWindow = { 0, 0, g_iWinSizeX, g_iWinSizeY };

   AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, FALSE);

   _float ScreentX = (rcWindow.right - rcWindow.left);
   _float ScreentY = (rcWindow.bottom - rcWindow.top);

   _float fPosX = (GetSystemMetrics(SM_CXSCREEN) - ScreentX) * 0.5f;
   _float fPosY = (GetSystemMetrics(SM_CYSCREEN) - ScreentY) * 0.5f;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       fPosX, fPosY, ScreentX, ScreentY, nullptr, nullptr, hInstance, nullptr);


   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

#ifdef _DEBUG
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // _DEBUG

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG

    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;
#endif // _DEBUG

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        default:
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
