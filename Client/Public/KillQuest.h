#pragma once
#include "Quest.h"

/*
* [kill 퀘스트]
* 특정 타겟를 사냥해야 할 때 생성하는 퀘스트.
* TargetIndex값을 넣어줘야 한다.
*/

BEGIN(Client)
class CKillQuest :
    public CQuest
{
public:
    typedef struct tKillQuestDesc : public CQuest::QUEST_DESC {
        _int iTargetIndex;
    } KILL_QUEST_DESC ; 

private:
    CKillQuest();
    virtual ~CKillQuest() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute();

private:
    _int m_iTargetIndex = { -1 };

public:
    static CKillQuest* Create(void* pArg);
    virtual void Free();
};
END
