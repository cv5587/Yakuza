#pragma once
#include "Base.h"
#include "Client_Defines.h"

#include "ScriptManager.h"

BEGIN(Client)
class CQuestManager :
    public CBase
{
    DECLARE_SINGLETON(CQuestManager)

public:
    enum QUEST_TYPE { QUEST_MAIN, QUEST_KILL, QUEST_MOVE, QUEST_TALK, QUEST_KARAOKE, QUEST_END };
    enum CHAPTRE { CHAPTER_1, CHAPTER_2, CHAPTER_3, CHAPTER_4, CHAPTER_5, CHAPTER_6, CHAPTER_7, CHAPTER_8, CHAPTER_END }; //레벨 진입 마다 나오는거 
    typedef struct tQuestInfo {
        tQuestInfo(_uint iType, _uint iScriptIndex, _int iQuestIndex, _int iNextQuestIndex, _int iTargetIndex = -1, _int iTriggerIndex = -1, _int iObjectIndex = -1)
            : iType{ iType },
            iScriptIndex{ iScriptIndex },
            iQuestIndex{ iQuestIndex },
            iNextQuestIndex{ iNextQuestIndex },
            iTargetIndex{ iTargetIndex },
            iTriggerIndex{ iTriggerIndex },
            iObjectIndex{ iObjectIndex } {}

        _uint iType;
        _uint iScriptIndex;

        _int iQuestIndex;
        _int iNextQuestIndex;
        _int iTargetIndex; //Kill
        _int iTriggerIndex; //Move
        _int iObjectIndex; // Talk
    } QUEST_INFO;

private:
    CQuestManager();
    virtual ~CQuestManager() = default;

public:
    _uint Get_ScriptIndex();
    _uint Get_CurrentQuestType();
    _uint Get_CurrentQuestTriggerIndex();
    _bool isFightStart();

public:
    void Set_Chapter(_uint iIndex) { m_iCurrentChapter = iIndex; }
    void Set_CurrentColl(_bool isColl = true);

public:
    HRESULT Initialize();
    void Start_Quest(_uint iChapter);
    _bool Execute();

private:
    class CQuest* m_pCurrentQuest = { nullptr };

    map<_uint, vector<QUEST_INFO>> m_QuestInfo; // 퀘스트 데이터 
    _int m_iCurrentQuestIndex = { -1 };
    _int m_iCurrentChapter = { -1 };

private:
    HRESULT Ready_Quest();
    HRESULT Add_KillQuest(_int iQuestIndex, _int iNextQuestIndex, _int iTargetIndex, _int iScriptIndex);
    HRESULT Add_MoveQuest(_int iQuestIndex, _int iNextQuestIndex, _int iTriggerIndex, _int iScriptIndex);
    HRESULT Add_TalkQuest(_int iQuestIndex, _int iNextQuestIndex, _int iObjectIndex, _int iScriptIndex);
    HRESULT Add_MainQuest(_int iQuestIndex, _int iNextQuestIndex, _int iObjectIndex, _int iScriptIndex);
    HRESULT Add_KaraokeQuest(_int iQuestIndex, _int iNextQuestIndex, _int iObjectIndex, _int iScriptIndex);

public:
    virtual void Free();
};

END

