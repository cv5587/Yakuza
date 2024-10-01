#pragma once
#include "Quest.h"

BEGIN(Client)
class CMainQuest abstract:
    public CQuest
{
public:
    typedef struct tMainQuestDesc : public CQuest::QUEST_DESC {
        _uint iScriptIndex;
    } MAIN_QUEST_DESC;

protected:
    CMainQuest();
    virtual ~CMainQuest() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;

protected:
    class CUIManager* m_pUIManager = { nullptr };
    class CFileTotalMgr* m_pFileTotalMgr = { nullptr };

protected:
    _uint m_iScriptIndex = { 0 };
    _bool m_isStart = { false };

protected:
    void Player_Stop(_bool isStop);
    void PlayerCom_Stop(_bool isStop);

public:
    virtual void Free();
};
END
