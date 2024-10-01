#pragma once
#include "UIScene.h"
#include "Client_Defines.h"
BEGIN(Client)
class CUITutorial :
    public CUIScene
{
public:
    enum UI_TUTORIAL { 
        TOTU_CLEAR, 
        TOTU_CLEAR_CEHCKBOX, 
        TOTU_FINISHBLOW,
        TOTU_FINISHBLOW_CHECKBOX,
        TOTU_GRAB,
        TOTU_GRAB_CHECKBOX,
        TOTU_HP,
        TOTU_RUSHCHECKBOX,
        TOTU_RUSHCOMBO,
        TOTU_NALNARI,
        TOTU_NISHIKIYAMA,
        TOTU_OK,
        TOTU_START,
        TOTU_END
    };

protected:
    CUITutorial();
    CUITutorial(const CUITutorial& rhs);
    virtual ~CUITutorial() = default;

public:
    _bool isCloseCurrentUIAnim();

public:
    void Set_State(_uint iState);
    _bool isShow(_uint iUIType) { return m_iState == iUIType; }
    _bool isAnimFinished() { return m_isStartEnd; }
    _bool isStartEnd() { return m_isStartEnd; }
    void Set_Text(wstring strText);

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;

private:
    _uint m_iState = { TOTU_END };
    _bool m_isStartEnd = { false };

    _float m_fStartDuration = { 1.f };
    _float m_fStartTime = { 0.f };

public:
    static CUITutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
