#pragma once
#include "Camera.h"

#include "Client_Defines.h"

BEGIN(Client)
class CCutSceneCamera :
    public CCamera
{
private:
    CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCutSceneCamera(const CCutSceneCamera& rhs);
    virtual ~CCutSceneCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Store_PrevMatrix() {
        m_PrevMatrix = m_WorldMatrix;
    };
    void Store_StartMatrix(_fmatrix mat) {
        XMStoreFloat4x4(&m_StartMatrix, mat);
    };

    void On_Return() {
        m_isReturn = true;
    }

    void Set_StartFov(_float f)
    {
        m_fStartFov = f;
    }


private:
    // 이전 월드매트릭스로 선형보간하는 함수
    void    Return_PrevWorld(const _float& fTimeDelta);
    
    void    Play_FovLerp(const _float& fTimeDelta);

public:
    void    Reset_ReturnVariables();            // 관련 변수 초기화 함수

private:
    class CSystemManager* m_pSystemManager = { nullptr };

private:
    _float fConvertX = {90.f};        //90
    _float fConvertY = {-180.f};      //-180
    _float fConvertZ = {90.f};        //90

    // 카메라 돌아가는거 관련
    _bool       m_isReturn = { false };
    _float4x4    m_StartMatrix;         // 컷신 캠으로 변경 시 지금상태의 매트릭스를 저장
    _float4x4    m_PrevMatrix;         // 컷신 캠으로 변경 시 지금상태의 매트릭스를 저장
    _float       m_fLerpRatio = { 0.f };    // 보간 비율
    float       m_fElapsedTime = 0.0f; // 경과 시간
    float       m_fTotalLerpTime = 0.5f; // 보간에 걸리는 총 시간 (초 단위)
    float       m_fStartFov = 0.0f; // 시작하는 Fov값

public:
    static CCutSceneCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();

};
END
