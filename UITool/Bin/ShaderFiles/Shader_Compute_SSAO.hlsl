cbuffer SSAOBuffer : register(b0)
{
    float4 SSAORandoms[64];
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix ViewMatrixInv;
    matrix ProjMatrixInv;
    matrix CamViewMatrix;
    matrix CamProjMatrix;
    
    float fFar;
    float fRadiuse;
    float fSSAOBise;
    float buffer1;
}

Texture2D g_SSAONoiseTexture : register(t0);
Texture2D g_DepthTexture : register(t1);
Texture2D g_NormalTexture : register(t2);


RWTexture2D<float4> g_OutputTexture : register(u0);


float3x3 Get_TBN(float3 vNormal, float2 vTexcoord)
{
    float2 g_NoiseScale = float2(1280.f / 4.f, 720.f / 4.f);
    float2 scaledTexcoord = vTexcoord * g_NoiseScale;
    scaledTexcoord = scaledTexcoord * float2(1280.f, 720.f);
    float2 wrappedTexcoord = fmod(scaledTexcoord, 4.0f);
    int2 intWrappedTexcoord = int2(wrappedTexcoord);
    
    float3 vRandomVec = g_SSAONoiseTexture.Load(int3(intWrappedTexcoord, 0)).xyz;
    vRandomVec = vRandomVec * 2.f - 1.f;
    matrix matWV = mul(WorldMatrix, CamViewMatrix);
    vRandomVec = normalize(mul(vector(vRandomVec, 0.f), CamViewMatrix)).xyz;
    
    float3 tangent = normalize(vRandomVec - vNormal * dot(vRandomVec, vNormal));
    float3 bitangent = vector(normalize(cross(vNormal.xyz, tangent.xyz)), 0.f);
    float3x3 TBN = float3x3(tangent, bitangent, vNormal);
    
    return TBN;
}

float4 SSAO(float3x3 TBN, float3 vPosition)
{
    float fOcclusion = 0.f;
    
    for (int i = 0; i < 64; ++i)
    {
        float3 vSample = vPosition + mul(SSAORandoms[i].xyz, TBN) * fRadiuse; // 뷰스페이스
       
        vector vOffset = vector(vSample, 1.f);
        vOffset = mul(vOffset, CamProjMatrix);
        vOffset.xyz /= vOffset.w;
        vOffset.xy = vOffset.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
        int2 vIntOffset = int2(vOffset.xy * float2(1280.f, 720.f));
        
        vector vOccNorm = g_DepthTexture.Load(int3(vIntOffset.xy, 0));
        
        /* 뷰스페이스 상의 위치를 구한다. */
        vector vOccPosition;
        vOccPosition.x = vOffset.x * 2.f - 1.f;
        vOccPosition.y = vOffset.y * -2.f + 1.f;
        vOccPosition.z = vOccNorm.x; /* 0 ~ 1 */
        vOccPosition.w = 1.f;
        
        vOccPosition = vOccPosition * (vOccNorm.y * fFar);
        
        vOccPosition = mul(vOccPosition, ProjMatrixInv);
        
        fOcclusion += (vOccPosition.z >= vSample.z + fSSAOBise ? 1.0 : 0.0);
    }
    
    float4 vAmbient = fOcclusion / 64.f;
    
    return vAmbient;
}

[numthreads(16, 16, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    float2 uv = id.xy;

    vector vNormalDesc = g_NormalTexture.Load(int3(uv, 0));
    vector vDepthDesc = g_DepthTexture.Load(int3(uv, 0));

    if (vNormalDesc.a != 0.f || vDepthDesc.a != 0.f)
    {
        g_OutputTexture[uv] = float4(1.f, 1.f, 1.f, 1.f);
    }
    else
    {
        // 뷰스페이스 위치로 옮기기
        float3 vNormal = vector(normalize(mul(vNormalDesc * 2.f - 1.f, CamViewMatrix)).xyz, 0.f);

        // 뷰 행렬 상의 위치 구하기
        vector vPosition;
        
        float2 vTexcoord = uv / float2(1280.f, 720.f);

        vPosition.x = vTexcoord.x * 2.f - 1.f;
        vPosition.y = vTexcoord.y * -2.f + 1.f;
        vPosition.z = vDepthDesc.x; /* 0 ~ 1 */
        vPosition.w = 1.f;

        vPosition = vPosition * (vDepthDesc.y * fFar);
        vPosition = mul(vPosition, ProjMatrixInv);

        float4 fAmbient = SSAO(Get_TBN(vNormal, vTexcoord), vPosition.xyz);

        g_OutputTexture[uv] = 1.f - fAmbient;
    }
}