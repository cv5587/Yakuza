#include "Chapter2_0.h"

#include "GameInstance.h"

#include "Subtitle.h"
#include "Nishiki.h"

CChapter2_0::CChapter2_0()
	: CMainQuest{}
{
}

HRESULT CChapter2_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CSubtitle::SUBTITLE_DESC Desc{};
	Desc.iScriptChapter = m_iScriptIndex;

	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Subtitle"), TEXT("Layer_Subtitle"), &Desc)))
		return E_FAIL;

	m_pSubtitle = dynamic_cast<CSubtitle*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Subtitle"), 0));
	Safe_AddRef(m_pSubtitle);

	m_pNishiki = dynamic_cast<CNishiki*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Nishiki"), 0));
	Safe_AddRef(m_pNishiki);

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter2_0::Execute()
{
	if (m_pNishiki->isWalkFinished())
		return true;

	if (m_pNishiki->isWalkStartEnd())
		m_pSubtitle->Start_Subscript();
	
	return false;
}

CChapter2_0* CChapter2_0::Create(void* pArg)
{
	CChapter2_0* pInstance = new CChapter2_0();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter2_0::Free()
{
	__super::Free();
	Safe_Release(m_pSubtitle);
	Safe_Release(m_pNishiki);
}
