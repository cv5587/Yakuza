#pragma once
#include "MainQuest.h"

/* 니시키야마와 대화 */
BEGIN(Client)
class CChapter1_1 :
   public CMainQuest
{
private:
	CChapter1_1();
	virtual ~CChapter1_1() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

private:
	_bool m_isTitleFinished = { false };
	class CNishiki* m_pNishiki = { nullptr };

public:
	static CChapter1_1* Create(void* pArg);
	virtual void Free();
};
END
