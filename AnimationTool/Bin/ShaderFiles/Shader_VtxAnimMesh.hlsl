#include "Engine_Shader_Defines.hlsli"
#include "Shader_Client_Defines.hlsli"

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHTS;
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

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix TransformMatrix = mul(g_BoneMatrices[In.vBlendIndices.x], In.vBlendWeights.x) +
    mul(g_BoneMatrices[In.vBlendIndices.y], In.vBlendWeights.y) +
    mul(g_BoneMatrices[In.vBlendIndices.z], In.vBlendWeights.z) +
    mul(g_BoneMatrices[In.vBlendIndices.w], fWeightW);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), TransformMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f); //바이
    
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

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix TransformMatrix = mul(g_BoneMatrices[In.vBlendIndices.x], In.vBlendWeights.x) +
    mul(g_BoneMatrices[In.vBlendIndices.y], In.vBlendWeights.y) +
    mul(g_BoneMatrices[In.vBlendIndices.z], In.vBlendWeights.z) +
    mul(g_BoneMatrices[In.vBlendIndices.w], fWeightW);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    Out.vPosition = mul(vPosition, g_WorldMatrix);
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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMultiDiffuce = g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    //노말 벡터 구하기
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vNormalDesc = vNormalDesc * 2.f - 1.f;
    vNormalDesc = vector(vNormalDesc.w, vNormalDesc.y, 1.f, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vector vNormalBTN = vector(mul(vNormalDesc.xyz, WorldMatrix), 0.f);

    /*
    float3 vLocalNormal = In.vNormal.xyz; //이건 옵젝 노멀임[법선]

    
    vector vWorldTangent = normalize(vector(vLocalTangent.xyz, 0.f));
    vector vWorldNormal = normalize(vector(vLocalNormal.xyz, 0.f));
    
    vector vWorlBinormal = vector(cross(vWorldNormal.xyz, vWorldTangent.xyz), 0.f);

    
  //  float3x3 WorldMatrix = float3x3(vWorlBinormal.xyz, vWorldNormal.xyz, vWorldTangent.xyz);
    float3x3 WorldMatrix = float3x3(vWorldTangent.xyz, vWorlBinormal.xyz, vWorldNormal.xyz);
    float3 vFinalNormal = mul(vLocalNormal.xyz, WorldMatrix);
    
    vFinalNormal = mul(vector(vWorldNormal.xyz, 0.f), g_WorldMatrix);
*/
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    //RS + RD
    vector vRSRD;
    vector vRDDesc;
    if (g_isRD)
    {
        vRDDesc = g_RDTexture.Sample(LinearSampler, In.vTexcoord * 50.f);
        Out.vRD = vRDDesc;
    }
    
    if (g_isRS)
    {
        vector vRSDesc = g_RSTexture.Sample(LinearSampler, In.vTexcoord * 20.f);
        Out.vRS = vRSDesc;
        vRSDesc = lerp(vRSDesc, vRDDesc, 0.7f);
        Out.vDiffuse = lerp(vDiffuse, vRSDesc, vMultiDiffuce.z);
        
    }
    else
    {
        if (g_isRD)
            Out.vDiffuse = lerp(vRDDesc, vDiffuse, vMultiDiffuce.z);
        else
            Out.vDiffuse = vDiffuse;
    }
    

    float RimIndex = 0.f;
    if (0.05f < g_isRimLight)
    {
        if (In.vTexcoord.y >= g_fRimUV.x && In.vTexcoord.y < g_fRimUV.y)
        {
            RimIndex = g_isRimLight;
        }
    }


    Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, RimIndex, 0.f);
    
    Out.vMulti = vMultiDiffuce;
    
    return Out;
}

PS_OUT PS_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMultiDiffuce = g_MultiDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    vDiffuse.xyz *= vDiffuse.a;
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    //RS + RD
    vector vRSRD;
    vector vRDDesc;
    if (g_isRD)
    {
        vRDDesc = g_RDTexture.Sample(LinearSampler, In.vTexcoord * 50.f);
        Out.vRD = vRDDesc;
    }
    
    if (g_isRS)
    {
        vector vRSDesc = g_RSTexture.Sample(LinearSampler, In.vTexcoord * 20.f);
        Out.vRS = vRSDesc;
        vRSDesc = lerp(vRSDesc, vRDDesc, 0.7f);
        Out.vDiffuse = lerp(vDiffuse, vRSDesc, vMultiDiffuce.z);
        
    }
    else
    {
        if (g_isRD)
            Out.vDiffuse = lerp(vDiffuse, vRDDesc, vMultiDiffuce.z);
        else
            Out.vDiffuse = vDiffuse;
    }


    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.f);
    Out.vMulti = vMultiDiffuce;
    

    return Out;
}

PS_OUT PS_MAGENTA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = float4(1, 0, 1, 1);
    
    //if (vDiffuse.a < 0.1f)
    //    discard;

    Out.vDiffuse = vDiffuse;
    //Out.vDiffuse = vector(1, 1, 1, 1);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 0.f);

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
	
    Out.vLightDepth = vector(In.vProjPos.w / 1000.f, 0.0f, 0.f, 0.f);

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

    pass MagentaPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAGENTA();
    }

    pass WireframePass
    {
        SetRasterizerState(RS_Wireframe_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}