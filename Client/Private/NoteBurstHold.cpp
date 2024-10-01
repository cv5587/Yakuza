#include "NoteBurstHold.h"
#include "GameInstance.h"

CNoteBurstHold::CNoteBurstHold(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime)
    : CNoteBase(iNoteType, iButtonType, fStartTime),
    m_fEndTime{ fEndTime }
{
}

CNoteBurstHold* CNoteBurstHold::Create(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime)
{
	return new CNoteBurstHold(iNoteType, iButtonType, fStartTime, fEndTime);
}

void CNoteBurstHold::Free()
{
    __super::Free();
}