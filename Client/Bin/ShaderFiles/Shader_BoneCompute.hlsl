struct GS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHTS;
};

struct GS_OUT
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

matrix g_BoneMatrices[512] : register(b0);

StructuredBuffer<GS_IN> g_InputBuffer : register(t0);
RWStructuredBuffer<GS_OUT> g_OutputBuffer : register(u0);

[numthreads(64, 1, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{

    // 전체 정점 인덱스 계산
    uint globalIndex = dispatchThreadID.x;
    
  // 전체 정점 인덱스를 사용해 입력 버퍼에서 정점 데이터 가져오기
    GS_IN In = g_InputBuffer[globalIndex];
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix TransformMatrix = mul(g_BoneMatrices[In.vBlendIndices.x], In.vBlendWeights.x) +
    mul(g_BoneMatrices[In.vBlendIndices.y], In.vBlendWeights.y) +
    mul(g_BoneMatrices[In.vBlendIndices.z], In.vBlendWeights.z) +
    mul(g_BoneMatrices[In.vBlendIndices.w], fWeightW);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), TransformMatrix);
    vector vTangent = mul(float4(In.vTangent, 0.f), TransformMatrix);
    
        // 글로벌 인덱스를 사용해 출력 버퍼에 저장
    g_OutputBuffer[globalIndex].vPosition = vPosition.xyz;
    g_OutputBuffer[globalIndex].vNormal = vNormal.xyz;
    g_OutputBuffer[globalIndex].vTexcoord = In.vTexcoord;
    g_OutputBuffer[globalIndex].vTangent = vTangent.xyz;

}