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


struct PS_MAIN_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vSurface : SV_TARGET3; // vector(metallic, goughness, speculer, 0.f)
    vector vOEShader : SV_Target4;
    vector vSpecular : SV_Target5;
};

struct PS_MAIN_COLOR_OUT
{
    vector vDiffuse : SV_TARGET0;
};



PS_MAIN_OUT PS_MAIN(PS_IN In)
{
    PS_MAIN_OUT Out = (PS_MAIN_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;
   
    vector vMulti = g_isMulti ? g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = g_isRD ? g_RDTexture.Sample(LinearSampler, In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = g_isRS ? g_RSTexture.Sample(LinearSampler, In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = g_isRM ? g_RMTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = vector(0.5f, 0.5f, 1.f, 0.5f);
    
    //노말 벡터 구하기
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vDiffuse = DiffusePortion(vDiffuse, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuse = Get_Diffuse(vMulti.a, vDiffuse); // Diffuse 최종
    
    //Tangent Normal 구하기 
    vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);
    
    vNormalDesc = vNormalDesc * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vector vNormalBTN = vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f);
    Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);

    float RimIndex = 0.f;
    if (0.05f < g_isRimLight)
    {
        if (In.vTexcoord.y >= g_fRimUV.x && In.vTexcoord.y < g_fRimUV.y)
        {
            RimIndex = g_isRimLight;
        }
    }
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    float fMixMultiFactor = lerp(vMulti.y, 1.f, AssetShader);
    float fDeffuseFactor = vDiffuse.a * 1.f;
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, RimIndex, 0.f);
    Out.vDiffuse = vDiffuse;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, 0);
    Out.vOEShader = vector(OEResult.fRouhness, OEResult.fMixShaderFactor, fMixMultiFactor, fDeffuseFactor);
    Out.vSpecular = vector(OEResult.vSpecular, 0.f);
    
    return Out;

}


PS_MAIN_COLOR_OUT PS_MAIN_Bright(PS_IN In)
{
    PS_MAIN_COLOR_OUT Out = (PS_MAIN_COLOR_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    float4 albedo = float4(1, 1, 1, 1); // 흰색 알베도
    
    
    if (true == g_bBright)
    {
        float minBrightness = 0;
        float maxBrightness = 0.5;
    
        float brightness = 0;
        
        if(1 < g_fBrightTime)
        {
            float fTime = g_fBrightTime - 1;
            brightness = lerp(maxBrightness, minBrightness, fTime);
        }
        else
        {
            brightness = lerp(minBrightness, maxBrightness, g_fBrightTime);
        }

        // 완전히 흰색으로 깜빡이기 위한 색상 혼합
       
        vDiffuse.a = 0;
        vDiffuse = lerp(vDiffuse, albedo, brightness); // 밝기를 알베도 색상과 혼합
        Out.vDiffuse = vDiffuse;
    }

    Out.vDiffuse = vDiffuse;
    
    return Out;
}


PS_MAIN_COLOR_OUT PS_MAIN_Dissolve(PS_IN In)
{
    PS_MAIN_COLOR_OUT Out = (PS_MAIN_COLOR_OUT) 0;

    // 원래 텍스처 색상
    float4 color = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    // 노이즈 텍스처의 값 (0.0에서 1.0 사이)
    float noiseValue = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord).r;

    // 테두리 계산
        //noiseValue > g_fDissolveTime - g_fEdgeWidth && 
    if (noiseValue < g_fDissolveTime)
    {
        // 테두리 외부 부분 제거
        discard;
    }

    
    Out.vDiffuse = color;

    //if (Out.vDiffuse.a < 0.1f)
    //    discard;

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


    pass DefaultBrightPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Bright();
    }


    pass DissolvePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Dissolve();
    }
    

    pass LightDepth //2 - construction , Construction의 render light depth에서 변경해주기
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


}