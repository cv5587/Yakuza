#pragma once
#include "Quest.h"

/*
* [대화 퀘스트]
* 특정 타겟을 찾아가야 할 때 생성하는 퀘스트.
* ObjectIndex값을 넣어줘야 한다.
*/

BEGIN(Client)
class CTalkQuest :
    public CQuest
{
public:
    typedef struct tTalkQuestDesc : public CQuest::QUEST_DESC {
        _int iObjectIndex;
    } TALK_QUEST_DESC;
    
private:
    CTalkQuest();
    virtual ~CTalkQuest() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute();

private:
    _int m_iObjectIndex = { -1 };

public:
    static CTalkQuest* Create(void* pArg);
    virtual void Free();
};
END
