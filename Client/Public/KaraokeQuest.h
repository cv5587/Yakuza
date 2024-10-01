#pragma once
#include "Quest.h"

/*
* [대화 퀘스트]
* 특정 타겟을 찾아가야 할 때 생성하는 퀘스트.
* ObjectIndex값을 넣어줘야 한다.
*/

BEGIN(Client)
class CKaraokeQuest :
    public CQuest
{
public:
    typedef struct tKaraokeQuestDesc : public CQuest::QUEST_DESC {
    } KARAOKE_QUEST_DESC;
    
private:
    CKaraokeQuest();
    virtual ~CKaraokeQuest() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute();

public:
    static CKaraokeQuest* Create(void* pArg);
    virtual void Free();
};
END
