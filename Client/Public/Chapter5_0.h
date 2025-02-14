#pragma once
#include "MainQuest.h"

/*길거리로 나와서 사체업자 사무소로 가기*/
BEGIN(Client)
class CChapter5_0 :
   public CMainQuest
{
private:
	CChapter5_0();
	virtual ~CChapter5_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	static CChapter5_0* Create(void* pArg);
	virtual void Free();
};
END
