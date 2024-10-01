struct COMPUTEMATRIX
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float4 vDirection;
    float2 vLifeTime;
    float vRectSize;
    float4 vCamPos;
};

//이거 버퍼 바인딩은 꼭 다시 한번 둘러보기.
RWStructuredBuffer<COMPUTEMATRIX> g_OutMatrix : register(u0);

groupshared COMPUTEMATRIX shared_data[256];

[numthreads(16, 16, 1)]
void CS_Main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
    // 스레드 그룹 내부의 고유 인덱스 계산
    uint index = GTid.y * 16 + GTid.x;

    // 입력 버퍼에서 데이터 로드
    shared_data[index] = g_OutMatrix[DTid.x * 256 + index];

    // 스레드 그룹 내부에서 병렬 정렬 수행
    for (uint i = 1; i < 256; i *= 2)
    {
        if ((index % (2 * i)) == 0)
        {
            if (index + i < 256 && any(abs(shared_data[index].vTranslation.xyz - shared_data[index].vCamPos.xyz) < abs(shared_data[index + i].vTranslation.xyz - shared_data[index + i].vCamPos.xyz)))
            {
                COMPUTEMATRIX temp = shared_data[index];
                shared_data[index] = shared_data[index + i];
                shared_data[index + i] = temp;
            }
        }
        GroupMemoryBarrierWithGroupSync();
    }

    // 정렬된 데이터를 출력 버퍼에 쓰기
    g_OutMatrix[DTid.x * 256 + index] = shared_data[index];
}