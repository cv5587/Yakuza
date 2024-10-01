#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;


Texture2D g_Texture;
Texture2D g_NormalTexture;
Texture2D g_ResultTexture;
Texture2D g_AccumColor;
Texture2D g_AccumAlpha;

vector g_vStartColor, g_vEndColor;
vector g_vCamPosition;
float2 g_lifeAlpha;
float g_fRadian;
float g_NearZ = 0.01f;
float g_fFar = 3000.f;
float g_FarZ = 3000.f;
float g_fDistortionWeight;
bool g_isAttach;
bool g_isNormal;

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

struct VS_NOBILL_OUT
{
    row_major matrix TransformMatrix : WORLD;
    float3 vPosition : POSITION;
    float2 vPSize : TEXCOORD0;
    float4 vDir : COLOR0;
    float2 vLifeTime : COLOR1;
    float2 vRectSize : COLOR2;
};
/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);//로컬이동.

    if (g_isAttach)
    {  
        Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;//월드상
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

VS_NOBILL_OUT VS_NOBILLBOARD(VS_IN In)
{
    VS_NOBILL_OUT Out = (VS_NOBILL_OUT) 0;

    if(g_isAttach)
    {
        vector vPosition = mul(mul(float4(In.vPosition, 1.f), In.TransformMatrix), g_WorldMatrix); //로컬이동.
    
        Out.TransformMatrix = mul(In.TransformMatrix, g_WorldMatrix);
        Out.vPosition = vPosition.xyz; //월드상
        Out.vPSize = In.vPSize;
        Out.vDir = normalize(In.vDir);
        Out.vLifeTime = In.vLifeTime;
        Out.vRectSize = In.vRectSize;
    }
    else
    {
        vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix); //로컬이동.
    
        Out.TransformMatrix = In.TransformMatrix;
        Out.vPosition = vPosition.xyz; //월드상
        Out.vPSize = In.vPSize;
        Out.vDir = normalize(In.vDir);
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

struct GS_NOBILL_IN
{
    row_major matrix TransformMatrix : WORLD;
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

    float2 vLifeTime : COLOR0;
};

struct GS_NOBIL_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;

    float2 vLifeTime : COLOR0;
    float4 vProjPos : COLOR1;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

//파티클의 점 하나를 그리고 픽셀로 넘어간다 사각형 한개 생성후 픽셸로 감
[maxvertexcount(6)]//방향성 x
void GS_DEAFULT(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
    }

    vector vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);//받아온건 월드.
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize.x* 0.5f;

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f);//월드좌표

        
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
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

[maxvertexcount(6)]//방향성o 기본 회전 o 지속 회전은넘겨주는 각도를 계속 증가시켜주면될듯
void GS_DIRSCALE(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
    }
	
    float3 vDirection = In[0].vDir.xyz;
    //방향에 회전을 준다.
   
    vector vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = normalize(cross(vDirection, vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //월드좌표
        //-1~1 x,y,z

    
        
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(0.f, 1.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(1.f, 0.f);
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
/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */
[maxvertexcount(6)] //방향성 x
void GS_DISTORTION(point GS_NOBILL_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
    }

    float3 vRight = In[0].TransformMatrix._11_12_13_14 * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = In[0].TransformMatrix._21_22_23_24 * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;
    vector vLook = In[0].TransformMatrix._31_32_33_34;

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //월드좌표

        
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;

    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;

    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;


    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
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
void GS_NOBILLBOARD(point GS_NOBILL_IN In[1], inout TriangleStream<GS_NOBIL_OUT> Triangles)
{
    GS_NOBIL_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
        Out[i].vProjPos = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vNormal = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTangent = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vBinormal = float4(0.f, 0.f, 0.f, 0.f);
    }

    float3 vRight = In[0].TransformMatrix._11_12_13_14 * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = In[0].TransformMatrix._21_22_23_24 * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;
    vector vLook = In[0].TransformMatrix._31_32_33_34;  

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //월드좌표

        
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vProjPos = Out[0].vPosition;
    Out[0].vNormal = vLook;
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vProjPos = Out[1].vPosition;
    Out[1].vNormal = vLook;
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vProjPos = Out[2].vPosition;
    Out[2].vNormal = vLook;

    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vProjPos = Out[3].vPosition;
    Out[3].vNormal = vLook;
    
    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}

[maxvertexcount(6)] //방향성 x
void GS_FALL(point GS_NOBILL_IN In[1], inout TriangleStream<GS_NOBIL_OUT> Triangles)
{
    GS_NOBIL_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
        Out[i].vProjPos = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vNormal = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTangent = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vBinormal = float4(0.f, 0.f, 0.f, 0.f);
    }

    float3 vDirection = In[0].vDir.xyz;
    //방향에 회전을 준다.
   
    vector vLook = normalize(g_vCamPosition - vector(In[0].vPosition, 1.f));
    float3 vRight = normalize(cross(vDirection, vLook.xyz)) * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;

    float3 vPosition;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //월드좌표

        
    vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vProjPos = Out[0].vPosition;
    Out[0].vNormal = vLook;
    Out[0].vTangent =  vector(vRight, 0.f);
    Out[0].vBinormal = vector(vUp, 0.f);
    
    vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vProjPos = Out[1].vPosition;
    Out[1].vNormal = vLook;
    Out[1].vTangent = vector(vRight, 0.f);
    Out[1].vBinormal = vector(vUp, 0.f);
    
    
    vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vProjPos = Out[2].vPosition;
    Out[2].vNormal = vLook;
    Out[2].vTangent = vector(vRight, 0.f);
    Out[2].vBinormal = vector(vUp, 0.f);
    
    vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vProjPos = Out[3].vPosition;
    Out[3].vNormal = vLook;
    Out[3].vTangent = vector(vRight, 0.f);
    Out[3].vBinormal = vector(vUp, 0.f);
    
    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}
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


[maxvertexcount(6)] //방향성o 기본 회전 o 지속 회전은넘겨주는 각도를 계속 증가시켜주면될듯
void GS_ROTSCALE(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = float4(0.f, 0.f, 0.f, 0.f);
        Out[i].vTexcoord = float2(0.f, 0.f);
        Out[i].vLifeTime = float2(0.f, 0.f);
    }
	
    float3 vDirection = In[0].vDir.xyz;

    float3 vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
    float3 vRight = (cross(vDirection, vLook.xyz));
    float3 vUp = (cross(vLook.xyz, vRight));

    
   float LookLength= length(vLook);
    
    matrix rotate = RotationMatrix(vLook,g_fRadian);
    
    vLook = normalize(mul(vector(vLook, 0.f), rotate)).xyz;
    vRight = normalize(mul(vector(vRight, 0.f), rotate)).xyz * In[0].vPSize.x * In[0].vRectSize.x * 0.5f;
    vUp = normalize(mul(vector(vUp, 0.f), rotate)).xyz * In[0].vPSize.y * In[0].vRectSize.x * 0.5f;
    

    float4 vPosition;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float4 PointPosition = float4(In[0].vPosition, 1.f); //월드좌표
        //-1~1 x,y,z

    
    //로컬상 사각형 점 만들기.
    vPosition = float4((In[0].vPosition + vRight + vUp), 1.f);
    Out[0].vPosition = mul(vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    vPosition = float4((In[0].vPosition - vRight + vUp) , 1.f);
    Out[1].vPosition = mul(vPosition, matVP);
    Out[1].vTexcoord = float2(0.f, 1.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    vPosition = float4((In[0].vPosition - vRight - vUp) , 1.f);
    Out[2].vPosition = mul(vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    vPosition = float4((In[0].vPosition + vRight - vUp) , 1.f);
    Out[3].vPosition = mul(vPosition, matVP);
    Out[3].vTexcoord = float2(1.f, 0.f);
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
struct PS_NOBIL_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
    float4 vProjPos : COLOR1;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_NOBILL_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vRS : SV_Target4;
    vector vMulti : SV_Target5;
    vector vRD : SV_Target6;
};

//지오메트리 어차피 그리는 순서나 픽셀이나 똑같다
PS_NOBILL_OUT PS_NOBILLBOARD_NOCOLOR(PS_NOBIL_IN In)
{
    PS_NOBILL_OUT Out = (PS_NOBILL_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalmap = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vParticle;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    
    if (g_isNormal)
        {   
            float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
           vector vNormalBTN = vector(mul(vNormalmap.xyz, WorldMatrix), 0.f);
    
           Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
        }
    else
    {
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    
    return Out;
}

PS_NOBILL_OUT PS_FALL(PS_NOBIL_IN In)
{
    PS_NOBILL_OUT Out = (PS_NOBILL_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표
    
    float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    
    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vParticle *= LerpColor;
    vector vNormalmap = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    float factor = lerp(0.1f, 3.f, In.vLifeTime.y / In.vLifeTime.x);

    vParticle.a *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x);
    vParticle.a=pow(vParticle.a, factor);
    
    Out.vDiffuse = vParticle;

    if (vParticle.a < 0.1f)
        discard;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
   
    if (g_isNormal)
    {
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vector vNormalBTN = vector(mul(vNormalmap.xyz, WorldMatrix), 0.f);
    
        Out.vNormal = vector(vNormalBTN.xyz * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    
    
    
    return Out;
}



PS_OUT PS_MAIN_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    
    vParticle *= LerpColor;
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;

    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;
}

PS_OUT PS_ROTANIM_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    
    vParticle *= LerpColor;
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;

    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;

}

PS_OUT PS_ANIM_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 PointPosition = In.vPosition; //월드좌표

    float BaseAlpha = g_Texture.Sample(LinearSampler, In.vTexcoord).g;

    vector vParticle = vector(1.f, 1.f, 1.f, BaseAlpha);
    
    float4 LerpColor = lerp(g_vStartColor, g_vEndColor, In.vLifeTime.y / In.vLifeTime.x);
    
    vParticle.rgba *= LerpColor;
    
    float3 ColorN = vParticle.rgb;
    
    float AlphaN = vParticle.a;

    float fWeight = abs(PointPosition.z); //정규화된 z 값을 가져옴(0~1)    

    Out.vColor = float4(ColorN.rgb * AlphaN, AlphaN) * fWeight;
    
    Out.vAlpha = float4(AlphaN, AlphaN, AlphaN, AlphaN);
    Out.vDistortion = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return Out;

}

PS_OUT PS_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vParticle = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    vParticle.g *= lerp(g_lifeAlpha.x, g_lifeAlpha.y, In.vLifeTime.y / In.vLifeTime.x); 
    
    Out.vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    Out.vAlpha = float4(0.0f, 0.0f, 0.0f, 0.0f);
   
    Out.vDistortion = float4(0.0f, 0.0f, g_fDistortionWeight, vParticle.g);
    
    return Out;

}

struct RIM_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vRS : SV_Target4;
    vector vMulti : SV_Target5;
    vector vRD : SV_Target6;
};

RIM_OUT PS_RIM_DEPTH(PS_IN In)
{
    RIM_OUT Out = (RIM_OUT) 0;
    Out.vDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
    Out.vDepth = vector(0.f, 0.f, 0.f, 0.f);
    Out.vRM = vector(0.f, 0.f, 0.f, 0.f);
    Out.vRS = vector(0.f, 0.f, 0.f, 0.f);//스펙영향이 초기화 값을 영향 받을수도.?
    
    Out.vMulti = vector(0.f, 0.f, 0.f, 0.f);
    Out.vRD = vector(0.f, 0.f, 0.f, 0.f);
    
    float BaseAlpha = g_Texture.Sample(LinearSampler, In.vTexcoord).a;
    Out.vDepth = float4(0.f, 0.f, 0.f, BaseAlpha);
        
    return Out;
}

technique11 DefaultTechnique
{
	
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
     pass NOCOLOR    //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DIRSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOCOLOR();
    }

    pass COLOR //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DIRSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }
    
    pass ROTATE//2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_ROTSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }

    pass ROTANIM//3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_ROTSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ANIM_COLOR();
    }
    pass ANIM //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_WeightsBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DIRSCALE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ANIM_COLOR();

    }

    pass RimDepth //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DEAFULT();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RIM_DEPTH();
    }

    pass NO_BILL_Distortion //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_NOBILLBOARD();
        GeometryShader = compile gs_5_0 GS_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass NoBillboard //7
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0  VS_NOBILLBOARD();
        GeometryShader = compile gs_5_0 GS_NOBILLBOARD();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_NOBILLBOARD_NOCOLOR();
    }

    pass Distortion //8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DEAFULT();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass FallSpread //9
    {
        SetRasterizerState(RS_Cull_NON_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_NOBILLBOARD();
        GeometryShader = compile gs_5_0 GS_FALL();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FALL();
    }

}

