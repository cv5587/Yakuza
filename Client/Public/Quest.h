#pragma once
#include "Base.h"

#include "Client_Defines.h"

#include "QuestManager.h"

BEGIN(Engine)
class CGameInstance;
END 

BEGIN(Client)
class CQuest abstract :
    public CBase
{
public:
    typedef struct tQuestDesc {
        _uint iType;
        _int iQuestIndex;
        _int iNextQuestIndex;
    } QUEST_DESC;

protected:
    CQuest();
    virtual ~CQuest() = default;

public:
    _uint Get_Type() { return m_iType; }
    _int Get_QuestIndex() { return m_iQuestIndex; }
    _int Get_NextQuestIndex() { return m_iNextQuestIndex; }

public:
    void Set_Coll(_bool isColl) { m_isColl = isColl; }

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute() = 0;

protected:
    CGameInstance* m_pGameInstance = { nullptr };

protected:
    _uint m_iType = { CQuestManager::QUEST_END };
    _int m_iQuestIndex = { -1 };
    _int m_iNextQuestIndex = { -1 };

    _bool m_isCompleted = { false };

    _bool m_isColl = { false };

public:
    virtual void Free();
};
END
