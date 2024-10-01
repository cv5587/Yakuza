#pragma once
#include "MainQuest.h"

/*가라오케에서 니시키야마와 대화*/
BEGIN(Client)
class CChapter3_0 :
   public CMainQuest
{
private:
	CChapter3_0();
	virtual ~CChapter3_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

private:
	class CNishiki* m_pNishiki = { nullptr };

public:
	static CChapter3_0* Create(void* pArg);
	virtual void Free();
};
END
