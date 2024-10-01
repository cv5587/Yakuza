
Texture2D<float4> InputTexture : register(t0); // t : 인풋 버퍼로 쓰겠다. 0 ~ 9 슬롯 (적제되는 넘버링?)
RWTexture2D<float4> OutputTexture : register(u0); // u : 아웃풋 버퍼, 0 ~ 9 

[numthreads(16, 16, 1)]
void GS_MAIN(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 outputCoord = dispatchThreadID.xy; // 픽셀의 고유번호 

    float4 colorSum = float4(0, 0, 0, 0);

    // 2x2 영역의 픽셀을 평균화하여 다운샘플링 
    colorSum += InputTexture.Load(int3(outputCoord * 2 + uint2(0, 0), 0));
    colorSum += InputTexture.Load(int3(outputCoord * 2 + uint2(1, 0), 0));
    colorSum += InputTexture.Load(int3(outputCoord * 2 + uint2(0, 1), 0));
    colorSum += InputTexture.Load(int3(outputCoord * 2 + uint2(1, 1), 0));

    // 평균값 저장
    OutputTexture[outputCoord] = colorSum / 4.0; 
}