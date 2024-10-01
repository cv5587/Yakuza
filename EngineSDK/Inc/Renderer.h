#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CRenderer :
    public CBase
{
public:
    enum RENDERER_STATE {
        RENDER_OCCULUSION,
        RENDER_PRIORITY,
        RENDER_SHADOWOBJ,
        RENDER_NONBLENDER,
        RENDER_DECAL,
        RENDER_GLASS,
        RENDER_PUDDLE,
        RENDER_NONLIGHT_NONBLUR,
        RENDER_NONLIGHT,
        RENDER_BLENDER,
        RENDER_EFFECT,
        RENDER_UI,
        RENDER_END
    };

    enum SHADER_TYPE { DOWNSAMPLING, BLURX, BLURY, DOWNSAMPLING_DEPTH, SSAO, PBR, SHADER_TYPE_END };

    //SSAO 버퍼용
    struct SSAO_BUFFER
    {
        _float4 vSSAOKernal[64];
        _matrix WorldMatrix;
        _matrix ViewMatrix;
        _matrix ProjMatrix;
        _matrix ViewMatrixInv;
        _matrix ProjMatrixInv;
        _matrix CamViewMatrix;
        _matrix CamProjMatrix;
        
        _float fFar;
        _float fRadiuse;
        _float fSSAOBise;
        _float buffer1;
    };

    //PBR 버퍼용
    struct PBR_BUFFER
    {
        _matrix WorldMatrix;
        _matrix ViewMatrix;
        _matrix ProjMatrix;
        _matrix ViewMatrixInv;
        _matrix ProjMatrixInv;
        _matrix CamViewMatrix;
        _matrix CamProjMatrix;

        _float4 vCamPosition;
        _float fFar;
        _float fPadding[3];
    };


private:
    CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderer() = default;

public:
    void Set_HDR(_bool isHDR) { m_isHDR = isHDR; }
    void Set_HDRLight(_float fLight) { m_fHDRLight = fLight; } // HDR 밝기 조절(빛 세기)
    void Set_SSAO(_bool isSSAO) { m_isSSAO = isSSAO; }
    void Set_SSAORadiuse(_float fRadiuse) { m_fSSAORadiuse = fRadiuse; }
    void Set_SSAOBlur(_float fBlur) { m_fSSAOBlur = fBlur; }
    void Set_SSAOBias(_float fBias) { m_fSSAOBiae = fBias; }
    void Set_PBR(_bool isPBR) { m_isPBR = isPBR; }
    void Set_BOF(_bool isBOF) { m_isBOF = isBOF; }
    void Set_Shadow(_bool isShadow) { m_isShadow = isShadow; }
    void Set_RimLight(_bool isRimLight) { m_isRimLight = isRimLight; }
    void Set_RadialBlur(_bool isRadialBlur) { m_isRadialBlur = isRadialBlur; }
    void Set_MotionBlur(_bool isMotionBlur) { m_isMotionBlur = isMotionBlur; }
    void Set_InvertColor(_bool isInvertColor) { m_isInvertColor = isInvertColor; }
    void Set_InvertColorDuration(_float fDuration) { m_fInvertDuration = fDuration; }
    void Set_Vignette(_bool isVignette) { m_isVignette = isVignette; }
    void Set_InvertSaturationColor(_bool isInvertSaturationColor) { m_isInvertSaturationColor = isInvertSaturationColor; }
    void Set_InvertSaturationFactor(_float fFactor) { m_fSaturationFactor = fFactor; }
    void Set_VignetteIntensity(_float fIntensity) { m_fVignetteIntensity = fIntensity; }
    void Set_VignetteSmootness(_float fSmootness) { m_fVignetteSmootness = fSmootness; }
    void Set_RadialPower(_float fPower) { m_fRadialPower = fPower; }
    void Set_RadialSample(_int iSample) { m_iRadialSample = iSample; }
    void Set_AdjectBrightness(_float fBrightness) { m_fAdjectBrightness = fBrightness; }
    void Set_AdjectContrast(_float fContrast) { m_fAdjectContrast = fContrast; }
    void Set_AdjectSaturation(_float fSaturation) { m_fAdjectSaturation = fSaturation; }
    void Set_AdjectTint(_float4 vTine) { m_vAdjectTint = vTine; }

public:
    _bool isHDR() { return m_isHDR; }
    _bool isSSAO() { return m_isSSAO; }
    _bool isPBR() { return m_isPBR; }
    _bool isBOF() { return m_isBOF; }
    _bool isShadow() { return m_isShadow; }
    _bool isRimLight() { return m_isRimLight; }
    _bool isRadialBlur() { return m_isRadialBlur; }
    _bool isMotionBlur() { return m_isMotionBlur; }
    _bool isInvertColor() { return m_isInvertColor; }
    _bool isVignette() { return m_isVignette; }
    _bool isInvertLightnessColor() { return m_isInvertSaturationColor; }

    _float Get_HDRLight() { return m_fHDRLight; }
    _float Get_SSAORadiuse() { return m_fSSAORadiuse; }
    _float Get_SSAOBlur() { return m_fSSAOBlur; }
    _float Get_SSAOBias() { return m_fSSAOBiae; }
    _float Get_InvertSaturationFactor() { return m_fSaturationFactor; }
    _float Get_InvertColorDuration() { return m_fInvertDuration; }
    _float Get_VignetteIntensity() { return m_fVignetteIntensity; }
    _float Get_VignetteSmootness() { return m_fVignetteSmootness; }
    _float Get_RadialPower() { return m_fRadialPower; }
    _int Get_RadialSample() { return m_iRadialSample; }
    _float Get_AdjectBrightness() { return m_fAdjectBrightness; }
    _float Get_AdjectContrast() { return m_fAdjectContrast; }
    _float Get_AdjectSaturation() { return m_fAdjectSaturation; }
    _float4 Get_AdjectTint() { return m_vAdjectTint; }

    int     Get_RenderState() { return m_iRenderState; }

public:
    HRESULT Initialize();
    void Add_Renderer(RENDERER_STATE eRenderState, class CGameObject* pGameObject);
    void Draw();
    void Clear();

    void Occulusion_Culling_Draw(); // 컬링을 위해 Tick과 Late_Tick 사이에 돌릴 예정

#ifdef _DEBUG
public:
    _bool isDebugView() { return m_isDebugView; }
    void Set_DebugView(_bool isDebugView) { m_isDebugView = isDebugView; }

public:
    HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif

private:
    void Render_Priority();
    void Render_AnimSkinning();
    void Render_ShadowObjects();
    void Render_NonBlender();

    /* Map */
    void Render_Decal();
    void Render_Glass();
    void Render_Puddle();

    /*PBR*/
    void Render_PBR();

    void Render_LightAcc(); // Light 연산 + SSAO 합 + PBR
    void Render_CopyBackBuffer();
    void Render_DeferredResult();

    /* BOF */
    void Render_DeferredBlur();
    void Render_BOF();

    /* HDR*/
    void Render_DownSampling();
    void Render_CopyLuminance();
    void Render_AvgLuminance();
    void Render_LuminanceResult();

    /* NonLight인데 blur 안 들어감 */
    void Render_NonLight_NonBlur();

    /* Effect */
    void Render_RimLight();
    void Render_NonLight();//이펙트 시작
    void Render_Bloom();//블러
    void Render_FinalEffectBlend();//블러 합치기
    void Render_Blender();//기존 논블렌드 렌더
    void Render_Effect();// 파티클렌더 
    void Render_FinlaOIT();// 파티클 최종병합
    void Render_Distortion();//디스토션 파티클

    /*PostProcessing*/
    void Render_RadialBlur();
    void Render_Crack();
    void Render_InvertColor();
    void Render_InvertSaturationColor();
    void Render_Vignette();

    /* SSAO */
    void Render_SSAO();
    void Render_SSAOBlur();

    /*최종 렌더 (빼지마세요)*/
    void Render_FinalResult();

    /*최종적으로 화면에 그려질 색사 보정*/
    void Render_AdjustColor();

    void Render_UI();

    void Render_OcculusionDepth();

private:
    HRESULT Ready_Targets();
    HRESULT Ready_MRTs();
    HRESULT Ready_LightDepth();
    HRESULT Ready_SSAONoiseTexture();

    HRESULT Ready_OcculusionDepth();

#ifdef _DEBUG
private:
    list<class CComponent*>	m_DebugComponents;

private:
    void Render_Debug();
#endif // DEBUG

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    list<class CGameObject*> m_RenderObject[RENDER_END];

private:
    class CShader* m_pShader = { nullptr };
    class CComputeShader* m_pComputeShader[SHADER_TYPE_END] = {nullptr};
    class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

    _float4x4 m_WorldMatrix;
    _float4x4 m_ViewMatrix;
    _float4x4 m_ProjMatrix;

    ID3D11DepthStencilView* m_pLightDepthStencilView = { nullptr };
    ID3D11ShaderResourceView* m_pSSAONoiseView = { nullptr };

    ID3D11DepthStencilView* m_pOcculusionDepthView = { nullptr };

    ID3D11Buffer* m_pSSAOBuffer = { nullptr };
    ID3D11Buffer* m_pPBRBuffer = { nullptr };

    _bool m_isRadialBlur = { false };
    _bool m_isMotionBlur = { false };
    _bool m_isInvertColor = { false };
    _bool m_isInvertSaturationColor = { false };
    _bool m_isVignette = { false };

#ifdef _DEBUG
            private:
                _bool m_isHDR = { false };
                _bool m_isSSAO = { false };
                _bool m_isPBR = { false };
                _bool m_isBOF = { false };
                _bool m_isShadow = { false };
                _bool m_isRimLight = { true };
#else
            private:
                _bool m_isHDR = { true };
                _bool m_isSSAO = { true };
                _bool m_isPBR = { true };
                _bool m_isBOF = { true };
                _bool m_isShadow = { false };
                _bool m_isRimLight = { true };
#endif // DEBUG

    _float m_fHDRLight = { 1.f };

    _float m_fSSAORadiuse = { 0.02f };
    _float m_fSSAOBlur = { 2.f };
    _float m_fSSAOBiae = { 0.05f };
    _float4* m_vSSAOKernal;

    int      m_iRenderState = { RENDER_END };

    /* Invert Color*/
    _float m_fInvertTime = { 0.f };
    _float m_fInvertDuration = { 0.3f };

    /* Invert Lighness Color*/
    _float m_fSaturationFactor = { 0.2f };

    /* Vignette */
    _float m_fVignetteIntensity = { 0.5f };
    _float m_fVignetteSmootness = { 0.2f };

    /*RadialBlur*/
    _int m_iRadialSample = { 10 };
    _float m_fRadialPower = { 0.01 };

    /*AdjustColor*/
    float m_fAdjectBrightness = { 0.f }; // 밝기 조정
    float m_fAdjectContrast = { 1.f }; // 대비 보정
    float m_fAdjectSaturation = { 1.f }; // 채도 보정
    _float4 m_vAdjectTint = _float4(1.f, 1.f, 1.f, 0.f); // 색조 조정 (약간의 틴트를 추가)

#ifdef _DEBUG
    _bool m_isDebugView = { false };
#endif // _DEBUG

public:
    static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END