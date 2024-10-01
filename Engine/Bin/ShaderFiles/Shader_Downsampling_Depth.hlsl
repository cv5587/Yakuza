Texture2D<float> InputTexture : register(t0); // t : 인풋 버퍼로 사용, 0 ~ 9 슬롯
RWTexture2D<float> OutputTexture : register(u0); // u : 아웃풋 버퍼, 0 ~ 9 

[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // Depth는 보간을 하면 안됨
    
    uint2 outputCoord = dispatchThreadID.xy; // 픽셀의 고유번호 

    // 초기값을 큰 값으로 설정 (깊이값은 보통 0.0이 카메라에 가까운, 1.0이 멀리 있는 값)
    float minDepth = InputTexture.Load(int3(outputCoord * 2 + uint2(0, 0), 0)).x;

    // 2x2 영역의 픽셀 중 최소 Depth 값을 선택
    minDepth = min(minDepth, InputTexture.Load(int3(outputCoord * 2 + uint2(0, 0), 0)).x);
    minDepth = min(minDepth, InputTexture.Load(int3(outputCoord * 2 + uint2(1, 0), 0)).x);
    minDepth = min(minDepth, InputTexture.Load(int3(outputCoord * 2 + uint2(0, 1), 0)).x);
    minDepth = min(minDepth, InputTexture.Load(int3(outputCoord * 2 + uint2(1, 1), 0)).x);

    // 최종 최소 Depth 값을 Output Texture에 저장
    OutputTexture[outputCoord] = minDepth;
}