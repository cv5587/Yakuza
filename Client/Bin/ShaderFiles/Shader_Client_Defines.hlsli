/* Header */
/* 상수 테이블과 struct 모음*/

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_OldWorldMatrix, g_OldViewMatrix, g_OldProjMatrix;
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
Texture2D g_RTTexture;
Texture2D g_EmissiveTexture;

float g_fProgress;

float4 g_vColor;

float3 g_vLifeTime;
float3 g_vStartPos;
float2 g_AnimScale;

float2 g_fAnimTime;
float2 g_fControlAlpha;
bool g_isUVAnim;
float2 g_StartUV;
float2 g_EndUV;
vector g_vCamPosition;

float g_ExcludeSSAO; // 바닥 부분 SSAO 제외

/* Aura */
float2 g_lifeAlpha;
float g_iSpriteIndex;
float2 g_fUVCount;
float g_fCurTime;

float g_NearZ;
float g_FarZ;

/* Trail */
Texture2D g_AccumColor;
Texture2D g_AccumAlpha;

/* Point */
vector g_vStartColor, g_vEndColor;
float g_fRadian;

/* AnimMesh */
matrix g_ViewMatrixArray[3], g_ProjMatrixArray[3];
matrix g_BoneMatrices[512];

float g_fFar = { 200.f };
float g_fTimeDelta;

float g_fOut = { 0.5f };

float g_fTexcoordX = { 0.f };
float g_fTexcoordY = { 0.f };

//림라이트
float g_isRimLight;
float2 g_fRimArmUV;
float2 g_fRimUV;
bool g_isTop;
//디스토션
float g_fDistortionWeight;
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


/* Map */
bool g_bCompulsoryAlpha;
bool g_bEmissiveTex;
bool g_bLightCut; // 잘라낼건지
float g_fDynamicSpeed;

/* Item */
float g_fBrightTime;
bool g_bBright;
float g_fDissolveTime;
float g_fEdgeWidth = { 0.2f };


int g_iCount = { 0 };
const int g_iMaxCasCade = { 3 };

/*Neo 셰이더 옵션*/
bool g_isRS;
bool g_isRD;
bool g_isRM;
bool g_isRT;
bool g_isNormal;
bool g_isMulti;

float g_RDCount = { 1.f };

float Opacity = { 0.f };

float fSpecularPower = { 0.f };
vector vSpecularColor;

float AssetShader = { 0.f };
float DisableRDRT = { 0.f };
float Engine = { 0.f };
float IsOEClothShader = { 0.f };
float SkinShader = { 0.f };
float Rough = { 0.f };
float IsY3Shader = { 0.f };
float Imperfection = { 0.f };
float SPShader = { 0.f };

bool g_isUVShader = { false };
float g_fRTX = { 0.f };
float g_fRTY = { 0.f };
float g_fRDRMRS_X = { 0.f };
float g_fRDRMRS_Y = { 0.f };
float f_fImperfection_UV = { 0.f };
float f_fEngine_UV = { 0.f };