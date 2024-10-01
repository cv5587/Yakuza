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
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);
    
    return Out;
}

struct VS_OUT_LIGHTDEPTH
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

//그림자용
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
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vSurface : SV_TARGET3; // vector(metallic, goughness, speculer, 0.f)
    vector vOEShader : SV_Target4;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    UV_SHADER UV = (UV_SHADER) 0;
    if (g_isUVShader)
        UV = UV_Shader(In.vTexcoord);
    
    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    vector vMulti = g_isMulti ? g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = g_isRD ? g_RDTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = g_isRS ? g_RSTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = g_isRM ? g_RMTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = g_isRT ? g_RTTexture.Sample(LinearSampler, g_isUVShader ? UV.RT : In.vTexcoord) : vector(0.5f, 0.5f, 1.f, 0.5f);
    //노말 벡터 구하기
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuse = DiffusePortion(vDiffuseDesc, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuse = Get_Diffuse(vMulti.a, vDiffuse); // Diffuse 최종
    
    //Tangent Normal 구하기 
    vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);

    vNormalDesc = vNormalDesc * 2.f - 1.f;
    //vNormalDesc = vector(vNormalDesc.w, vNormalDesc.y, 1.f, 0.f);
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vector vNormalBTN = normalize(vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f));
    //vector vNormalBTN = (vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f));
    
    float RimIndex = 0.f;
    float RimAlpha = 0.f;
    if (0.05f < g_isRimLight)
    {
        if(g_isTop)
        {
            if (In.vTexcoord.y >= g_fRimUV.x && In.vTexcoord.y < g_fRimUV.y)
            {
                RimIndex = g_isRimLight;

                float Range = g_fRimUV.y - g_fRimUV.x;
                float FullRange = g_fRimUV.x + 0.75f * Range;
    
                if (In.vTexcoord.y <= FullRange)
                {
                    RimAlpha = 1.0f; // 75% 구간까지는 1로 유지
                }
                else
                {
                    float RemainingRange = g_fRimUV.y - FullRange;
                    float NewDistance = In.vTexcoord.y - FullRange;
        
                    RimAlpha = 1.0f - (NewDistance / RemainingRange);
                    RimAlpha = saturate(RimAlpha); 
                }
            }
            else if (In.vTexcoord.y >= g_fRimArmUV.x && In.vTexcoord.y < g_fRimArmUV.y)
            {
                RimIndex = g_isRimLight;

                float Range = g_fRimArmUV.y - g_fRimArmUV.x;
                float FullRange = g_fRimArmUV.x + 0.75f * Range;
    
                if (In.vTexcoord.y <= FullRange)
                {
                    RimAlpha = 1.0f; // 75% 구간까지는 1로 유지
                }
                else
                {
                    float RemainingRange = g_fRimArmUV.y - FullRange;
                    float NewDistance = In.vTexcoord.y - FullRange;
        
                    RimAlpha = 1.0f - (NewDistance / RemainingRange);
                    RimAlpha = saturate(RimAlpha); 
                }
            }
        }
        
      
    }
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, RimIndex, RimAlpha);
    Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    Out.vDiffuse = vDiffuse;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, Result.fFactor);
    Out.vOEShader = vector(OEResult.fRouhness, OEResult.vSpecular);
    
    return Out;
}

PS_OUT PS_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    UV_SHADER UV = (UV_SHADER) 0;
    if (g_isUVShader)
        UV = UV_Shader(In.vTexcoord);
    
    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    vector vMulti = g_isMulti ? g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord) : vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = g_isRD ? g_RDTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = g_isRS ? g_RSTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = g_isRM ? g_RMTexture.Sample(LinearSampler, g_isUVShader ? UV.RDRMRS : In.vTexcoord) : vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = g_isRT ? g_RTTexture.Sample(LinearSampler, g_isUVShader ? UV.RT : In.vTexcoord) : vector(0.5f, 0.5f, 1.f, 0.5f);
    //노말 벡터 구하기
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuseColor = DiffusePortion(vDiffuseDesc, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuseColor = Get_Diffuse(vMulti.a, vDiffuseColor); // Diffuse 최종
    vDiffuseColor.a = vDiffuseDesc.a;
    
    //Tangent Normal 구하기 
    vNormalDesc = Get_Normal(vNormalDesc, vRT, fFactor);
    vNormalDesc = vNormalDesc * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vector vNormalBTN = vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f);

    float RimIndex = 0.f;
    if (0.05f < g_isRimLight)
    {
        if (In.vTexcoord.y >= g_fRimUV.x && In.vTexcoord.y < g_fRimUV.y)
        {
            RimIndex = g_isRimLight;
        }
    }
    
    OE_SPECULAR OEResult = Neo_OE_Specular(vMulti, vRM, vRS);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, RimIndex, 0.f);
    Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    Out.vDiffuse = vDiffuseColor;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, Result.fFactor);
    Out.vOEShader = vector(OEResult.fRouhness, OEResult.vSpecular);
    
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
    pass DefaultPass
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

    pass BlendPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND();
    }

    pass LightDepth
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