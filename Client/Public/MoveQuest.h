#pragma once
#include "Quest.h"

/*
* [길찾기 퀘스트]
* 특정 장소를 찾아가야 할 때 생성하는 퀘스트.
* TriggerIndex값을 넣어줘야 한다.
*/

BEGIN(Client)
class CMoveQuest :
    public CQuest
{
public:
    typedef struct tMoveQuestDesc : public CQuest::QUEST_DESC {
        _int iTriggerIndex;
    } MOVE_QUEST_DESC;

private:
    CMoveQuest();
    virtual ~CMoveQuest() = default;

public:
    void Set_Coll(_uint iTriggerIndex, _bool isColl) {
        if (iTriggerIndex == m_iTriggerIndex)
            m_isColl = isColl; 
    }

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute();

private:
    _int m_iTriggerIndex = { -1 };

public:
    static CMoveQuest* Create(void* pArg);
    virtual void Free();
};
END
