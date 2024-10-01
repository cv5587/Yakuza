#include "Shader_NeoMetallic.hlsli"

float3 LinearToSRGB(float3 linearColor)
{
    // 감마 보정 적용
    return (linearColor <= 0.0031308) ?
           (linearColor * 12.92) :
           (1.055 * pow(linearColor, 1.0 / 2.4) - 0.055);
}

float RepeatingPatternBlendFactor(vector vMulti)
{
    float isCloth = 1.f - IsOEClothShader;
    float fMix = lerp(isCloth, vector(1.f, 1.f, 1.f, 1.f), Engine);
    
    float vMix2 = lerp(vMulti.z, vMulti.a, isCloth);
    float fDisableRDAndRT = 1.f - DisableRDRT;
    
    return vMix2 * fDisableRDAndRT;
}

vector DiffusePortion(vector vDiffuse, vector vRS, vector vRD, float Factor, float2 vTexcoord)
{
    vector fMix = lerp(vRD, vRS.r, IsY3Shader);
    vector fMix2 = lerp(fMix, vector(1.f, 1.f, 1.f, 1.f), SkinShader);
    vector fMultiply = lerp(vDiffuse, vDiffuse * fMix2, Factor);
    
    return vector(LinearToSRGB(fMultiply.xyz * fMultiply.xyz), 1.f);
}

vector SubSurface(vector vRD)
{
    float fMultiply = Engine * SkinShader;
    
    return lerp(vector(0.f, 0.f, 0.f, 0.f), vRD, fMultiply);
}

vector Get_Diffuse(float fMultiAlpha,  vector vDiffuse)
{
    float fFactor = fMultiAlpha * Engine;
    float3 vMix = lerp(float3(0.5f, 0.5f, 0.5f), float3(0.5f, 1.f, 0.5f), fFactor);
    
    float vMapRange1 = MapRange(vMix.r, 0.f, 0.5f, 0.f, 1.f);
    float vMapRange2 = MapRange(vMix.r, 0.5f, 1.f, 0.f, 1.f);

    vector vDiffuseResult = lerp(vDiffuse, vector(1.f, 1.f, 1.f, 1.f), vMapRange2);
    vector vMix2 = lerp(vector(0.f, 0.f, 0.f, 0.f), vDiffuseResult, vMapRange1);
    //vector vResultMix = vector(lerp(vMix2.xyz, vMix2.xyz, vMetallicFactor), 1.f);
    
    return vMix2;
}

bool CheckIfGreenNormalMap(float fX)
{
    return fX > 0.01f ? 0.f : 1.f;
}

float4 Overlay(float4 baseColor, float4 overlayColor, float factor)
{
    // 오버레이 효과 계산
    float3 resultColor;

    // 각 채널에 대해 오버레이 계산
    resultColor.rgb = (baseColor.rgb < 0.5) ? (2 * baseColor.rgb * overlayColor.rgb) : (1 - 2 * (1 - baseColor.rgb) * (1 - overlayColor.rgb));

    return lerp(baseColor, float4(resultColor, baseColor.a), factor); // 알파 값 유지
}

float4 Get_Normal(vector vNormal, vector vRT, float fFactor)
{
    //check if Green normal map
    float isCheck = CheckIfGreenNormalMap(vNormal.x);
    
    float fMix = lerp(vNormal.r, vNormal.a, isCheck);
    float4 vCombineNormal = float4(fMix, vNormal.g, 1.f, 0.f);
    
    isCheck = CheckIfGreenNormalMap(vRT.x);
    
    float fRTMix = lerp(vRT.r, vRT.a, isCheck);
    float4 vCombineRT = float4(fRTMix, vRT.g, 1.f, 0.f);

    return Overlay(vCombineNormal, vCombineRT, fFactor);
}

// RGB를 HSV로 변환하는 함수
float3 RGBtoHSV(float3 rgb)
{
    float3 hsv;
    float maxVal = max(rgb.r, max(rgb.g, rgb.b));
    float minVal = min(rgb.r, min(rgb.g, rgb.b));
    float delta = maxVal - minVal;

    // Hue 계산
    if (delta == 0.0)
    {
        hsv.x = 0.0; // Hue가 0
    }
    else if (maxVal == rgb.r)
    {
        hsv.x = 60.0 * fmod(((rgb.g - rgb.b) / delta), 6.0);
    }
    else if (maxVal == rgb.g)
    {
        hsv.x = 60.0 * (((rgb.b - rgb.r) / delta) + 2.0);
    }
    else
    {
        hsv.x = 60.0 * (((rgb.r - rgb.g) / delta) + 4.0);
    }

    // Saturation 계산
    hsv.y = (maxVal == 0.0) ? 0.0 : (delta / maxVal);

    // Value 계산
    hsv.z = maxVal;

    return hsv;
}

struct OE_SPECULAR
{
    float3 vSpecular;
    float fMixShaderFactor;
    float fRouhness;
};

OE_SPECULAR Neo_OE_Specular(vector vMulti, vector vRM, vector vRS)
{
    OE_SPECULAR Out = (OE_SPECULAR) 0;
    
    float3 vMix = lerp(vRS.xyz, float3(1.f, 1.f, 1.f), IsY3Shader);
    float3 vMix2 = lerp(vMix, vRM.xyz, SPShader);
    float vMix3 = lerp(vMulti.x, float3(1.f, 1.f, 1.f), SPShader);
    float3 hsv = RGBtoHSV(vSpecularColor.xyz);
    
    float fSubtract = 1.f - Engine;
    float Multiply = vMix3 * fSubtract;
    float Multiply2 = Multiply * hsv.z;
    float Mix4 = lerp(Multiply2, float3(1.f, 1.f, 1.f), SPShader);
    
    float MixShaderFactor = Mix4 * 0.25f; // Factor
    hsv = float3(hsv.r, hsv.g, 1.f);
    float3 vSpecular = vMix2 * hsv; // Speculer
    
    //Roughness
    
    vMix = lerp(vMulti.y, vMulti.z, AssetShader);
    vMix2 = lerp(vMix, float3(1.f, 1.f, 1.f), IsOEClothShader);
    vMix3 = lerp(vMix2, float3(1.f, 1.f, 1.f), SPShader);
    
    Multiply = fSpecularPower * vMix3;
    float Divide = fSpecularPower / 4.f;
    float Add = Divide + 2.f;
    Divide = 2 / Add;
    float fRouhness = sqrt(Divide);
    
    Out.fRouhness = fRouhness;
    Out.fMixShaderFactor = MixShaderFactor;
    Out.vSpecular = vSpecular;
    
    return Out;
}

struct UV_SHADER
{
    vector vImperfection;
    float2 RDRMRS;
    float2 RT;
};

UV_SHADER UV_Shader(float2 vTexcoord)
{
    UV_SHADER Out = (UV_SHADER) 0;
    
    //RT
    float RTX = max(g_fRTX, 1.f);
    float RTY = max(g_fRTY, 1.f);
    float2 RT = float2(RTX, RTY) * vTexcoord;
    
    //RD/RM/RS
    float RDRMRS_X = max(g_fRDRMRS_X, 1.f);
    float RDRMRS_Y = max(g_fRDRMRS_Y, 1.f);
    float2 RDRMRS = vTexcoord * float2(RDRMRS_X, RDRMRS_Y);
    
    //Imperfction
    vector Imperfection = pow(2.f, f_fImperfection_UV);
    Imperfection = vector(RT.xy, 0.f, 0.f) / Imperfection;
    
    Out.RT = RT;
    Out.RDRMRS = RDRMRS;
    Out.vImperfection = Imperfection;
    
    return Out;
}