#pragma once
#include "NoteBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteBurstHold :
    public CNoteBase
{
protected:
    CNoteBurstHold(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime);
    virtual ~CNoteBurstHold() = default;

public:
    _float Get_EndTime() {
        return m_fEndTime;
    }

    _uint Get_GreatCount() {
        return 6;
    }

    _uint Get_InputCount(){
        return m_iInputCount;
    }

    void Acc_Count() {
        m_iInputCount++;
    }

private:
    _float m_fEndTime = { 0.f };
    _uint m_iInputCount = { 0 };            //6¹øÄ¡¸é

public:
    static CNoteBurstHold* Create(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime);
    virtual void Free() override;
};
END
