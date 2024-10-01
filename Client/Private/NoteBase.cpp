#include "NoteBase.h"
#include "GameInstance.h"

CNoteBase::CNoteBase(_uint iNoteType, _uint iButtonType, _float fStartTime)
    : m_pGameInstance{ CGameInstance::GetInstance() },
    m_iNoteType{iNoteType},
    m_iButtonType{ iButtonType },
    m_fStartTime{ fStartTime }
{
    Safe_AddRef(m_pGameInstance);
}

void CNoteBase::Free()
{
    Safe_Release(m_pGameInstance);
}