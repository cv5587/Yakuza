#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking :
    public CBase
{
private:
    CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPicking() = default;

public:
    _float Get_Z() { return m_fZ; }

public:
    HRESULT Initialize(HWND hWnd);
    _vector Picking(_bool* isSuccess);
    _bool Picking_UI(class CTransform* pUITransform); // UI 피킹 용. 파라미터에 UI의 TransformCom을 넣어주면 된다.

    /* 맵툴에서 사용하는 ID 판별 */
    float   FindObjID(_bool* isSuccess);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    _uint							m_iWinSizeX, m_iWinSizeY;

    class CGameInstance* m_pGameInstance = { nullptr };
    HWND							m_hWnd = {};

    ID3D11Texture2D* m_pTexture2D = { nullptr };

    _bool							m_isSuccess = { false };
    _float4							m_vPickPos = {};

    _float m_fZ = {0.f};

private:
    _float Compute_ProjZ(_float fX, _float fY);

    /* 맵툴에서 사용하는 ID 판별 */
    _float Find_ProjID(_float fX, _float fY);

public:
    static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
    virtual void Free() override;
};

END