#include "NoteLong.h"
#include "GameInstance.h"

CNoteLong::CNoteLong(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime)
    : CNoteBase(iNoteType, iButtonType, fStartTime),
    m_fEndTime{ fEndTime }
{
}

CNoteLong* CNoteLong::Create(_uint iNoteType, _uint iButtonType, _float fStartTime, _float fEndTime)
{
	return new CNoteLong(iNoteType, iButtonType, fStartTime, fEndTime);
}

void CNoteLong::Free()
{
    __super::Free();
}