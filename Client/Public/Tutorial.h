#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CTutorial :
    public CBase
{
public:
    enum TUTORIAL_STATE { STATE_START, STATE_RUNNGIN, STATE_END, TUTORIAL_STATE_END };
    typedef struct tTutorialDesc {
        _uint iConstance;
        string strPlayerSkillName;
        vector<_uint> TutorialUI; // 튜토리얼 UI Index 여러개일경우를 대비
        _uint iTutorialCheckUI; // 튜토리얼 체크 UI Index
        _bool isFinished; // 마지막인지 체크
    }TUTORIAL_DESC;

private:
    CTutorial();
    virtual ~CTutorial() = default;

public:
    HRESULT Intialize(void* pArg);
    _bool Execute();

    _bool Start();
    _bool Running();
    _bool End();

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    class CPlayer* m_pPlayer = { nullptr };

private:
    vector<_uint> m_TutorialUIIndex;
    _uint m_iTutorialCheckUIIndex;
    _uint m_iCoustance = { false };
    _uint m_iCount = { 0 };
    _bool m_isFinished = { false };

    string m_strPlayerSkillName = { "" };

private:
    _uint m_iState = { TUTORIAL_STATE_END };
    _uint m_iTutorialIndex = { 0 };

    _bool m_isStart = { true };

public:
    static CTutorial* Create(void* pArg);
    virtual void Free();
};
END
