#include "Shader_Neo.hlsli"


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

    return Out;
}

struct VS_OUT_LIGHTDEPTH
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
    VS_OUT_LIGHTDEPTH Out = (VS_OUT_LIGHTDEPTH) 0;

    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

// LightDepth용 GS
struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    
    uint fIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void GS_MAIN_LIGHTDEPTH(triangle GS_IN In[3], inout TriangleStream<GS_OUT> Out)
{
    GS_OUT Output[3] = (GS_OUT[3]) 0;
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float4 vPosition = mul(In[j].vPosition, mul(g_ViewMatrixArray[i], g_ProjMatrixArray[i]));
            Output[j].vPosition = vPosition;
            Output[j].vProjPos = vPosition;
            Output[j].vTexcoord = In[j].vTexcoord;
            Output[j].fIndex = i;
            Out.Append(Output[j]);
        }
        Out.RestartStrip();
    }
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vRS : SV_Target4;
    vector vMulti : SV_Target5;
    vector vRD : SV_Target6;
};

struct PS_OUT_COLOR
{
    vector vDiffuse : SV_TARGET0;
};

struct PS_MAIN_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vSurface : SV_TARGET3; // vector(metallic, goughness, speculer, 0.f)
    vector vOEShader : SV_Target4;
};

PS_MAIN_OUT PS_MAIN(PS_IN In)
{
    PS_MAIN_OUT Out = (PS_MAIN_OUT) 0;
    UV_SHADER UV = (UV_SHADER) 0;
    if (g_isUVShader)
        UV = UV_Shader(In.vTexcoord);
    
    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    vector vMulti = g_isMulti ? g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = g_isRD ? g_RDTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = g_isRS ? g_RSTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = g_isRM ? g_RMTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = g_isRT ? g_RTTexture.Sample(LinearSampler, g_isUVShader ? UV.RT : In.vTexcoord) : vector(0.5f, 0.5f, 1.f, 0.5f);
    
     //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuse = DiffusePortion(vDiffuseDesc, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuse = Get_Diffuse(vMulti.a, vDiffuse); // Diffuse 최종
    
    //노말 벡터 구하기
    if (g_isNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
            //Tangent Normal 구하기 
        vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);

        vNormalDesc = vNormalDesc * 2.f - 1.f;
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vector vNormalBTN = normalize(vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f));
        Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    }
    else
    {
        vector vNormalDesc = In.vNormal;
        Out.vNormal = vector(vNormalDesc.xyz * 0.5f + 0.5f, 0.f);
    }
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, g_ExcludeSSAO < 0.1f ? 0.f : 1.f);
    Out.vDiffuse = vDiffuse;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, Result.fFactor);
    Out.vOEShader = vector(OEResult.fRouhness, OEResult.vSpecular);
    
    return Out;
}

PS_MAIN_OUT PS_MAIN_FAR(PS_IN In)
{
    PS_MAIN_OUT Out = (PS_MAIN_OUT) 0;
    UV_SHADER UV = (UV_SHADER) 0;
    if (g_isUVShader)
        UV = UV_Shader(In.vTexcoord);
    
    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    vector vMulti = g_isMulti ? g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = g_isRD ? g_RDTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = g_isRS ? g_RSTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = g_isRM ? g_RMTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = g_isRT ? g_RTTexture.Sample(LinearSampler, g_isUVShader ? UV.RT : In.vTexcoord) : vector(0.5f, 0.5f, 1.f, 0.5f);
    
     //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuse = DiffusePortion(vDiffuseDesc, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuse = Get_Diffuse(vMulti.a, vDiffuse); // Diffuse 최종
    
    //노말 벡터 구하기
    if (g_isNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
            //Tangent Normal 구하기 
        vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);

        vNormalDesc = vNormalDesc * 2.f - 1.f;
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vector vNormalBTN = normalize(vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f));
        Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    }
    else
    {
        vector vNormalDesc = In.vNormal;
        Out.vNormal = vector(vNormalDesc.xyz * 0.5f + 0.5f, 0.f);
    }
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, g_ExcludeSSAO);
    Out.vDiffuse = vDiffuse;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, Result.fFactor);
    Out.vOEShader = vector(OEResult.fRouhness, OEResult.vSpecular);
    
    return Out;
}

PS_MAIN_OUT PS_GLASSDOOR(PS_IN In)
{

    PS_MAIN_OUT Out = (PS_MAIN_OUT) 0;
    UV_SHADER UV = (UV_SHADER) 0;
    if (g_isUVShader)
        UV = UV_Shader(In.vTexcoord);
    
    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    vector vMulti = g_isMulti ? g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = g_isRD ? g_RDTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = g_isRS ? g_RSTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = g_isRM ? g_RMTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = g_isRT ? g_RTTexture.Sample(LinearSampler, g_isUVShader ? UV.RT : In.vTexcoord) : vector(0.5f, 0.5f, 1.f, 0.5f);
    
     //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuse = DiffusePortion(vDiffuseDesc, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuse = Get_Diffuse(vMulti.a, vDiffuse); // Diffuse 최종
    
    //노말 벡터 구하기
    if (g_isNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
            //Tangent Normal 구하기 
        vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);

        vNormalDesc = vNormalDesc * 2.f - 1.f;
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vector vNormalBTN = normalize(vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f));
        Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    }
    else
    {
        vector vNormalDesc = In.vNormal;
        Out.vNormal = vector(vNormalDesc.xyz * 0.5f + 0.5f, 0.f);
    }
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    
    vDiffuse = vector(0, 0, 0, 0);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, g_ExcludeSSAO);
    Out.vDiffuse = vDiffuse;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, Result.fFactor);
    Out.vOEShader = vector(OEResult.fRouhness, OEResult.vSpecular);
    
    return Out;
}



PS_MAIN_OUT PS_MAIN_AlphaMask(PS_IN In)
{
    PS_MAIN_OUT Out = (PS_MAIN_OUT) 0;
    UV_SHADER UV = (UV_SHADER) 0;
    if (g_isUVShader)
        UV = UV_Shader(In.vTexcoord);
    
    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuseDesc.a < 0.6)
        discard;
   
    vector vMulti = g_isMulti ? g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = g_isRD ? g_RDTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = g_isRS ? g_RSTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = g_isRM ? g_RMTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = g_isRT ? g_RTTexture.Sample(LinearSampler, g_isUVShader ? UV.RT : In.vTexcoord) : vector(0.5f, 0.5f, 1.f, 0.5f);
    
     //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuse = DiffusePortion(vDiffuseDesc, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuse = Get_Diffuse(vMulti.a, vDiffuse); // Diffuse 최종
    
    //노말 벡터 구하기
    if (g_isNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
            //Tangent Normal 구하기 
        vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);

        vNormalDesc = vNormalDesc * 2.f - 1.f;
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vector vNormalBTN = normalize(vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f));
        Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    }
    else
    {
        vector vNormalDesc = In.vNormal;
        Out.vNormal = vector(vNormalDesc.xyz * 0.5f + 0.5f, 0.f);
    }
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, g_ExcludeSSAO);
    Out.vDiffuse = vDiffuse;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, Result.fFactor);
    Out.vOEShader = vector(OEResult.fRouhness, OEResult.vSpecular);
    
    return Out;
}

PS_OUT_COLOR PS_MAIN_LightMask_Alpha(PS_IN In)
{
    // 모양대로 자르는거
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bLightCut)
    {
        if (vDiffuse.a < 0.6 || (0 == vDiffuse.r && 0 == vDiffuse.g && 0 == vDiffuse.b))
            discard;
    }
 
    
    Out.vDiffuse = vDiffuse;
    
    return Out;
}


PS_MAIN_OUT DEFAULT_SIGN_PASS(PS_IN In)
{
    PS_MAIN_OUT Out = (PS_MAIN_OUT) 0;
    UV_SHADER UV = (UV_SHADER) 0;
    if (g_isUVShader)
        UV = UV_Shader(In.vTexcoord);
    
    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    vector vMulti = g_isMulti ? g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = g_isRD ? g_RDTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = g_isRS ? g_RSTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = g_isRM ? g_RMTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = g_isRT ? g_RTTexture.Sample(LinearSampler, g_isUVShader ? UV.RT : In.vTexcoord) : vector(0.5f, 0.5f, 1.f, 0.5f);
    
     //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuse = DiffusePortion(vDiffuseDesc, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuse = Get_Diffuse(vMulti.a, vDiffuse); // Diffuse 최종
    
    //노말 벡터 구하기
    if (g_isNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
            //Tangent Normal 구하기 
        vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);

        vNormalDesc = vNormalDesc * 2.f - 1.f;
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vector vNormalBTN = normalize(vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f));
        Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    }
    else
    {
        vector vNormalDesc = In.vNormal;
        Out.vNormal = vector(vNormalDesc.xyz * 0.5f + 0.5f, 0.f);
    }
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, g_ExcludeSSAO);
    Out.vDiffuse = vDiffuse;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, Result.fFactor);
    Out.vOEShader = vector(OEResult.fRouhness, OEResult.vSpecular);
    
    return Out;
}
   
PS_OUT PS_MAIN_Lamp(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector emissive;
    if (true == g_bExistRMTex)
    {
        emissive = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    }
    else
    {
        emissive = vDiffuse.r;
    }
   
    vector finalColor = saturate(vDiffuse + (emissive.r) * float4(1, 0.5, 0, 1) * 0.5);
    
    if (finalColor.r == 1 && finalColor.g == 1 && finalColor.b == 1)
        finalColor = float4(1, 0.5, 0, 1);
    
  
    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT_COLOR PS_DECAL(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);  
   
    if(vDiffuse.a < 0.3)
        discard;
    
    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT_COLOR PS_BloomWhite(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    if (vDiffuse.r + vDiffuse.g + vDiffuse.b > 1.8)
    {
        // 약간만 bloom 되게끔
        Out.vDiffuse = vDiffuse * 0.5;
    }
    else
    {
        discard;
    }
    
    return Out;
  

}


PS_OUT_COLOR PS_MaskEmissive(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector emissiveColor;
    
    emissiveColor.rgb = vDiffuse.rgb * 0.4;
    
    emissiveColor.a = vDiffuse.a;
    
    if (vDiffuse.r < 0.3 && vDiffuse.g < 0.3 && vDiffuse.b < 0.3 && vDiffuse.a < 0.3)
        discard;
    
    Out.vDiffuse = vDiffuse + emissiveColor;
    
    return Out;
  

}

PS_OUT_COLOR PS_StrongBloom(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    
   
    if (vDiffuse.r + vDiffuse.g + vDiffuse.b > 1.8)
    {
        // 약간만 bloom 되게끔
        Out.vDiffuse = vDiffuse;
    }
    else
    {
        discard;
    }
    
    return Out;
  

}

PS_OUT_COLOR PS_Compulsory_DECAL(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    vDiffuse.a = 0.3;
    
    Out.vDiffuse = vDiffuse;
    
    return Out;
}


PS_OUT_COLOR PS_DYNAMIC_SMALL(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
    
    
    
    In.vTexcoord.x += g_fTimeDelta;
    
    //if (1 < In.vTexcoord.x)
    //    In.vTexcoord.x = 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    if(vDiffuse.a < 0.4)
        discard;
    
    vector emissiveColor = float4(0, 0, 0, 0);
    
    emissiveColor.rgb = vDiffuse.rgb * 0.4;
    
    Out.vDiffuse = vDiffuse + emissiveColor;
    
    return Out;
}




PS_OUT_COLOR PS_DynamicBloom(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
 
    In.vTexcoord.x += g_fTimeDelta;
    
    //if (1 < In.vTexcoord.x)
    //    In.vTexcoord.x = 0;
 
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    if (vDiffuse.a < 0.4)
        discard;
   
    if (vDiffuse.r + vDiffuse.g + vDiffuse.b > 1)
    {
        // 약간만 bloom 되게끔
        Out.vDiffuse = vDiffuse;
    }
    else
    {
        discard;
    }
    
    return Out;
  
}

PS_OUT_COLOR PS_DynamicFast(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;

    // 텍스처 컬러를 샘플링
    vector diffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    // 특정 영역 계산
    float2 emissiveCenter = float2(0.5, 0.5) + float2(sin(g_fTimeDelta), cos(g_fTimeDelta)) * 0.25;
    float emissiveRadius = 0.2;
    float distanceFromCenter = distance(In.vTexcoord, emissiveCenter);

    // 특정 영역에 대한 emissive 효과 적용
    float emissiveFactor = saturate(1.0 - distanceFromCenter / emissiveRadius);
    

    vector emissiveColor = diffuse * 0.4;
    if (true == g_bEmissiveTex)
        emissiveColor = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

    // 최종 컬러 계산
    float4 finalColor = diffuse;
    
    if (emissiveFactor > 0.8)
        finalColor += emissiveColor;
    
    Out.vDiffuse = finalColor;

    return Out;
}


PS_OUT_COLOR PS_GLASSCOLOR(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR) 0;
    
    float2 vRefractTexCoord;
    vRefractTexCoord.x = In.vProjPos.x / In.vProjPos.w / 2.0f + 0.5f;
    vRefractTexCoord.y = -In.vProjPos.y / In.vProjPos.w / 2.0f + 0.5f;


    // Refract - 유리 뒤에 비치는 씬
    float4 vRefractColor = g_RefractionTexture.Sample(LinearSampler, vRefractTexCoord);
    float4 vGlassTexColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vFinalColor = lerp(vRefractColor, vGlassTexColor, 0.5f);
    
    vFinalColor.a = 1;
    Out.vDiffuse = vFinalColor;
    
    return Out;
}



struct PS_IN_LIGHTDEPTH
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT_LIGHTDEPTH
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass GlassDoorPass //1, 정보값만
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_GLASSDOOR();
    }

    pass BlackMaskPass //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_AlphaMask();
    }


    pass ALPHALightMaskPass //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LightMask_Alpha();
    }

    pass SIGNPASS //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 DEFAULT_SIGN_PASS();
    }

    pass LampPass //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Lamp();
    }

    pass DECALPASS //6->decal blend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DECAL();
    }

    pass BloomWhite //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BloomWhite();
    }

    pass maskEmissive //8 - sign mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MaskEmissive();
    }

    pass PS_StrongBloom     //9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_StrongBloom();
    }

    pass PS_Compulsory_BlendDecal // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Compulsory_DECAL();
    }


    pass DynamicSmallSign // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DYNAMIC_SMALL();
    }


    //PS_DynamicBloom
    pass DynaicBloomPass // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DynamicBloom();
    }

    pass DynaicFastPass // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DynamicFast();
    }

    pass GlassColorPass // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_GLASSCOLOR();
    }
   
    pass LightDepth // - construction , Construction의 render light depth에서 변경해주기
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
        GeometryShader = compile gs_5_0 GS_MAIN_LIGHTDEPTH();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

    pass FarRender //14 - construction , Construction의 render light depth에서 변경해주기
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FAR();
    }

}