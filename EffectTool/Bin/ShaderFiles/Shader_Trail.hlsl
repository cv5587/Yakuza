#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;

float g_fProgress;


Texture2D g_AccumColor;
Texture2D g_AccumAlpha;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix  matWVP;

    matWVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
    vector vAlpha : SV_TARGET1;
    vector vDistortion : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    vector vParticle = g_Texture.Sample(PointSampler, In.vTexcoord);

    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;
    
    if(AlphaN<0.1f)
    {
        AlphaN = 0.f;
    }

    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultTrailPass
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AuraTrailPass
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}