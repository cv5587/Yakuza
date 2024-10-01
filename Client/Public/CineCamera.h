#pragma once

#include "Camera.h"
#include "Client_Defines.h"


BEGIN(Client)
class CCineCamera :
    public CCamera
{
public:
    enum CAMERA_STATE {
        CAM_STATE_LERP,
        CAM_STATE_STAY,
        CAM_STATE_END
    };


public:
    typedef struct tCineCameraDesc : public CAMERA_DESC
    {
        _int        iFileNum;
    }CINE_CAMERA_DESC;


private:
    CCineCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCineCamera(const CCineCamera& rhs);
    virtual ~CCineCamera() = default;

public:
    virtual HRESULT                                 Initialize_Prototype() override;
    virtual HRESULT                                 Initialize(void* pArg) override;
    virtual void                                    Priority_Tick(const _float& fTimeDelta) override;
    virtual void                                    Tick(const _float& fTimeDelta) override;
    virtual void                                    Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT                                 Render() override;

    HRESULT                                         Load_CamBin(int iFileNum);
    void                                            Setting_Start_Cinemachine(int iCineNum); /* 이미 생성 후 */
    void                                            Initialize_Camera_Class();
    void                                            Set_Loop(_bool isLoop) {
        m_isLoop = isLoop;
    };

    int                                            Get_CameraIndex() { return m_iCurCamIndex; }

private:
    HRESULT                                         Import_Bin_Cam_Data_OnTool(CAMERAOBJ_IO* camData, int iFileNum);
    void			                                Mouse_Fix();
    void                                            Cam_Move_Handle_Setting(const _float& fTimeDelta);
    void                                            Cam_Lerp(const _float& fTimeDelta);
    void                                            Change_CamDesc();

    void                                            Set_Start();
    void                                            Set_Ending();

    void                                            Start_Lerp(const _float& fTimeDelta);

    XMVECTOR                                        Positioning_Using_Linear_Interpolation(XMVECTOR vCurPos, XMVECTOR vTargetPos, float fSpeed);



private:
    class CSystemManager*                           m_pSystemManager = { nullptr };
    _float                                          m_fSensor = { 0.f };
    vector<CAMERAOBJ_DESC>							m_vCamerasObjDesc;


    map<int, CAMERAOBJ_IO>                          m_CamerasStorage;

    int                                             m_iCurCamIndex = { 0 };

    float                                           m_fStayDeltaTime = { 0 };
    float                                           m_fStayTime = { 0 };

    float                                           m_fLerpDeltaTime = { 0 };
    float                                           m_fLerpTime = { 0 };

    float                                           m_fMoveDeltaTime = { 0 };
    float                                           m_fMoveTime = { 0 };
    float                                           m_fMoveSpeed = { 0 };

    bool                                            m_bFirstLerp = { false };
    bool                                            m_bLastLerp = { false };
    bool                                            m_bStart = { true }; /* 시작할때 선형보간 처리관련 */

    bool                                            m_bInitialize = { false };

    _bool                                           m_isLoop = { false };
    _bool                                           m_bChangeNextCam = { false };


    XMVECTOR                                        m_vStartEye;
    XMVECTOR                                        m_vStartFocus;

    XMVECTOR                                        m_vEye;
    XMVECTOR                                        m_vFocus;

    CAMERA_STATE                                    m_eCurCamState = { CAM_STATE_END };

public:
    static CCineCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();

};
END
