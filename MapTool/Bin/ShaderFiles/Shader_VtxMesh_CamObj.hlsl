
#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
float   g_fObjID;
bool g_bWriteID;

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

/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


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
    //vector vNormal : SV_TARGET1;
    //vector vDepth : SV_TARGET2;
    //vector vSpecular : SV_TARGET3;
};



PS_OUT PS_MAIN_CAMERAEYE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    // 투명할 경우(0.1보다 작으면 투명하니) 그리지 않음
    //if (Out.vDiffuse.a < 0.1f)
    //    discard;
    Out.vDiffuse = float4(1, 0, 0, 1);
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, g_fObjID, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_CAMERAFOCUS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    // 투명할 경우(0.1보다 작으면 투명하니) 그리지 않음
    //if (Out.vDiffuse.a < 0.1f)
    //    discard;
    Out.vDiffuse = float4(0, 0, 1, 1);
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, g_fObjID, 1.f);
    
    return Out;
}



technique11 DefaultTechnique
{
    pass CameraEyePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CAMERAEYE();
    }

    pass CameraFocusPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CAMERAFOCUS();
    }

}

