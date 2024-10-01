#pragma once
#include "MainQuest.h"

//쿠제 보스 시작 모션 (버림)
BEGIN(Client)
class CChapter7_1 :
   public CMainQuest
{
private:
	CChapter7_1();
	virtual ~CChapter7_1() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	static CChapter7_1* Create(void* pArg);
	virtual void Free();
};
END
