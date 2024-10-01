#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIKaraoke_Select :
    public CUIScene
{
public:
    enum SELECT { SELECT_MUSIC , SELECT_SING , SELECT_END};
    enum DISC{DISC0 , TAG0 , TAG1, TAG2 , DISC_END};
protected:
    CUIKaraoke_Select();
    CUIKaraoke_Select(const CUIKaraoke_Select& rhs);
    virtual ~CUIKaraoke_Select() = default;

public:
    virtual HRESULT Show_Scene()override;//ui 애님 준비(초기화/열때 정방향 진행)
    virtual HRESULT Close_Scene()override;//ui 애님 준비(초기화/닫을떄 반대로진행)

public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag = TEXT("")) override;
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual _bool Click_InterSect()override;//클릭시충돌
    virtual _bool Over_InterSect()override;//오버시 충동
    virtual void Action() override;
    virtual void OverAction() override;

    class CGroup* m_Disc;
    class CGroup* m_Select;
    vector<CUI_Object*> m_UIGrade;
    vector<CUI_Object*> m_UIName;
    class CUI_Object* m_MusicTitle;


    _float m_iCurrentTime = { 0.f };
    _bool m_isStart = false;
   
public:
    static CUIKaraoke_Select* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();

};
END
