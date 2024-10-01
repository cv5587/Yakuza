/* PBR */
#include "Shader_SSAO.hlsl"

float fGamma = { 2.2f };
float PI = { 3.14f };

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


float3 GammaToLinear(float3 color)
{
    return pow(color, fGamma);
}

float3 LinearToGamma(float3 color)
{
    return pow(color, 1.0f / fGamma);
}

//F
float3 FresnelSchlick(float cosTheta, float3 R0)
{
    return R0 + (1.f - R0) * pow(1.f - cosTheta, 5.f);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

//D
float NormalDistributionGGXTR(float3 vNormal, float3 h, float a)
{
    float a2 = a * a;
    float NdotH = saturate(dot(vNormal, h));
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = NdotH2 * (a2 - 1.f) + 1.f;
    denom = PI * denom * denom;
    
    return nom / denom;
}

float3 SchlickApprox(float3 F0, float3 L, float3 H)
{
    float dotLH = saturate(dot(L, H));
    return F0 + (1.0f - F0) * pow(1.0f - dotLH, 5.0f);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = 3.14159265359f * denom * denom;

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

float epsilon = 1e-6f;

float4 BRDF(float4 vPosition, float2 vTexcoord, float4 vNormal, float4 vDepthDesc, float3 vLook)
{
    float3 vAlbedo = g_DiffuseTexture.Sample(LinearSampler, vTexcoord).xyz;
    vAlbedo = GammaToLinear(vAlbedo);
    
    vector Combine = g_SurfaceTexture.Sample(PointSampler, vTexcoord);
    
    /* Li */
    float vDistance = length(normalize(g_vLightPos - vPosition));
    float vAttenuation = 1.f / (vDistance * vDistance);
    float3 vLi = g_vLightDiffuse * vAttenuation;
    
    float3 F0 = 0.4;
    
    float fMetalic = Combine.r;
    //F0 = lerp(F0, vAlbedo, fMetalic);
    
    //vector vLightDir = reflect(normalize(g_vLightDir), vNormal); //g_vLightDir * -1.f;
    float3 vLightDir = reflect(normalize(g_vLightDir), vNormal).xyz;
    float3 vHalfway = normalize(vLook + vLightDir);
    float3 vRadiance = g_vLightDiffuse;
    
    //BRDF
    float vRoughness = Combine.g;
    float D = NormalDistributionGGXTR(vNormal.xyz, vHalfway, vRoughness); //r : Roughness
    float G = GeometrySmith(vNormal.xyz, vLook, vLightDir, vRoughness); 
    float3 F = SchlickApprox(F0, vLightDir, vHalfway);
    
    float3 nominator = D * G * F;
    
    float WoDotN = max(dot(vLook, vNormal.xyz), 0.f);
    float WiDotN = saturate(dot(vLightDir, vNormal.xyz));
    float denominator = (WoDotN * WiDotN * 4.f) + 0.001f;
    
    float3 vSpecular = (nominator / (denominator)) * Combine.z;
    
    float3 vKS = F;
    float3 vKD = 1.f - vKS;
    vKD *= 1.f - fMetalic;
    
    float3 vDiffuse = vKS * vAlbedo / PI;
    vDiffuse = LinearToGamma((vDiffuse + vSpecular) * WiDotN);
    return vector(vDiffuse, 1.f);
}

float3 CalcuateSpecular(float2 vTexcoord, float4 vNormal, float3 vLook)
{
    vector vOEShader = g_OEShaderTexture.Sample(LinearSampler, vTexcoord);
    vector vSpecular = g_OESpecularTexture.Sample(LinearSampler, vTexcoord);
    
    float3 reflectDir = reflect(-normalize(g_vLightDir), vNormal);
    float spec = pow(max(dot(reflectDir, vLook), 0.0), vOEShader.x); // Ω∫∆Â≈ß∑Ø ∞ËªÍ
    return spec * vSpecular.r; // Ω∫∆Â≈ß∑Ø ∞≠µµ π›»Ø
}