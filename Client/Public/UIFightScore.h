#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIFightScore :
    public CUIScene
{
public:
    enum SCORE{BONUS0 ,BONUS1 , BONUS2 , BACK , SCORE_END};
protected:
    CUIFightScore();
    CUIFightScore(const CUIFightScore& rhs);
    virtual ~CUIFightScore() = default;

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
    void Set_Money();
    //돈추가시 초기화
    void AddMoney(_uint AddMoney);
    void StartAddMoney(_uint iAddMoney);
    void Money_Anim();
private:
    vector<CUI_Object*>m_Bonus;
    class CGroup* m_pMoney = { nullptr };
    class CGroup* m_pAddMoney = { nullptr };

    _float m_iCurrentTime = { 0.f };
    _bool m_isEnd = { true };

    _uint m_CurrentKiryuMoney = { 0 };
    _uint m_TargetKiryuMoney = { 0 };
    _uint m_iAddMoney = { 0 };
    _uint m_iAnimMoney = { 0 };

    _float U = 0.0625f;
    _float V = 0.2f;
    _float m_fGetTime = { 0.f };
    _float m_fEndTime = { 1.f };

    _float2 m_fSignStartUV = {0.45f,0.2f};
    _float2 m_fSignEndUV = {0.52f,0.4f};
    _float2 m_fAddStartUV = { 0.53f,0.2f };
    _float2 m_fAddEndUV = { 0.57f,0.4f };
public:
    static CUIFightScore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END