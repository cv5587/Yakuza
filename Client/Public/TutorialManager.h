#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Client)
class CTutorialManager :
    public CBase
{
private:
    CTutorialManager();
    virtual ~CTutorialManager() = default;

public:
    HRESULT Initialize();
    _bool Tick();

public:
    void Start_Tutorial();

private:
    class CUIManager* m_pUIManager = { nullptr };
    vector<class CTutorial*> m_Tutorials = {};
    _uint m_iCurrentIndex = { 0 };
    _bool m_isFinished = { false };

private:
    HRESULT Ready_Tutorial();

public:
    static CTutorialManager* Create();
    virtual void Free();
};
END
