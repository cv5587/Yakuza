/* Header */
/* 상수 테이블과 struct 모음*/

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_WorldMatrixInv, g_ViewMatrixInv, g_ProjMatrixInv;

Texture2D g_Texture;
Texture2D g_ToneTexture;
Texture2D g_UVAnimTexture;
Texture2D g_BaseAlphaTexture;
Texture2D g_FluidTexture;
Texture2D g_ResultTexture;

Texture2D g_DiffuseTexture;
Texture2D g_DepthTexture;
Texture2D g_NormalTexture;
Texture2D g_DissolveTexture;
Texture2D g_MultiDiffuseTexture;
Texture2D g_RSTexture;
Texture2D g_RMTexture;
Texture2D g_RDTexture;

float g_fProgress;

float4 g_vColor;

float3 g_vLifeTime;
float3 g_vStartPos;

float2 g_fAnimTime;
float2 g_fControlAlpha;

vector g_vCamPosition;

/* Aura */
float2 g_lifeAlpha;
float g_iSpriteIndex;
float2 g_fUVCount;
float g_fCurTime;

float g_NearZ = 0.01f;
float g_FarZ = 3000.f;

/* Trail */
Texture2D g_AccumColor;
Texture2D g_AccumAlpha;

/* Point */
vector g_vStartColor, g_vEndColor;
float g_fRadian;

/* AnimMesh */
matrix g_ViewMatrixArray[3], g_ProjMatrixArray[3];
matrix g_BoneMatrices[512];

float g_fFar = { 3000.f };
float g_fTimeDelta;

float g_fOut = { 0.5f };

bool g_isRS;
bool g_isRD;

float g_fTexcoordX = { 0.f };
float g_fTexcoordY = { 0.f };

//림라이트
float g_isRimLight;
float2 g_fRimUV;
/* Decal */
float2 g_RenderResolution = float2(1280, 720);


/* Mesh */
matrix g_ReflectViewMatrix;

Texture2D g_RefractionTexture;
Texture2D g_NoiseTexture;
Texture2D g_BlurReverseTexture;
Texture2D g_ReflectionTexture;

float g_fObjID;
float g_fRefractionScale = { 0.001f };

float g_fSpeed = 2.f;
bool g_bExistNormalTex;
bool g_bExistRMTex;
bool g_bExistRSTex;
bool g_bReflExist;


int g_iCount = { 0 };
const int g_iMaxCasCade = { 3 };
