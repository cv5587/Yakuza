#pragma once
#include "Base.h"

#include "Client_Defines.h"
#include "CarChaseManager.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CCarChase :
    public CBase
{
public:
    enum STAGE_STATE { START, RUNNING, STAGE_END, STAGE_STATE_END };
    typedef struct tMonsterInfo {
        _uint iObjectIndex;
        _uint iMonsterLine;
        _uint iMonsterDir;
        _uint iReactorType;
        _int iWeaponType[2];
    }STAGE_MONSTER_INFO;

    typedef struct tStageInfo {
        _uint iPlayerLine;
        _uint iNextPlayerLine;
        _uint iStageDir;
        vector<STAGE_MONSTER_INFO> MonsterInfo;
    } STAGE_INFO;

public:
    typedef struct tCarChaseDesc {
        STAGE_INFO Info;
    }CARCHASE_DESC;

public:
    _uint Get_StageDir() { return m_Info.iStageDir; }

private:
    CCarChase();
    virtual ~CCarChase();

public:
    HRESULT Initialize(void* pArg);
    _bool Tick();

public:
    _bool Start();
    _bool Running();
    _bool End();

private:
    void Set_TaxiStageDir();

private:
    CGameInstance* m_pGameInstance = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };

private:
    STAGE_INFO m_Info = {};

    _uint m_iState = { STAGE_STATE_END };
    _bool m_isPlayerMove = { false };

public:
    static CCarChase* Create(void* pArg);
    virtual void Free();
};
END
