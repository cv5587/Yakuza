#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	_float			Get_TimeDelta() const {
		return m_fTimeDelta; 
	}

public:
	void Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
	void Reset() { m_fSpeed = 1.f; }

public:
	HRESULT		Ready_Timer(void);
	void		Update_TimeDelta(void);

private:
	LARGE_INTEGER			m_FrameTime;
	LARGE_INTEGER			m_FixTime;
	LARGE_INTEGER			m_LastTime;
	LARGE_INTEGER			m_CpuTick;

	_float					m_fTimeDelta;
	_float m_fSpeed = { 1.f };

public:
	static CTimer*	Create(void);
	virtual void	Free();
};

END