#pragma once
#include "Base.h"

#include "Client_Defines.h"
BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUIScene :
    public CBase
{
public:
    typedef struct tSceneDesc {
        _bool isLoading;
        wstring strSceneName;
    }SCENE_DESC;

protected:
    CUIScene();
    CUIScene(const CUIScene& rhs);
    virtual ~CUIScene() = default;

public:
    //loader 에서 만든 ui객체를 받아와서 저장해둠. 무조건 파일 순서대로 받아와서 정렬함(순서중요)
    //씬마다 사용법 다르면 오버라이딩 ㄱ
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag = TEXT(""));
    
    virtual HRESULT Show_Scene();//ui 애님 준비(초기화/열때 정방향 진행)
    virtual HRESULT Close_Scene();//ui 애님 준비(초기화/닫을떄 반대로진행)

    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,void* pArg);
    virtual HRESULT Tick(const _float& fTimeDelta);
    virtual HRESULT Late_Tick(const _float& fTimeDelta);

    virtual void Check_AimFin();//모든 상호작용은 애니메이션이 다 끝난뒤 진행함(m_UI 에 들어간 상시 떠있는 ui)

    virtual _bool Click_InterSect();//클릭시충돌
    virtual _bool Over_InterSect();//오버시 충동
    virtual void Action();//클릭시 행동
    virtual void OverAction();//오버시 행동

public:
    _bool Get_isAnimFinish() { return m_isAnimFin; }
    _bool Get_isClose() { return m_isClose; }
    _bool Get_isOpen() { return m_isOpen; }
    _bool Get_isLoading() { return m_isLoading; }
    virtual void Reset() {
        m_isClose = false;  
    }
    wstring Get_SceneName() { return m_strSceneName; }

public:
    void Set_Open(_bool isOpen = true) { m_isOpen = isOpen; }

protected:
    class CGameInstance* m_pGameInstance = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    vector<class CUI_Object*> m_UI;//이벤트 제외한 ui를 받는곳(상시 떠있을 ui)
    vector<class CUI_Object*> m_EventUI;//이벤트 ui를 받는곳(특정 상황에 쓸 ui)ex.클릭,오버
    vector<class CBtn*> m_Button;//버튼만 복사해서 모아 둠(반응할 버튼을 모아서 순서대로 관리)

    _int m_iCurButton = { -1 };
    _int m_iPrevButton = { -1 };


    _bool m_isAnimFin = { false };
    _bool m_isClose = { true };
    wstring m_strSceneName = { TEXT("") };

    _bool m_isLoading = { false };
    _bool m_isOpen = { false };

public:
    virtual void Free();
};

END