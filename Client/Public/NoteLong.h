#pragma once
#include "NoteBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteLong :
    public CNoteBase
{
protected:
    CNoteLong(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime);
    virtual ~CNoteLong() = default;

public:
    _float Get_EndTime() {
        return m_fEndTime;
    };

    _float Get_InputAcc() {
        return m_fInputAcc;
    };

    void Set_InputAcc(_float fAcc)
    {
        m_fInputAcc += fAcc;
    }

private:
    _float m_fEndTime = { 0.f };
    _float m_fInputAcc = { 0.f };         // 키를 얼마나 유지했는지

public:
    static CNoteLong* Create(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime);
    virtual void Free() override;
};
END
