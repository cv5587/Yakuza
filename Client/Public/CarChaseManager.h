#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CCarChaseManager :
    public CBase
{
public:
    enum LINE_TYPE { LINE_A, LINE_B, LINE_C, LINE_D, LINE_END };
    enum STAGE_NUMBER { STAGE_1, STAGE_2, STAGE_3, STAGE_4, STAGE_5, STAGE_END };

public:
    void Set_Coll();

private:
    CCarChaseManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CCarChaseManager() = default;

public:
    HRESULT Initialize();
    _bool Tick();

private:
    CGameInstance* m_pGameInstance = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    vector<class CCarChase*> m_Stages;

private:
    _uint m_iCurrentStage = { 0 };
    _bool m_isFinished = { false };
    _bool m_isStart = { false };

private:
    HRESULT Ready_Stage();

public:
    static CCarChaseManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END
