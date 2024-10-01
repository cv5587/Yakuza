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
    LEVEL Get_Level() { return m_iCurrentLevel; }

public:
    void Set_Level(LEVEL iLevel) { m_iCurrentLevel = iLevel; }
    void Set_Camera(CAMERA iCamNum) { m_iCamera = iCamNum; }
    CAMERA Get_Camera() { return m_iCamera; }

private:
    LEVEL                       m_iCurrentLevel = { LEVEL_END };
    CAMERA                      m_iCamera = { CAMERA_END };

public:
    virtual void Free();
};
END
