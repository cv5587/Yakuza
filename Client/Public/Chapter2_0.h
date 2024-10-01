#pragma once
#include "MainQuest.h"

/*니시키야마와 걸으면서 대화*/
BEGIN(Client)
class CChapter2_0 :
   public CMainQuest
{
private:
	CChapter2_0();
	virtual ~CChapter2_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	class CNishiki* m_pNishiki = { nullptr };
	class CSubtitle* m_pSubtitle = { nullptr };

public:
	static CChapter2_0* Create(void* pArg);
	virtual void Free();
};
END
