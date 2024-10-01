uint2 g_WinSize = { 1280, 720 };

static const float g_fWeight[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
};

float g_fTotal = 0.5f;

Texture2D<float4> InputTexture : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

[numthreads(256, 1, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID, int3 GroupIndex : SV_GroupThreadID)
{
    int2 outputCoord = dispatchThreadID.xy; 
     
    int width, height;
    InputTexture.GetDimensions(width, height);
    int2 ImageSize = int2(width, height);

    vector vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    [unroll]
    for (int i = -6; i < 7; ++i)
    {
        int2 offsetCoord = outputCoord + int2(i, 0);
        offsetCoord.x = clamp(offsetCoord.x, 0, int(ImageSize.x - 1));
        vColor += g_fWeight[6 + i] * InputTexture.Load(int3(offsetCoord, 0));
    }
    
    vColor *= 0.5f;
    
    OutputTexture[outputCoord] = vColor;
}