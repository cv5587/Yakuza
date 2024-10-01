#include "Engine_Shader_Defines.hlsli"
#include "Shader_Client_Defines.hlsli"

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

    matrix World = g_WorldMatrix;
    
    float3 EndPos = World._41_42_43;
    float3 StartPos = EndPos.xyz + g_vStartPos.xyz;
    
    float XScale = g_AnimScale.x;
    float YScale = g_AnimScale.y;
    
    
    float factor = g_fAnimTime.x / g_fAnimTime.y;
    float XScalefactor = lerp(1.f, XScale, factor);
    float YScalefactor = lerp(1.f, YScale, factor);
    World._41_42_43 = lerp(StartPos, EndPos, float3(factor, factor, factor));
    World._11 *= XScalefactor;
    World._22 *= YScalefactor;
    
    
    float4 WorldPos = mul(float4(In.vPosition, 1.f), World);
    
    Out.vPosition = mul(WorldPos, matVP);
    Out.vTexcoord = In.vTexcoord;
    
    
    return Out;
}

VS_OUT VS_EFFECTANIM(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);

    matrix World = g_WorldMatrix;
    
    float3 EndPos = World._41_42_43;
    float3 StartPos = EndPos.xyz + g_vStartPos.xyz;
    float factor = saturate((g_vLifeTime.x - g_vLifeTime.y) / (g_vLifeTime.z - g_vLifeTime.y));
    
    float XScale = g_AnimScale.x;
    float YScale = g_AnimScale.y;
    
    float XScalefactor = lerp(1.f, XScale, factor);
    float YScalefactor = lerp(1.f, YScale, factor);
    World._41_42_43 = lerp(StartPos, EndPos, float3(factor, factor, factor));
    World._11 *= XScalefactor;
    World._22 *= YScalefactor;
    

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
    
    
    vector BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
   // float2 ControlAlpha = g_fControlAlpha;
    //BaseColor.rgb = BaseColor.rgb * BaseColor.a;
    //BaseColor.a *= ControlAlpha.x;
    Out.vColor = BaseColor;
    
    return Out;
}

PS_OUT PS_COLOR_ALPHABLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vMergeColor = g_vColor;
    
    vector FinalColor = vMergeColor * BaseColor;

    Out.vColor = FinalColor;
    
    return Out;
}

PS_OUT PS_COLOR_ALPHABLEND_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (g_vLifeTime.x < g_vLifeTime.y || g_vLifeTime.x > g_vLifeTime.z)
        discard;
    else
    {
      
        vector BaseColor = 0;
        
        if (g_isUVAnim)
        {
            float Move = frac(g_vLifeTime.x - g_vLifeTime.y);
            
            float TexX = frac(In.vTexcoord.x + Move);
            
            if (TexX > g_EndUV.x || TexX <= g_StartUV.x)
                TexX += g_StartUV.x + (1.f - g_EndUV.x);

                
            
            
            
            float2 Texcoord = float2(frac(TexX), In.vTexcoord.y);
            
            BaseColor = g_Texture.Sample(LinearSampler, Texcoord);
        }
        else
            BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
        vector BaseAlpha = 0.f;
        vector MergeColor = g_vColor;
        vector FinalColor = vector(0.f, 0.f, 0.f, 0.f);
        



        FinalColor = BaseColor * MergeColor;
        float MiddleTime = (g_vLifeTime.z - g_vLifeTime.y) / 2.f;
        float CurrentTime = (g_vLifeTime.x - g_vLifeTime.y);
            
        if (MiddleTime > CurrentTime)
        {
            BaseAlpha = saturate(CurrentTime / MiddleTime);
            FinalColor.a *= BaseAlpha;
        }
        else
        {
            BaseAlpha = saturate((CurrentTime - MiddleTime) / MiddleTime);
            FinalColor.a *= 1.f - BaseAlpha;
        }

        
        Out.vColor = FinalColor;
    }
    
    return Out;
}

PS_OUT PS_COLOR_REPEAT_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (g_vLifeTime.x < g_vLifeTime.y || g_vLifeTime.x > g_vLifeTime.z)
        discard;
    else
    {
      
        vector BaseColor = 0;
        
        if (g_isUVAnim)
        {
            float Move = frac(g_vLifeTime.x - g_vLifeTime.y);
            
            float TexX = frac(In.vTexcoord.x + Move);
            
            if (TexX > g_EndUV.x || TexX <= g_StartUV.x)
                TexX += g_StartUV.x + (1.f - g_EndUV.x);

                
            
            
            
            float2 Texcoord = float2(frac(TexX), In.vTexcoord.y);
            
            BaseColor = g_Texture.Sample(LinearSampler, Texcoord);
        }
        else
            BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
        vector BaseAlpha = 0.f;
        vector MergeColor = g_vColor;
        vector FinalColor = vector(0.f, 0.f, 0.f, 0.f);
        



        FinalColor = BaseColor * MergeColor;
        float MiddleTime = (g_vLifeTime.z - g_vLifeTime.y) / 2.f;
        float CurrentTime = (g_vLifeTime.x - g_vLifeTime.y);
            
        if (MiddleTime > CurrentTime)
        {
            BaseAlpha = saturate(CurrentTime / MiddleTime);
            FinalColor.a *= BaseAlpha;
        }
        else
        {
            BaseAlpha = saturate((CurrentTime - MiddleTime) / MiddleTime);
            FinalColor.a *= 1.f - BaseAlpha;
        }

        
        Out.vColor = FinalColor;
    }
    
    return Out;
}
PS_OUT PS_COLOR_ALPHABLEND_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    vector MergeColor = g_vColor;
    vector EndMergeColor = g_vEndColor;
    vector StartColor = BaseColor * MergeColor;
    vector EndColor = BaseColor * EndMergeColor;

    
    float factor = g_fAnimTime.x / g_fAnimTime.y;
    
    float2 ControlAlpha = g_fControlAlpha;
    
    vector FinColor = lerp(StartColor, EndColor, factor);

    Out.vColor = FinColor;
    
    return Out;
}


PS_OUT PS_COLOR_SCREEN_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    vector MergeColor = g_vColor;
    vector EndMergeColor = g_vEndColor;
    vector StartColor = BaseColor * MergeColor;
    vector EndColor = BaseColor * EndMergeColor;
    
    float factor = g_fAnimTime.x / g_fAnimTime.y;
    
    float2 ControlAlpha = g_fControlAlpha;
    
    float Alpha = lerp(ControlAlpha.x, ControlAlpha.y, factor);
    
    
    vector FinColor = lerp(StartColor, EndColor, factor);
    Out.vColor = vector(FinColor.rgb, FinColor.a * Alpha);
    
    return Out;
}

PS_OUT PS_COLOR_SCREEN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    vector MergeColor = g_vColor;
    vector FinColor = BaseColor * MergeColor;

    Out.vColor = FinColor;
    
    return Out;
}


PS_OUT PS_ALPHABLEND_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
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

PS_OUT PS_CIRCLE_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 Center = float2(0.5f, 0.5f);
    
    float2 CurrentCoord = In.vTexcoord;
    
    float2 Dir = normalize(CurrentCoord - Center);
    
    float Angle = atan2(-Dir.x, -Dir.y) / (2.0f * 3.141592f);
    
    if (Angle < 0.f)
        Angle += 1.0f; // 각도 범위를 0 ~ 1로 조정
    
    float factor = g_fAnimTime.x / g_fAnimTime.y;
    
    vector BaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Angle <= factor)
    {
        BaseColor = float4(0.f, 0.f, 0.f, 0.f); // 투명하게 처리하여 지우기
    }
    
    float2 ControlAlpha = g_fControlAlpha;
    
    float Alpha = lerp(ControlAlpha.x, ControlAlpha.y, factor);
    
    BaseColor = vector(BaseColor.rgb, BaseColor.a * Alpha);
    
    vector FinColor = BaseColor;

    Out.vColor = FinColor;
    
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

    pass AlphaBlend_Texture //1
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


    pass Color_AlphaBlend_Texture //2
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
        SetBlendState(BS_Screen, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_EFFECTANIM();
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
    pass Color_Screen_Anim_Texture //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Screen, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ANIM();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_SCREEN_ANIM();
    }
    pass Color_Screen_Texture //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Screen, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_SCREEN();
    }

    pass Color_Repeat_Effect_Texture //8 처음부터 끝까지 반복
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_EFFECTANIM();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_REPEAT_EFFECT();
    }

    pass Circle_Anim_Texture //9 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_EFFECTANIM();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_ANIM();
    }
}