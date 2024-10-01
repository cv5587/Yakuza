#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vCamPosition;

Texture2D g_ToneTexture;
Texture2D g_UVAnimTexture;
Texture2D g_BaseAlphaTexture;
Texture2D g_FluidTexture;

float g_fProgress;
float2 g_lifeAlpha;
float g_iSpriteIndex;
float2 g_fUVCount;
float2 g_fToneUVCount;
float g_fCurTime;
float g_fFlowPow;
float g_fFlowSpeed;

float g_NearZ = 0.01f;
float g_FarZ = 3000.f;

float4 g_vStartColor;
float4 g_vEndColor;

bool g_isAttach;
//vLifeTime.x 종료시간 /vLifeTime.y 현재시간 / vLifeTime.z 누적인덱스
struct VS_IN
{
    //m_pVB
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    //m_pInstance
    row_major matrix TransformMatrix : WORLD;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float2 vRectSize : COLOR2;
};

struct VS_OUT
{
    float3 vPosition : POSITION;
    float2 vPSize : TEXCOORD0;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float2 vRectSize : COLOR2;
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */

VS_OUT VS_LOCAL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

    Out.vPosition = vPosition.xyz;
    Out.vPSize = In.vPSize;
    Out.vDir = In.vDir;
    Out.vLifeTime = In.vLifeTime;
    Out.vRectSize = In.vRectSize;

    return Out;
}

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix); //로컬이동.
   
    if(g_isAttach)
    {
        Out.vPosition = mul(vPosition, g_WorldMatrix).xyz; //월드상
        Out.vPSize = In.vPSize;
        Out.vDir = normalize(mul(In.vDir, g_WorldMatrix));
        Out.vLifeTime = In.vLifeTime;
        Out.vRectSize = In.vRectSize;
    }
    else
    {
        Out.vPosition = vPosition.xyz;
        Out.vPSize = In.vPSize;
        Out.vDir = In.vDir;
        Out.vLifeTime = In.vLifeTime;
        Out.vRectSize = In.vRectSize;
    }


    return Out;
}


struct GS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : TEXCOORD0;

    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float2 vRectSize : COLOR2;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vAlphaTex : TEXCOORD1;
    float2 vToneTex : TEXCOORD2;
    
    float2 vLifeTime : COLOR0;

};

float4x4 RotationMatrix(float3 axis, float angle)
{
    float c, s;
    sincos(angle, s, c);

    float3 a = normalize(axis);
    float3x3 rotation =
    {
        a.x * a.x * (1 - c) + c, a.x * a.y * (1 - c) - a.z * s, a.x * a.z * (1 - c) + a.y * s,
        a.y * a.x * (1 - c) + a.z * s, a.y * a.y * (1 - c) + c, a.y * a.z * (1 - c) - a.x * s,
        a.z * a.x * (1 - c) - a.y * s, a.z * a.y * (1 - c) + a.x * s, a.z * a.z * (1 - c) + c
    };

    return float4x4(
        rotation[0], 0,
        rotation[1], 0,
        rotation[2], 0,
        float3(0, 0, 0), 1
    );
}

//파티클의 점 하나를 그리고 픽셀로 넘어간다 사각형 한개 생성후 픽셸로 감
[maxvertexcount(6)] //방향성 x
void GS_DEAFULT(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4] = (GS_OUT[4]) 0;
	    
    float3 vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;

    matrix rotate = RotationMatrix(vLook, In[0].vRectSize.y);
    
    vLook = normalize(mul(vector(vLook, 0.f), rotate)).xyz;
    vRight = mul(vector(vRight, 0.f), rotate).xyz;
    vUp = mul(vector(vUp, 0.f), rotate).xyz;
    
    
    
    
    
    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //월드좌표
        //-1~1 x,y,z

    
    float2 uvWeight = float2(1 / g_fUVCount.x, 1 / g_fUVCount.y);
    
    float SpriteIndex = lerp(0.f , (g_fUVCount.x* g_fUVCount.y) , (In[0].vLifeTime.y / In[0].vLifeTime.x));
    
    float2 uv = float2(float(floor(SpriteIndex) % g_fUVCount.x) / g_fUVCount.x, floor(SpriteIndex / g_fUVCount.x) / g_fUVCount.y);

    
    float2 uvToneWeight = float2(1 / g_fToneUVCount.x, 1 / g_fToneUVCount.y);
    
    float ToneSpriteIndex = lerp(0.f, (g_fToneUVCount.x * g_fToneUVCount.y) , (In[0].vLifeTime.y / In[0].vLifeTime.x));
    
    float2 Toneuv = float2(float(floor(ToneSpriteIndex) % g_fToneUVCount.x) / g_fToneUVCount.x, floor(ToneSpriteIndex / g_fToneUVCount.x) / g_fToneUVCount.y);

    
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = uv;
    Out[0].vToneTex = Toneuv;
    Out[0].vAlphaTex = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(uv.x + uvWeight.x,uv.y);
    Out[1].vToneTex = float2(Toneuv.x + uvToneWeight.x, Toneuv.y);
    Out[1].vAlphaTex = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(uv.x + uvWeight.x, uv.y + uvWeight.y);
    Out[2].vToneTex = float2(Toneuv.x + uvToneWeight.x, Toneuv.y + uvToneWeight.y);
    Out[2].vAlphaTex = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(uv.x, uv.y + uvWeight.y);
    Out[3].vToneTex = float2(Toneuv.x, Toneuv.y + uvToneWeight.y);
    Out[3].vAlphaTex = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;

    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}

//파티클의 점 하나를 그리고 픽셀로 넘어간다 사각형 한개 생성후 픽셸로 감
[maxvertexcount(6)] //방향성 x
void GS_NOROTATE(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4] = (GS_OUT[4]) 0;
    
	    
    float3 vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;
    
    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //월드좌표
        //-1~1 x,y,z

    
    float2 uvWeight = float2(1 / g_fUVCount.x, 1 / g_fUVCount.y);
    
    float SpriteIndex = lerp(0.f, (g_fUVCount.x * g_fUVCount.y), (In[0].vLifeTime.y / In[0].vLifeTime.x));
    
    float2 uv = float2(float(floor(SpriteIndex) % g_fUVCount.x) / g_fUVCount.x, floor(SpriteIndex / g_fUVCount.x) / g_fUVCount.y);

    
    float2 uvToneWeight = float2(1 / g_fToneUVCount.x, 1 / g_fToneUVCount.y);
    
    float ToneSpriteIndex = lerp(0.f, (g_fToneUVCount.x * g_fToneUVCount.y), (In[0].vLifeTime.y / In[0].vLifeTime.x));
    
    float2 Toneuv = float2(float(floor(ToneSpriteIndex) % g_fToneUVCount.x) / g_fToneUVCount.x, floor(ToneSpriteIndex / g_fToneUVCount.x) / g_fToneUVCount.y);

    
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = uv;
    Out[0].vToneTex = Toneuv;
    Out[0].vAlphaTex = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(uv.x + uvWeight.x, uv.y);
    Out[1].vToneTex = float2(Toneuv.x + uvToneWeight.x, Toneuv.y);
    Out[1].vAlphaTex = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(uv.x + uvWeight.x, uv.y + uvWeight.y);
    Out[2].vToneTex = float2(Toneuv.x + uvToneWeight.x, Toneuv.y + uvToneWeight.y);
    Out[2].vAlphaTex = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(uv.x, uv.y + uvWeight.y);
    Out[3].vToneTex = float2(Toneuv.x, Toneuv.y + uvToneWeight.y);
    Out[3].vAlphaTex = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;

    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vAlphaTex : TEXCOORD1;
    float2 vToneTex : TEXCOORD2;
    float2 vLifeTime : COLOR0;

};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vAlpha : SV_TARGET1;
    vector vDistortion : SV_TARGET2;
};

//지오메트리 어차피 그리는 순서나 픽셀이나 똑같다
PS_OUT PS_MAIN_NOCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    
    
    float2 LifeAlpha = g_lifeAlpha;

    vector Tone = g_ToneTexture.Sample(PointSampler, In.vToneTex);

    float FlowPow = g_fFlowPow;//왜곡 강도 0~1사이
    float FlowSpeed = g_fFlowSpeed; //흐름 속도
    
    //스프라이트 는 시간이랑 다름
    float TimeA = frac(In.vLifeTime.y * FlowSpeed);
    float TimeB = frac(In.vLifeTime.y + 0.5f * FlowSpeed);
    
    float2 ResultTime = float2(TimeA * 0.2f, TimeB * 0.3f);

    // Noise 텍스처를 사용하여 UV 좌표 변화 계산
    
    float2 Dist = (g_FluidTexture.Sample(PointSampler, In.vAlphaTex) * 2.0f - 1.0f)*FlowPow;
   // float2 flowUV = In.vAlphaTex + (g_FluidTexture.Sample(PointSampler, In.vAlphaTex + ResultTime).xy * 2.0 - 1.0) * FlowPow;


    float BaseAlphaA = g_BaseAlphaTexture.Sample(PointSampler, In.vTexcoord + Dist * TimeA).a;
    float BaseAlphaB = g_BaseAlphaTexture.Sample(PointSampler, In.vTexcoord + Dist * TimeB).a;

    float mixlerp = abs(frac(In.vLifeTime.y) * 2.f - 1.f);
    
    float BaseAlpha = lerp(BaseAlphaA, BaseAlphaB, mixlerp);//flow 셰이더 반복
    
    vector UVSprite = g_UVAnimTexture.Sample(PointSampler, In.vTexcoord);
    
    float Alphafactor = frac(In.vLifeTime.y / In.vLifeTime.x);
    
    float lerpAlpha = lerp(LifeAlpha.x, LifeAlpha.y, Alphafactor);
  
    vector FinalColor = Tone;
    FinalColor.a *= lerpAlpha * BaseAlpha;

    // FinalColor = vector(flowUV, flowUV);

    float3 ColorN = FinalColor.rgb;
    
    float AlphaN = FinalColor.a;
        
  //  AlphaN *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x);
    
    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;
}

//지오메트리 어차피 그리는 순서나 픽셀이나 똑같다
PS_OUT PS_MAIN_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    
    
    float2 LifeAlpha = g_lifeAlpha;

    //vector Tone = g_ToneTexture.Sample(PointSampler, In.vToneTex);
        float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    float FlowPow = g_fFlowPow; //왜곡 강도 0~1사이
    float FlowSpeed = g_fFlowSpeed; //흐름 속도
    
    //스프라이트 는 시간이랑 다름
    float TimeA = frac(In.vLifeTime.y * FlowSpeed);
    float TimeB = frac(In.vLifeTime.y + 0.5f * FlowSpeed);
    
    float2 ResultTime = float2(TimeA * 0.2f, TimeB * 0.3f);

    // Noise 텍스처를 사용하여 UV 좌표 변화 계산
    
    float2 Dist = (g_FluidTexture.Sample(PointSampler, In.vAlphaTex) * 2.0f - 1.0f) * FlowPow;
    
    vector BaseColorA = g_BaseAlphaTexture.Sample(LinearSampler, In.vTexcoord + Dist * TimeA);
    vector BaseColorB = g_BaseAlphaTexture.Sample(LinearSampler, In.vTexcoord + Dist * TimeB);

    float mixlerp = abs(frac(In.vLifeTime.y) * 2.f - 1.f);
    
    vector BaseColor = lerp(BaseColorA, BaseColorB, mixlerp); //flow 셰이더 반복
    
    vector UVSprite = g_UVAnimTexture.Sample(LinearSampler, In.vTexcoord);
    
    float Alphafactor = frac(In.vLifeTime.y / In.vLifeTime.x);
    
    float lerpAlpha = lerp(LifeAlpha.x, LifeAlpha.y, Alphafactor);
    
   
    
    vector FinalColor = BaseColor * LerpColor;
    FinalColor.a *= lerpAlpha;

    float3 ColorN = FinalColor.rgb;
    
    float AlphaN = FinalColor.a;
        
  //  AlphaN *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x);
    
    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;
}
//지오메트리 어차피 그리는 순서나 픽셀이나 똑같다
PS_OUT PS_AURA_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    
    
    float2 LifeAlpha = g_lifeAlpha;

    vector Tone = g_ToneTexture.Sample(PointSampler, In.vToneTex);

    float FlowPow = g_fFlowPow; //왜곡 강도 0~1사이
    float FlowSpeed = g_fFlowSpeed; //흐름 속도
    
    //스프라이트 는 시간이랑 다름
    float TimeA = frac(In.vLifeTime.y * FlowSpeed);
    float TimeB = frac(In.vLifeTime.y + 0.5f * FlowSpeed);
    
    float2 ResultTime = float2(TimeA * 0.2f, TimeB * 0.3f);

    // Noise 텍스처를 사용하여 UV 좌표 변화 계산
    
    float2 Dist = (g_FluidTexture.Sample(PointSampler, In.vAlphaTex) * 2.0f - 1.0f) * FlowPow;
   // float2 flowUV = In.vAlphaTex + (g_FluidTexture.Sample(PointSampler, In.vAlphaTex + ResultTime).xy * 2.0 - 1.0) * FlowPow;


    float BaseAlphaA = g_BaseAlphaTexture.Sample(PointSampler, In.vTexcoord + Dist * TimeA).a;
    float BaseAlphaB = g_BaseAlphaTexture.Sample(PointSampler, In.vTexcoord + Dist * TimeB).a;

    float mixlerp = abs(frac(In.vLifeTime.y) * 2.f - 1.f);
    
    float BaseAlpha = lerp(BaseAlphaA, BaseAlphaB, mixlerp); //flow 셰이더 반복
    
    vector UVSprite = g_UVAnimTexture.Sample(PointSampler, In.vTexcoord);
    
    float Alphafactor = frac(In.vLifeTime.y / In.vLifeTime.x);
    
    float lerpAlpha = lerp(LifeAlpha.x, LifeAlpha.y, Alphafactor);
  
    vector FinalColor;
    FinalColor.rgb= Tone.rgb;
    FinalColor.a = lerpAlpha * BaseAlpha;

    // FinalColor = vector(flowUV, flowUV);

    float3 ColorN = FinalColor.rgb;
    
    float AlphaN = FinalColor.a;
        
  //  AlphaN *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x);
    
    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;
}


technique11 DefaultTechnique
{
    pass Fluid //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DEAFULT();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOCOLOR();
    }

    pass StartAuraFluid //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DEAFULT();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOCOLOR();
    }

    pass StartAuraFire //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NOROTATE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }

    pass AuraFire //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NOROTATE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_AURA_FIRE();
    }
}

