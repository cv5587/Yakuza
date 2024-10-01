struct GS_IN
{
    float3 vPosition : POSITION; // 12바이트
    float3 vTexcoord : TEXCOORD0; // 12바이트
    float padding1 : TEXCOORD1; // 4바이트 패딩 (정렬 맞춤)
    float padding2 : TEXCOORD2; // 4바이트 패딩 (정렬 맞춤)
};

cbuffer ObjectData : register(b0)
{
    matrix worldViewProjMatrix; // 월드-뷰-투영 행렬
    float g_fFar; // 카메라의 Far plane 거리
    float padding[3];
};

StructuredBuffer<GS_IN> g_InputBuffer : register(t0); // GS_IN 데이터 구조체 배열
Texture2D<float4> depthTexture : register(t1); // 3x2 Depth 텍스처
RWStructuredBuffer<int> g_OutputBuffer : register(u0); // 가시성 판단 후 픽셀 수 기록

// 8개의 스레드를 한 그룹
[numthreads(8, 1, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint index = dispatchThreadID.x;

    // 인덱스가 버퍼 크기보다 큰 경우 빠르게 반환
    if (index >= g_InputBuffer.Length)
    {
        return;
    }

    // 정점 데이터 가져오기
    GS_IN cubeVertex = g_InputBuffer[index];
    float4 worldPos = float4(cubeVertex.vPosition, 1.0f);
    float4 projectedPos = mul(worldPos, worldViewProjMatrix);

    // w 나누기한 값 - vertex
    float ndcDepth = projectedPos.z / projectedPos.w;

    // 텍스처 좌표계에서의 정점 위치 계산 (3x2 텍스처에 맞춰 계산)
    uint2 texCoord = uint2(index % 1280, index / 1280);


    // 텍스처 좌표가 유효한지 확인
    if (texCoord.x >= 1280 || texCoord.y >= 720)
    {
        g_OutputBuffer[index] = 0; // 잘못된 좌표에서의 접근을 방지
        return;
    }

    // Depth Texture에서 해당 좌표의 깊이 값을 가져오기
    float4 depthValue = depthTexture.Load(int3(texCoord, 0));

    // 텍스처에서 가져온 깊이 값은 NDC 공간에 있을 가능성이 높음.
    // 따라서 이를 월드 좌표계 깊이와 비교하기 전에 Far Plane을 곱해 선형화함.
    float depthInTexture = depthValue.x; // 선형화된 깊이 값
    bool isVisible = (ndcDepth <= depthInTexture);

    // 가시성 결과를 int 값으로 기록 (1: 가시, 0: 가려짐)
    g_OutputBuffer[index] = isVisible ? 1 : 0;
}
