#pragma once
#include "NoteBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteSingle :
    public CNoteBase
{
protected:
    CNoteSingle(_uint iNoteType, _uint iButtonType, _float fStartTime);
    virtual ~CNoteSingle() = default;

public:
    static CNoteSingle* Create(_uint iNoteType, _uint iButtonType, _float fStartTime);
    virtual void Free() override;
};
END
