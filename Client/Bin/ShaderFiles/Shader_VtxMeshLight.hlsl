#include "Engine_Shader_Defines.hlsli"
#include "Shader_Client_Defines.hlsli"


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



PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_LightMask_Alpha(PS_IN In)
{
    // 모양대로 자르는거
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_LightMask(PS_IN In)
{
    // 모양대로 자르는거
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuse.r < 0.4f)
        discard;
  
    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_Lamp(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector emissive = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
   
    vector finalColor = saturate(vDiffuse + (emissive.r) * float4(1, 0.5, 0, 1));
    
    if(finalColor.r == 1 && finalColor.g == 1 && finalColor.b == 1)
        finalColor = float4(1, 0.5, 0, 1);
  
    Out.vDiffuse = finalColor;
    
    return Out;
}


PS_OUT PS_MAIN_AlphaLamp(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vDiffuse.a = 0.2;
    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_EMISSIVE_ALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    // 약한발광(0.1 - 0.3)
    // 보통(0.4 - 0.6)
    // 강한 (0.7-0.8)
    vector emissiveColor = vDiffuse * 0.1;
   
    vector finalColor = vDiffuse + emissiveColor;
  
    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector emissiveColor = vDiffuse * 0.3;
    
    vector finalColor = float4(0, 0, 0, 0);
    
    finalColor = vDiffuse + emissiveColor;
    
     
    
    Out.vDiffuse = finalColor;
    
    return Out;
}

PS_OUT PS_MAIN_Bloom(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector emissive = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
    
    // 약한발광(0.1 - 0.3)
    // 보통(0.4 - 0.6)
    // 강한 (0.7-0.8)
    //vector emissiveColor = vDiffuse * 0.3;
    //vector finalColor = float4(0, 0, 0, 0);
    //if (vDiffuse.r < 0.5 && vDiffuse.g < 0.5 && vDiffuse.b < 0.5)
    //{
    //    finalColor = vDiffuse * 0.3;
    //}
    //else
    //{
    //    finalColor = vDiffuse + emissiveColor;
    //}
    
   
    // 약한발광(0.1 - 0.3)
    // 보통(0.4 - 0.6)
    // 강한 (0.7-0.8)
    vector emissiveColor = vDiffuse * 0.4;
   

    float4 finalColor;
    
    if (vDiffuse.r < 0.5 && vDiffuse.g < 0.5 && vDiffuse.b < 0.5)
    {
        finalColor = vDiffuse;
    }
    else
    {
        finalColor = vDiffuse + emissiveColor;
    }

    Out.vDiffuse = finalColor;
    
    return Out;
}



technique11 DefaultTechnique
{
    pass DefaultPass //0 - 형광등
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

    pass LightMaskAlphaPass //1 - 형광등 자르기(자르기+빛 알파적용)
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

    pass LightMaskPass //2 - 형광등 자르기(자르기+빛 알파적용x)
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LightMask();
    }

    pass LampPass //3 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Screen, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Lamp();
    }

    pass AlphaPass //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_AlphaLamp();
    }


    pass EmissiveAlphaPass //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE_ALPHA();
    }

    pass EmissivePass //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }


    pass DefaultBloomPass //7 - rm 적용한 외부간판
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Bloom();
    }


}

