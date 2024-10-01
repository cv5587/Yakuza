/* Header */
/* 상수 테이블과 struct 모음*/

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_WorldMatrixInv, g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;

matrix g_CamProjMatrix, g_CamViewMatrix;

//그림자용
matrix g_ViewMatrixArray[3];
matrix g_ProjMatrixArray[3];

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;
vector g_vCamDir;
float g_fFar;

bool g_isSSAO;
bool g_isPBR;
bool g_isBOF;
bool g_isShadow;

float g_fTimeDelta;

Texture2D g_Texture;
Texture2DArray g_TextureArray;

Texture2D g_PriorityTexture;
Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_MultiDiffuseTexture;
Texture2D g_ShadeTexture;
Texture2D g_RMTexture;
Texture2D g_RSTexture;
Texture2D g_SurfaceTexture;
Texture2D g_SubSurfaceTexture;
Texture2D g_OEShaderTexture;
Texture2D g_LightMapTexture;
Texture2D g_SpecularTexture;
Texture2D g_DepthTexture;
Texture2D g_OESpecularTexture;

Texture2D g_BackBufferTexture;
Texture2D g_ToneMappingTexture;
Texture2D g_LuminanceTexture;
Texture2D g_CopyLuminanceTexture;
Texture2D g_AmbientTexture;
Texture2D g_BackBlurTexture;
Texture2D g_NonLightNonBlurTexture;

//그림자
Texture2DArray g_LightDepthTextureArray : register(t0);
bool g_isArray;
float4 g_CasecadesZ[3];

//블러용
Texture2D g_EffectTexture;
//데칼용
Texture2D g_DecalTexture;

Texture2D g_BlurTexture;
Texture2D g_ResultTexture;
Texture2D g_AccumTexture;
Texture2D g_AccumAlpha;
Texture2D g_Distortion;

Texture2D g_GlassDiffuseTexture;
Texture2D g_GlassNormalTexture;
Texture2D g_GlassDepthTexture;
Texture2D g_GlassRMTexture;
Texture2D g_GlassMetallicTexture;
Texture2D g_GlassRSTexture;

//HDR
float3 Luminance = float3(0.2125f, 0.7154f, 0.0721f);
float fDelta = { 0.0001f };
bool g_isFinished = { false };
float g_fLumVar = { 1.f };
float A = { 2.51f }, B = { 0.03f }, C = { 2.43f }, D = { 0.59f }, E = { 0.14f };

/*블룸(가우시안) */
float g_fTexW = 1280.0f;
float g_fTexH = 720.0f;

float3 g_vStartPos;
float2 g_fAnimTime;
float2 g_fControlAlpha;
bool g_isUVAnim;
float2 g_StartUV;
float2 g_EndUV;
float4 g_vEndColor;
float2 g_AnimScale;


static const float g_fWeight[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
};

float g_fTotal = 2.f;

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

struct VS_OUT_REVERSE
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vPositionReverse : TEXCOORD1;
};


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN_REVERSE
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vPositionReverse : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

struct PS_OUT_REVERSE
{
    vector vColor : SV_TARGET0;
    vector vReverseColor : SV_Target1;
};

struct PS_RIM_OUT
{
    vector vColor : SV_TARGET0;
    vector vAlpha : SV_TARGET1;
};