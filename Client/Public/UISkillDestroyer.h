#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUISkillDestroyer :
    public CUIScene
{
protected:
    CUISkillDestroyer();
    CUISkillDestroyer(const CUISkillDestroyer& rhs);
    virtual ~CUISkillDestroyer() = default;
public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag = TEXT("")) override;

    virtual HRESULT Show_Scene()override;//ui 애님 준비(초기화/열때 정방향 진행)
    virtual HRESULT Close_Scene()override;//ui 애님 준비(초기화/닫을떄 반대로진행)

    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual _bool Click_InterSect()override;//클릭시충돌
    virtual _bool Over_InterSect()override;//오버시 충동
    virtual void Action() override;
    virtual void OverAction() override;

    _bool* Get_Skill() {
        return m_isSkill;
    }

private:
    void Set_Money(const _float& fTimeDelta);
private:
    vector < CUI_Object*> m_pBall;
    vector <CUI_Object*> m_pGetter;
    _bool m_isSkill[6] = {  false, };
    _uint m_iPrice[6] = {400000,1000000000,400000000,400000000 ,400000000 ,400000000 };
    _bool m_isGet = { false };
    
    _float m_fGetTime = { 0.f };
    _float m_fEndTime = { 2.f };
    _bool m_isFinMoney = { false };
    _uint m_iCurMoney = { 0 };

    _float2 m_ImageNumber[10] = { _float2(0.075,0.124),_float2(0.13,0.18),_float2(0.184,0.233),_float2(0.239,0.288),_float2(0.296,0.342)
                                            ,_float2(0.35,0.396),_float2(0.403,0.451),_float2(0.458,0.507),_float2(0.512,0.56),_float2(0.568,0.614) };

    _float m_fCurFinMoney = { 0.f };
    _bool m_fCurFinAnim = { false };

    _bool m_isMoneyEndStart = { false };
    _bool m_isFinishMoney = { false };

public:
    static CUISkillDestroyer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
