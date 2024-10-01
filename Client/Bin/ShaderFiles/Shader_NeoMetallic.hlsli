#include "Engine_Shader_Defines.hlsli"
#include "Shader_Client_Defines.hlsli"

struct MRM_OUT
{
    float fRed;
    float fBlue;
};

struct COMBINE_OUT
{
    float fMetalness;
    float fRoughness;
    float fSpeclure;
    float fFactor;
};

float MapRange(float value, float fromMin, float fromMax, float toMin, float toMax)
{
    // 입력 값을 0에서 1 범위로 정규화
    float normalizedValue = (value - fromMin) / (fromMax - fromMin);

    // 정규화된 값을 새로운 범위로 변환
    return toMin + normalizedValue * (toMax - toMin);
}


MRM_OUT Line124(MRM_OUT vMT)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vMT.fRed;
    float fBlue = vMT.fBlue;
    
    //line124
    fRed = (fRed * -1.f) + 1.f;
    fBlue = (fBlue * -1.f) + 1.f;
    
    fRed = fRed + fRed;
    fBlue = fBlue + fBlue;
    
    //line129
    fRed *= -1.f;
    fBlue *= -1.f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

// RM은 line125에서 받아옴
MRM_OUT Line129(MRM_OUT vMT, MRM_OUT vRM)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vMT.fRed;
    float fBlue = vMT.fBlue;
    
    //line129
    fRed *= -1.f;
    fBlue *= -1.f;
    
    fRed = (fRed * vRM.fRed) + 1.f;
    fBlue = (fBlue * vRM.fBlue) + 1.f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

MRM_OUT Line118(MRM_OUT vRM)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vRM.fRed;
    float fBlue = vRM.fBlue;
    
    //line 118
    fRed -= 0.5f;
    fBlue -= 0.5f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

MRM_OUT Line119(MRM_OUT vRM, float fMultiAlpha)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vRM.fRed;
    float fBlue = vRM.fBlue;
    
        //line 119
    fRed = (fRed * fMultiAlpha) + 0.5f;
    fBlue = (fBlue * fMultiAlpha) + 0.5f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}


MRM_OUT Line125(MRM_OUT vRM)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vRM.fRed;
    float fBlue = vRM.fBlue;
    
    fRed = (fRed * -1.f) + 1.f;
    fBlue = (fBlue * -1.f) + 1.f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

MRM_OUT Line126(MRM_OUT vRM, MRM_OUT vMT)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vRM.fRed;
    float fBlue = vRM.fBlue;
    
    fRed *= vMT.fRed;
    fBlue *= vMT.fBlue;
    
    fRed += fRed;
    fBlue += fBlue;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

//참,거짓 반환
MRM_OUT Line127(MRM_OUT vMT)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    float fRed = vMT.fRed;
    float fBlue = vMT.fBlue;
    
    fRed = vMT.fRed > 0.5f ? 1.f : 0.f;
    fBlue = vMT.fBlue > 0.5f ? 1.f : 0.f;
    
    Out.fRed = fRed;
    Out.fBlue = fBlue;
    
    return Out;
}

MRM_OUT Line130(MRM_OUT vMT, MRM_OUT vRM, MRM_OUT vFactor)
{
    MRM_OUT Out = (MRM_OUT) 0;
    
    Out.fRed = lerp(vMT.fRed, vRM.fRed, vFactor.fRed);
    Out.fBlue = lerp(vMT.fBlue, vRM.fBlue, vFactor.fBlue);
    
    return Out;
}


COMBINE_OUT CombineMTAndRM(vector vMT, vector vRM, float fMetelnessOverride, float fGlossinessOverride)
{
    COMBINE_OUT Out = (COMBINE_OUT) 0;
    
    MRM_OUT vMTDesc = (MRM_OUT) 0;
    vMTDesc.fRed = vMT.r;
    vMTDesc.fBlue = vMT.b;
    
    float fMTAlpha = vMT.a;
    
    MRM_OUT vRMDesc = (MRM_OUT) 0;
    vRMDesc.fRed = vRM.r;
    vRMDesc.fBlue = vRM.b;
    
    MRM_OUT vLine118Result = Line118(vRMDesc);
    MRM_OUT vLine119Result = Line119(vLine118Result, fMTAlpha);
    MRM_OUT vLine126Result = Line126(vLine119Result, vMTDesc);
    MRM_OUT vLine125Result = Line125(vLine119Result);
    MRM_OUT vLine127Result = Line127(vMTDesc);

    MRM_OUT vLine124Result = Line124(vMTDesc);
    MRM_OUT vLine129Result = Line129(vLine124Result, vLine125Result);
    
    MRM_OUT vLine130 = Line130(vLine129Result, vLine126Result, vLine127Result);
    
    vLine130.fBlue = min(vLine130.fBlue, 0.996f);
    vector vResult = vector(vLine130.fRed, 0.f, vLine130.fBlue, 1.f);
    vResult *= 0.5f;

    float glossOverride = max(fGlossinessOverride, 0.f);
    
    vector vMax1 = lerp(vResult.r, fMetelnessOverride, fMetelnessOverride);
    vector vMax2 = lerp(vResult.b, fGlossinessOverride, glossOverride);

    Out.fMetalness = vMax1;
    Out.fRoughness = vMax2;
    
    return Out;
}


float Neo_Glossiness(float3 vColor, vector vMulti)
{
    float3 vMix = lerp(vColor.y, vMulti.x, Engine);
    float3 vMix2 = lerp(vColor.z, vMix, AssetShader);
    
    float fFactor = IsOEClothShader * (1.f - Engine);
    
    float3 vInvertedColor = lerp(vMix2, 1.f - vMix2, 1.f - Rough);
    
    return lerp(vInvertedColor, float3(0.5f, 0.5f, 0.5f), fFactor);
}

// Metallic과 Glossiness 구할 수 있음.
COMBINE_OUT Neo_MetallicAndGlossiness(vector vMulti, vector vRM)
{
    COMBINE_OUT Out = (COMBINE_OUT) 0;
    
    COMBINE_OUT MultiAndRM = CombineMTAndRM(vMulti, vRM, 0.f, 0.f);
    
    vector vRefl = vector(0.5f, 1.f, 0.5f, 1.f);
    vector vMultiAndRM = vector(MultiAndRM.fMetalness, 1.f, MultiAndRM.fRoughness, vMulti.a);
    
    COMBINE_OUT ReflCombine = CombineMTAndRM(vMultiAndRM, vRefl, 0.f, 0.f);
    float3 vResult = float3(ReflCombine.fMetalness, 1.f, ReflCombine.fRoughness);
    
    float3 vResultMix = lerp(vMulti.xyz, vResult, Engine);
    float fRoughness = Neo_Glossiness(vResultMix, vMulti);
    
    //factor 구하기
    bool fFactor = (AssetShader + Engine) == 2.f;
    
    float fMix = lerp(vResultMix.r, vMulti.y, fFactor);
    
    Out.fMetalness = lerp(vector(0.f, 0.f, 0.f, 0.f), MapRange(fMix, 0.1f, 1.f, 0.f, 1.f), Engine);
    Out.fRoughness = fRoughness;
    Out.fFactor = MapRange(fMix, 0.1f, 0.f, 0.f, 1.f);
    Out.fSpeclure = lerp(vector(0.f, 0.f, 0.f, 0.f), MapRange(fMix, 0.f, 0.1f, 0.1f, 1.f), Engine);
    
    return Out;
}