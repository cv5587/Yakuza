#pragma once
#include "Base.h"

BEGIN(Engine)
class CMeterial :
    public CBase
{
public:
    typedef struct tMeterialDesc {
        _char strNeterialName[MAX_PATH];
        _bool isUVShader;

        _float fOpacity;
        _float fSpecularPower;
        _float fAssetShader;
        _float fDisableRDRS;
        _float fEngine;
        _float fIsClothShader;
        _float fSkinShader;
        _float fRough;
        _float fIsY3Shader;
        _float Imperfection;
        _float SPShader;

        _float fRTX;
        _float fRTY;
        _float fRDRMRS_X;
        _float fRDRMRS_Y;
        _float fImperfection_UV;
        _float fEngine_UV;
    } METERIAL_DESC;

private:
    CMeterial();
    virtual ~CMeterial() = default;

public:
    const string Get_Name() const { return string(m_Info.strNeterialName); }

public:
    HRESULT Initialize(METERIAL_DESC* pDesc);
    HRESULT Bind_Shader(class CShader* pShader);

private:
    METERIAL_DESC m_Info;

public:
    static CMeterial* Create(METERIAL_DESC* pDesc);
    virtual void Free();
};
END