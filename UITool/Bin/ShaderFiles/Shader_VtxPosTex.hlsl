#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;

float g_fProgress;

float4 g_vColor;

float3 g_vLifeTime;

float3 g_vStartPos;
float2 g_fAnimTime;
float2 g_fControlAlpha;


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

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT VS_ANIM(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);

    //float4 WorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    //float3 StartPos = g_vStartPos;
    //float factor =g_fAnimTime.x / g_fAnimTime.y;
    
    //WorldPos.xyz = lerp(StartPos, WorldPos.xyz, float3(factor, factor, factor));
    //WorldPos.w = 1.f;
    //Out.vPosition = mul(WorldPos, matVP);
    //Out.vTexcoord = In.vTexcoord;

    matrix World=g_WorldMatrix;
    
    float3 EndPos = World._41_42_43;
    float3 StartPos = EndPos.xyz+g_vStartPos.xyz;
    float factor = g_fAnimTime.x / g_fAnimTime.y;
    
    World._41_42_43 = lerp(StartPos, EndPos, float3(factor, factor, factor));
    
    float4 WorldPos = mul(float4(In.vPosition, 1.f), World);
    
    Out.vPosition = mul(WorldPos, matVP);
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
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_ALPHABLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    vector BaseColor= g_Texture.Sample(PointSampler, In.vTexcoord);
    
    //BaseColor.rgb = BaseColor.rgb * BaseColor.a;
    Out.vColor = BaseColor;
    
    return Out;
}

PS_OUT PS_COLOR_ALPHABLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    vector vMergeColor = g_vColor;
    
   vector FinalColor = vMergeColor * BaseColor;

    Out.vColor = FinalColor;
    
    return Out;
}

PS_OUT PS_COLOR_ALPHABLEND_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    vector BaseAlpha = 0.f;
    vector MergeColor = g_vColor;
    vector FinalColor = vector(0.f, 0.f, 0.f, 0.f);
    
    if (g_vLifeTime.x < g_vLifeTime.y || g_vLifeTime.x > g_vLifeTime.z)
        discard;
    else
    {
        FinalColor = BaseColor * MergeColor;
        float MiddleTime =(g_vLifeTime.z - g_vLifeTime.y) / 2.f;
        float CurrentTime = (g_vLifeTime.x - g_vLifeTime.y);
        
        if (MiddleTime > CurrentTime)
        {
            BaseAlpha = CurrentTime / MiddleTime;
            FinalColor.a *= BaseAlpha;
        }
        else
        {
            BaseAlpha = (CurrentTime - MiddleTime) / MiddleTime;
            FinalColor.a *= 1.f-BaseAlpha;
        }
    }
    
    Out.vColor = FinalColor;
    
    
    return Out;
}

PS_OUT PS_COLOR_ALPHABLEND_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    vector MergeColor = g_vColor;
    BaseColor *= MergeColor;
    
    float factor = g_fAnimTime.x / g_fAnimTime.y;
    
    float2 ControlAlpha = g_fControlAlpha;
    
    BaseColor.a *= lerp(ControlAlpha.x, ControlAlpha.y, factor);//20240706
    
    Out.vColor = BaseColor;
    
    return Out;
}

PS_OUT PS_ALPHABLEND_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    float factor = g_fAnimTime.x / g_fAnimTime.y;
    
    float2 ControlAlpha = g_fControlAlpha;
    
    BaseColor.a *= lerp(ControlAlpha.x, ControlAlpha.y, factor); //20240706
    
    Out.vColor = BaseColor;
    
    return Out;
}


PS_OUT PS_BACKBUFFER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    
    return Out;
}

technique11 DefaultTechnique
{

    pass BackBuffer //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BACKBUFFER();
    }

    pass AlphaBlend_Texture//1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ALPHABLEND();
    }


    pass Color_AlphaBlend_Texture//2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_ALPHABLEND();
    }

    pass Color_AlphaBlend_Effect_Texture //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_ALPHABLEND_EFFECT();
    }

    pass AlphaBlend_Anim_Texture //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ANIM();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ALPHABLEND_ANIM();
    }

    pass Color_AlphaBlend_Anim_Texture //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ANIM();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_ALPHABLEND_ANIM();
    }


}