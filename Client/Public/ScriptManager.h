#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Client)
class CScriptManager :
    public CBase
{
public:
    typedef struct tScriptInfo {
        wstring strName;
        wstring strLine;

        tScriptInfo(wstring strName, wstring strLine) : strName{ strName }, strLine{ strLine } {}
    }SCRIPT_INFO;

private:
    CScriptManager();
    virtual ~CScriptManager() = default;

public:
    const SCRIPT_INFO Get_Script(_uint iIndex, _int iScriptIndex);

public:
    HRESULT Initialize();

private:
    vector<map<_int, SCRIPT_INFO>> m_Scripts;

private:
    void Ready_Script_1();
    void Ready_Script_2();
    void Ready_Script_3();
    void Ready_Script_4();
    void Ready_Script_5();
    void Ready_Script_6();
    void Ready_Script_7();
    void Ready_Script_8();
    void Ready_Script_9();
    void Ready_Script_10();

public:
    static CScriptManager* Create();
    virtual void Free();
};

END