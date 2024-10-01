#pragma once
#include "MainQuest.h"

#include "Kuze.h"

//쿠제와 첫 대화
BEGIN(Client)
class CChapter7_0 :
   public CMainQuest
{
private:
	CChapter7_0();
	virtual ~CChapter7_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

private:
	class CKuze* m_pKuze = { nullptr };
	
public:
	static CChapter7_0* Create(void* pArg);
	virtual void Free();
};
END
