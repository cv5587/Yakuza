#pragma once

#include "UIScene.h"
BEGIN(Client)
class CUIKaraoke_Score:
	public CUIScene
{
public:
    enum SCORE { ONE , TEN  , HUNDRED, IMAGE, SCORE_END};
    enum IMAGE { BAD, GOOD , GREAT ,IMAGE_END};
protected:
    CUIKaraoke_Score();
    CUIKaraoke_Score(const CUIKaraoke_Score& rhs);
    virtual ~CUIKaraoke_Score() = default;

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

public:
    void Set_Score(_uint iScore) { m_iScore = iScore; }
private:
    void ScoreUpdate();
private:
    _uint m_iScore = { 0 };
    _bool m_iOnce = { false };

public:
    static CUIKaraoke_Score* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();

};
END
