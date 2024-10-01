#include "Shader_Neo.hlsli"

/*
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fProgress;
float2 g_lifeAlpha;
float g_iSpriteIndex;
float2 g_fUVCount;
float g_fCurTime;

Texture2D g_Texture;
Texture2D g_DiffuseTexture;


float g_fFar = 3000.f;
*/
bool g_isAttach;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
        //m_pInstance
    row_major matrix TransformMatrix : WORLD;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float2 vRectSize : COLOR2;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
};

struct VS_NONBLENDER_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;

    float2 vLifeTime : COLOR0;
    float4 vProjPos : COLOR1;
    float4 vNormal : NORMAL;
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP, matVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    if (g_isAttach)
    {
        vector Right = In.TransformMatrix._11_12_13_14 * In.vRectSize.x;
        vector Up = In.TransformMatrix._21_22_23_24 * In.vRectSize.x;
        vector Look = In.TransformMatrix._31_32_33_34 * In.vRectSize.x;
        vector Pos = In.TransformMatrix._41_42_43_44;
        matrix Transform = matrix(Right, Up, Look, Pos);
    
    
        vector vPosition = mul(float4(In.vPosition, 1.f), Transform); //로컬이동.
    
        Out.vPosition = mul(vPosition, matWVP);

        Out.vTexcoord = In.vTexcoord;
        Out.vLifeTime = In.vLifeTime;
    }
    else
    {
        vector Right = In.TransformMatrix._11_12_13_14 * In.vRectSize.x;
        vector Up = In.TransformMatrix._21_22_23_24 * In.vRectSize.x;
        vector Look = In.TransformMatrix._31_32_33_34 * In.vRectSize.x;
        vector Pos = In.TransformMatrix._41_42_43_44;
        matrix Transform = matrix(Right, Up, Look, Pos);
    
    
        vector vPosition = mul(float4(In.vPosition, 1.f), Transform); //로컬이동.
    
        Out.vPosition = mul(vPosition, matVP);

        Out.vTexcoord = In.vTexcoord;
        Out.vLifeTime = In.vLifeTime;
    }



    return Out;
}

VS_NONBLENDER_OUT VS_NONBLENDER(VS_IN In)
{
    VS_NONBLENDER_OUT Out = (VS_NONBLENDER_OUT) 0;

    matrix matWV, matWVP, matVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    
    if (g_isAttach)
    {
        vector Right = In.TransformMatrix._11_12_13_14 * In.vRectSize.x;
        vector Up = In.TransformMatrix._21_22_23_24 * In.vRectSize.x;
        vector Look = In.TransformMatrix._31_32_33_34 * In.vRectSize.x;
        vector Pos = In.TransformMatrix._41_42_43_44;
        matrix Transform = matrix(Right, Up, Look, Pos);
    

        vector vPosition = mul(float4(In.vPosition, 1.f), Transform); //로컬이동.
    
        Out.vPosition = mul(vPosition, matWVP);
        Out.vTexcoord = In.vTexcoord;
        Out.vLifeTime = In.vLifeTime;
        Out.vProjPos = Out.vPosition;
        Out.vNormal = Look;
    }
    else
    {
        vector Right = In.TransformMatrix._11_12_13_14 * In.vRectSize.x;
        vector Up = In.TransformMatrix._21_22_23_24 * In.vRectSize.x;
        vector Look = In.TransformMatrix._31_32_33_34 * In.vRectSize.x;
        vector Pos = In.TransformMatrix._41_42_43_44;
        matrix Transform = matrix(Right, Up, Look, Pos);
    
    
        vector vPosition = mul(float4(In.vPosition, 1.f), Transform); //로컬이동.
    
        Out.vPosition = mul(vPosition, matVP);
        Out.vTexcoord = In.vTexcoord;
        
        Out.vLifeTime = In.vLifeTime;
        Out.vProjPos = Out.vPosition;
        Out.vNormal = Look;
    }
   


    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
};

struct PS_NOBLENDER_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
    float4 vProjPos : COLOR1;
    float4 vNormal : NORMAL;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vAlpha : SV_TARGET1;
    vector vDistortion : SV_TARGET2;
};

struct PS_NOBLENDER_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vSurface : SV_TARGET3; // vector(metallic, goughness, speculer, 0.f)
    vector vOEShader : SV_Target4;
    vector vSpecular : SV_Target5;
};


//지오메트리 어차피 그리는 순서나 픽셀이나 똑같다
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //프로젝션 좌표

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;
        
    AlphaN *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x);
    
    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;
}

PS_NOBLENDER_OUT PS_NONBLEDNER(PS_NOBLENDER_IN In)
{
    PS_NOBLENDER_OUT Out = (PS_NOBLENDER_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    vector vMulti = vector(0.f, 1.f, 0.f, 1.f);
    vector vRD = vector(1.f, 1.f, 1.f, 1.f);
    vector vRS = vector(1.f, 1.f, 1.f, 1.f);
    vector vRM = vector(0.5f, 1.f, 0.5f, 1.f);
    vector vRT = vector(0.5f, 0.5f, 1.f, 0.5f);
    
    //노말 벡터 구하기
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    //Neo Shader
    float fFactor = RepeatingPatternBlendFactor(vMulti);
    vector vDiffuseColor = DiffusePortion(vParticle, vRS, vRD, fFactor, In.vTexcoord);
    
    COMBINE_OUT Result = Neo_MetallicAndGlossiness(vMulti, vRM); // Metallic, Rouhness 최종
    vDiffuseColor = Get_Diffuse(vMulti.a, vDiffuseColor); // Diffuse 최종
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDiffuse = vDiffuseColor;
    Out.vSurface = vector(Result.fMetalness, Result.fRoughness, Result.fSpeclure, Result.fFactor);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass //0
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_NONBLENDER();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_NONBLEDNER();
    }

    pass NonBlenderPass //1
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}

