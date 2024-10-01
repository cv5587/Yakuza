#include "NoteSingle.h"
#include "GameInstance.h"

CNoteSingle::CNoteSingle(_uint iNoteType, _uint iButtonType, _float fStartTime)
    : CNoteBase(iNoteType, iButtonType, fStartTime)
{
}

CNoteSingle* CNoteSingle::Create(_uint iNoteType, _uint iButtonType, _float fStartTime)
{
	return new CNoteSingle(iNoteType, iButtonType, fStartTime);
}

void CNoteSingle::Free()
{
    __super::Free();
}