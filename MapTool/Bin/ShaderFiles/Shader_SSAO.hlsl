/* SSAO */
#include "Engine_Shader_Defines.hlsli"
#include "Shader_Deferred_Header.hlsli"

float g_fRadiuse = { 0.003f };
float4 g_SSAORandoms[64];
Texture2D g_SSAONoisesTexture;

float g_fSSAOBise = { 0.025f };
const float2 g_NoiseScale = float2(1280.f / 4.f, 720.f / 4.f);

float3x3 Get_TBN(float3 vNormal, float2 vTexcoord)
{
    float3 vRandomVec = g_SSAONoisesTexture.Sample(PointSampler, vTexcoord * g_NoiseScale).xyz;
    vRandomVec = vRandomVec * 2.f - 1.f;
    matrix matWV = mul(g_WorldMatrix, g_CamViewMatrix);
    vRandomVec = normalize(mul(vector(vRandomVec, 0.f), matWV)).xyz;
    
    float3 tangent = normalize(vRandomVec - vNormal * dot(vRandomVec, vNormal));
    float3 bitangent = cross(vNormal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, vNormal);
    
    return TBN;
}

float4 SSAO(float3x3 TBN, float3 vPosition)
{
    float fOcclusion = 0.f;
    
    for (int i = 0; i < 64; ++i)
    {
        float3 vSample = vPosition + mul(g_SSAORandoms[i % 64].xyz, TBN) * g_fRadiuse; // 뷰스페이스
       
        vector vOffset = vector(vSample, 1.f);
        vOffset = mul(vOffset, g_CamProjMatrix);
        vOffset.xyz /= vOffset.w;
        vOffset.xy = vOffset.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
        
        vector vOccNorm = g_DepthTexture.Sample(LinearSampler, vOffset.xy);
        
        	/* 뷰스페이스 상의 위치를 구한다. */
        vector vOccPosition;
        vOccPosition.x = vOffset.x * 2.f - 1.f;
        vOccPosition.y = vOffset.y * -2.f + 1.f;
        vOccPosition.z = vOccNorm.x; /* 0 ~ 1 */
        vOccPosition.w = 1.f;
        
        vOccPosition = vOccPosition * (vOccNorm.y * g_fFar);
        
        vOccPosition = mul(vOccPosition, g_ProjMatrixInv);
        
        //float rangeCheck = smoothstep(0.0, 1.0, g_fRadiuse / abs(vPosition.z - vOccPosition.z));
        fOcclusion += (vOccPosition.z >= vSample.z + g_fSSAOBise ? 1.0 : 0.0);
    }
    
    float4 vAmbient = fOcclusion / 64.f;
    
    return vAmbient;
}

PS_OUT PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    
    if (vNormalDesc.a != 0.f)
    {
        Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    }
    else
    {
        //뷰스페이스 위치로 옮기기
        vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
        vNormal = normalize(mul(vNormal, g_CamViewMatrix));
        
        //뷰행렬 상의 위치 구하기
        vector vPosition;
        
        vPosition.x = In.vTexcoord.x * 2.f - 1.f;
        vPosition.y = In.vTexcoord.y * -2.f + 1.f;
        vPosition.z = vDepthDesc.x; /* 0 ~ 1 */
        vPosition.w = 1.f;

        vPosition = vPosition * (vDepthDesc.y * g_fFar);
        vPosition = mul(vPosition, g_ProjMatrixInv);
        
        float4 fAmbient = SSAO(Get_TBN(vNormal.xyz, In.vTexcoord), vPosition.xyz);

        Out.vColor = 1.f - fAmbient;
    }
    
    return Out;
}