#pragma once
#include "Base.h"

#include "Client_Defines.h"

/*
Client 시스템에 전역적으로 필요한 정보들을 저장한다 (ex. 현재 레벨)
*/

BEGIN(Client)
class CSystemManager :
    public CBase
{
    DECLARE_SINGLETON(CSystemManager)

private:
    CSystemManager();
    virtual ~CSystemManager() = default;

public:
    void Set_Camera(CAMERA iCamNum) { m_iCamera = iCamNum; }
    CAMERA Get_Camera() { return m_iCamera; }
    void Set_ShadowViewPos(_vector vShadowPos) { m_vShadowViewPos = vShadowPos; }
    _vector Get_ShadowViewPos() { return m_vShadowViewPos; }

private:
    LEVEL                       m_iCurrentLevel = { LEVEL_END };

#ifdef _DEBUG
    CAMERA                      m_iCamera = { CAMERA_DEBUG };
#else
    CAMERA                      m_iCamera = { CAMERA_PLAYER };
#endif // _DEBUG



    _vector m_vShadowViewPos = { XMVectorSet(-0.6f, -0.7f, 0.1f, 1.f) };

public:
    virtual void Free();
};
END
