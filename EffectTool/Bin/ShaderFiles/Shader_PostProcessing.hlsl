#include "Shader_Effect.hlsl"

int g_iSample = { 10 };
float g_fPower = { 0.01 };

PS_OUT PS_MAIN_RADIALBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBlurColor = vector(0.f, 0.f, 0.f, 0.f);
    
    float2 vCenter = float2(0.5f, 0.5f);
    
    float2 vDir = In.vTexcoord - vCenter;
    float f = 1.f / float(g_iSample);
    
    for (int i = 0; i < g_iSample; ++i)
    {
        vBlurColor += g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord - g_fPower * vDir * float(i)) * f;
    }
    
    Out.vColor = vector(vBlurColor.xyz, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_SCREENCRACK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBlurColor = vector(0.f, 0.f, 0.f, 0.f);
    
    float2 vCenter = float2(0.5f, 0.5f);
    float fPower = 0.01f;
    
    float2 vDir = In.vTexcoord - vCenter;

    vBlurColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vector(vBlurColor.xyz, 1.f);
    
    return Out;
}

float3 RGBToHSL(float3 color)
{
    float fMax = max(max(color.r, color.g), color.b);
    float fMin = min(min(color.r, color.g), color.b);
    float h = 0.0;
    float s = 0.0;
    float l = (fMax + fMin) / 2.0;

    if (fMax != fMin)
    {
        float d = fMax - fMin;
        s = l > 0.5 ? d / (2.0 - fMax - fMin) : d / (fMax + fMin);

        if (fMax == color.r)
            h = (color.g - color.b) / d + (color.g < color.b ? 6.0 : 0.0);
        else if (fMax == color.g)
            h = (color.b - color.r) / d + 2.0;
        else
            h = (color.r - color.g) / d + 4.0;

        h /= 6.0;
    }

    return float3(h, s, l);
}

float3 HSLToRGB(float3 hsl)
{
    float h = hsl.x;
    float s = hsl.y;
    float l = hsl.z;

    float c = (1.0 - abs(2.0 * l - 1.0)) * s;
    float x = c * (1.0 - abs(fmod(h / 60.0, 2.0) - 1.0));
    float m = l - c / 2.0;

    float3 rgb;

    if (h < 60.0)
        rgb = float3(c, x, 0);
    else if (h < 120.0)
        rgb = float3(x, c, 0);
    else if (h < 180.0)
        rgb = float3(0, c, x);
    else if (h < 240.0)
        rgb = float3(0, x, c);
    else if (h < 300.0)
        rgb = float3(x, 0, c);
    else
        rgb = float3(c, 0, x);

    rgb += float3(m, m, m);

    return rgb;
}

float g_fInvertDuration = { 0.5f };
//흑백으로 변경 후 컬러반전
PS_OUT PS_MAIN_INVERTCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vHSL = RGBToHSL(vColor.xyz);
     
    float fFactor = saturate(g_fTimeDelta / g_fInvertDuration);
    
    vHSL.x = fmod(vHSL.x + 180.f, 360.f); // 색반전
    vHSL.z = fFactor - vHSL.z; // 명도반전
   
    Out.vColor = vector(HSLToRGB(vHSL), 1.f);
    return Out;
}

//흑백으로 변경
float g_fSaturationFactor;

PS_OUT PS_MAIN_INVERTSATURATION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vHSL = RGBToHSL(vColor.xyz);
    
    vHSL.y = vHSL.y * g_fSaturationFactor; // 채도반전
   
    Out.vColor = vector(HSLToRGB(vHSL), 1.f);
    return Out;
}

float g_fVignetteIntensity = 0.5f;
float g_fVignetteSmootness = 0.2f; // 가장자리의 어두운 영역이 부드럽게 확장되는 정도

PS_OUT PS_MAIN_VIGNETTE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vector(1.f - vColor.xyz, 1.f);
    
    float2 vPosition = In.vTexcoord * 2.f - 1.f;
    float vDistance = length(vPosition);
    
    float vVignette = smoothstep(g_fVignetteIntensity, g_fVignetteIntensity + g_fVignetteSmootness, vDistance);
    
    vColor.rgb *= (1.f - vVignette);
    
    Out.vColor = vector(vColor.rgb, 1.f);
    
    return Out;
}

float g_fBrightness = { 0.1f }; // 밝기 조정
float g_fContrast = { 1.f }; // 대비 보정
float g_fSaturation = { 1.1f }; // 채도 보정
float4 g_vTint = float4(1.05f, 0.95f, 1.f, 0.f); // 색조 조정 (약간의 틴트를 추가)

float3 AdjustColor(float3 color)
{
    // 밝기 조정
    color += g_fBrightness;

    // 대비 조정
    color = (color - 0.5f) * g_fContrast + 0.5f;

    // 채도 조정
    float3 grayscale = dot(color, float3(0.299f, 0.587f, 0.114f)); // 흑백 변환
    color = lerp(grayscale.xxx, color, g_fSaturation);

    // 색조 조정
    color *= g_vTint;

    // 색상 클램핑 (0~1 범위 유지)
    color = saturate(color);

    return color;
}

PS_OUT PS_MAIN_ABJECTCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = float4(AdjustColor(vColor.xyz), 1.f);
    
    return Out;
}