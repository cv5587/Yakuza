#pragma once
#include "MainQuest.h"

BEGIN(Client)
class CChapter1_0 :
   public CMainQuest
{
private:
	CChapter1_0();
	virtual ~CChapter1_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	class CBackground* m_pBackground = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };
	_bool m_isTitleFinished = { false };
	_bool m_isTitleStart = { false };

	_bool m_isStartCameraEnd = { false };

	_float m_fStartDuration = {8.f};
	_float m_fStartTime = { 0.f };

public:
	static CChapter1_0* Create(void* pArg);
	virtual void Free();
};
END
