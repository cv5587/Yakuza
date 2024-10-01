#pragma once

#ifdef _DEBUG



#include "Base.h"

#include "Client_Defines.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

/*
1. 디버깅 용 툴.
*/
BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CDebugManager :
    public CBase
{
    DECLARE_SINGLETON(CDebugManager)
private:
    CDebugManager();
    virtual ~CDebugManager() = default;

public:
    _bool isTimeStop() { return m_isTimeStop; }
    _bool isDebug() { return m_isDebug; }
    _bool isLimit() { return m_isLimit; }

public:
    void Set_Debug(_bool isDebug) { m_isDebug = isDebug; }
    
public:
    _float2 Get_Texcoord() { return m_vTexcoord; }

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick();
    HRESULT Render();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    class CSystemManager* m_pSystemManager = { nullptr };
    class CFileTotalMgr* m_pFileTotalMgr = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };

    _bool m_isDebug = { false }; // 디버그 툴 진입했는지 아닌지 체크

private:
    _bool m_isLimit = { true };
    _bool m_isTimeStop = { false }; // TimeDelta Stop
    _float m_fSpeed = { -1.f };
    _bool m_isPlayerMove = { false }; // 플레이어 위치 이동 bool 변수

private:
    _float2 m_vTexcoord = { 1.f, 1.f };

private://렌더러 제어 
    _float m_fHDRLight = { 1.f };
    _int m_iLightPass = { 0 };

private://플레이어 제어
    _uint m_iCutSceneAnimIndex = { 0 };

private:
    void Window_Debug();

public:
    static void Release_Debug();
    virtual void Free() override;
};
END

#endif // _DEBUG