#pragma once
#include "MainQuest.h"

/*길거리로 나와서 사체업자 사무소로 가기*/
BEGIN(Client)
class CChapter4_0 :
   public CMainQuest
{
private:
	CChapter4_0();
	virtual ~CChapter4_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

private:
	class CNishiki* m_pNishiki = { nullptr };

public:
	static CChapter4_0* Create(void* pArg);
	virtual void Free();
};
END
