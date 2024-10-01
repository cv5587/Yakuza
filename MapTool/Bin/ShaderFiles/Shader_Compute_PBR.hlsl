
cbuffer PBR_Buffer : register(b0)
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix ViewMatrixInv;
    matrix ProjMatrixInv;
    matrix CamViewMatrix;
    matrix CamProjMatrix;
    
    float4 vCamPosition;
    float fFar;
    float fPadding[3];
};

cbuffer Light_Buffer : register(b1)
{
    vector vLightDirection;

    vector vLightDiffuse;
    vector vLightAmbient;
    vector vLightSpecular;
};

Texture2D g_DepthTexture : register(t0);
Texture2D g_NormalTexture : register(t1);
Texture2D g_SurfaceTexture : register(t2);
Texture2D g_DiffuseTexture : register(t3);
Texture2D g_OETexture : register(t4);

RWTexture2D<float4> g_OutputTexture : register(u0);
RWTexture2D<float4> g_OutSpecularTexture : register(u1);

struct MRM_OUT
{
    float fRed;
    float fBlue;
};

struct COMBINE_OUT
{
    float fMetalness;
    float fRoughness;
};

float3 GammaToLinear(float3 color, float fGamma)
{
    return pow(color, fGamma);
}

float3 LinearToGamma(float3 color, float fGamma)
{
    return pow(color, 1.0f / fGamma);
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}


//F
float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness, float PI)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.f);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 CalculateSpecularTint(float3 F, float3 albedo, float specularTint)
{
    float3 vSpecularColor = float3(1.f, 1.f, 1.f);
    float3 tintedSpecular = lerp(vSpecularColor, vSpecularColor * albedo, specularTint);

    return F * tintedSpecular;
}

float4 BRDF(float4 vPosition, int2 vTexcoord, float4 vNormal, float4 vDepthDesc, float3 vLook)
{
    float PI = { 3.14159265359f };
    float fGamma = { 2.2f };
    
    float3 vAlbedo = g_DiffuseTexture.Load(int3(vTexcoord, 0)).xyz;
    vector Combine = g_SurfaceTexture.Load(int3(vTexcoord, 0));
    
    /* Li */
    float fMetalic = Combine.r;
    float3 F0 = max(0.04f, Combine.b);
    F0 = lerp(F0, vAlbedo, fMetalic);
   
    
    //vector vLightDir = reflect(normalize(g_vLightDir), vNormal); //g_vLightDir * -1.f;
    float3 vLightDir = normalize(vLightDirection * -1.f);
    float3 vHalfway = normalize(vLook + vLightDir);
    float3 vRadiance = vLightDiffuse;
    
    float WoDotN = max(dot(vNormal.xyz, vLook), 0.f);
    float WiDotN = max(dot(vNormal.xyz, vLightDir), 0.f);
    float denominator = (WoDotN * WiDotN * 4.f) + 0.001f;
   
    //BRDF
    float vRoughness = Combine.g;
    float D = DistributionGGX(vNormal.xyz, vHalfway, vRoughness, PI);
    float G = GeometrySmith(vNormal.xyz, vLook, vLightDir, vRoughness);
    float cosTheta = max(dot(vNormal.xyz, vHalfway), 0.f);
    float3 F = FresnelSchlickRoughness(cosTheta, F0, vRoughness);
    
    float3 specularWithTint = CalculateSpecularTint(F, vAlbedo, Combine.w);
    
    float3 nominator = D * G * specularWithTint;
        
    float3 vSpecular = (nominator / denominator);
        
    float3 vKS = F;
    float3 vKD = 1.f - vKS;
    vKD *= 1.f - fMetalic;
    
    // ¡÷∫Ø±§ √ﬂ∞°
    float3 ambientComponent = vLightAmbient.xyz * vAlbedo;
    
    float3 vDiffuse = vKD * (vAlbedo / PI);
    vDiffuse = (vDiffuse + vSpecular) * (vRadiance * WiDotN) + ambientComponent;
        
    return vector(vDiffuse, 1.f);
}

float3 CookTorranceBRDF(float3 vNormal, float3 vLook, float roughness, float3 F0, float3 vLight)
{
    float PI = { 3.14159265359f };
    
    float3 vHalfway = normalize(vLook + vLight);
    
    float NDF = DistributionGGX(vNormal, vHalfway, roughness, PI); // GGX Normal Distribution Function
    float G = GeometrySmith(vNormal, vLook, vLight, roughness); // Geometry function
    float3 F = FresnelSchlick(max(dot(vHalfway, vLook), 0.0), F0); // Fresnel Schlick

    float3 nominator = NDF * G * F;
    float NdotL = max(dot(vNormal, vLight), 0.0);
    float denominator = 4.0 * max(dot(vNormal, vLook), 0.0) * NdotL + 0.001f; // Avoid division by 0
    float3 vSpecular = (nominator / denominator);
    
    return vSpecular * NdotL;
}

float3 OESpecular(float4 vPosition, int2 vTexcoord, float3 vNormal, float3 vLook)
{
    float3 vLight = vLightDirection.xyz * -1.f;
    float4 vOEShader = g_OETexture.Load(int3(vTexcoord, 0));
    float3 vSpecular = vOEShader.yzw;
    float3 vHalfway = normalize(vLook + vLight);
    
    float3 F0 = vSpecular;
    float3 SpecularColor = CookTorranceBRDF(vNormal, vLook, vOEShader.x, F0, vLight);
    
    return SpecularColor;
}

[numthreads(16, 16, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    int2 vUV = id.xy;
    float2 vTexcoord = vUV / float2(1280.f, 720.f);
    
    vector vNormal = g_NormalTexture.Load(int3(vUV, 0));
    vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);
    vector vDepthDesc = g_DepthTexture.Load(int3(vUV, 0));
    vector vWorldPos;

    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * fFar);
    vWorldPos = mul(vWorldPos, ProjMatrixInv);
    vWorldPos = mul(vWorldPos, ViewMatrixInv);
    
    float3 vLook = normalize(vCamPosition - vWorldPos).xyz;
    
    vector vResult = BRDF(vWorldPos, vUV, normalize(vNormal), vDepthDesc, vLook);
    float3 vSpecular = OESpecular(vWorldPos, vUV, normalize(vNormal).xyz, vLook);
    
    g_OutputTexture[vUV] = vResult;
    g_OutSpecularTexture[vUV] = vector(vSpecular, 0.f);
}