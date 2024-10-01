#include "Chapter7_1.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Background.h"

CChapter7_1::CChapter7_1()
	: CMainQuest{}
{
}

HRESULT CChapter7_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//씬 카메라 이동 후 스크립트 텍스쳐 나옴
	m_pUIManager->Open_Scene(TEXT("Talk"));
	m_pUIManager->Start_Talk(m_iScriptIndex);

	return S_OK;
}

//대화가 끝나면 자동으로 다음 스테이지로 넘어감
_bool CChapter7_1::Execute()
{
	if (m_pUIManager->isTalkFinished())
		return true;

	return false;
}

CChapter7_1* CChapter7_1::Create(void* pArg)
{
	CChapter7_1* pInstance = new CChapter7_1();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CChapter7_1::Free()
{
	__super::Free();
}
