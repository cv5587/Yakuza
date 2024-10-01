#include "Shader_PostProcessing.hlsl"

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


VS_OUT_REVERSE VS_MAIN_REVERSE(VS_IN In)
{
    VS_OUT_REVERSE Out = (VS_OUT_REVERSE) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    float3 vPosReverse = In.vPosition;
    vPosReverse.y *= -1;
    
    Out.vPositionReverse = mul(float4(vPosReverse, 1.f), matWVP);

    return Out;
}

struct PS_OUT_GAMEOBJECT
{
    vector vColor : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRM : SV_TARGET3;
    vector vRS : SV_Target4;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (g_isArray)
    {
        for (int i = 0; i < 3; i++)
        {
            Out.vColor = g_TextureArray.Sample(LinearSampler, float3(In.vTexcoord, 0));
        }
    }
    else
        Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vLightMap : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float vAmbientDesc = g_AmbientTexture.Sample(LinearSampler, In.vTexcoord).r;
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    vector vAmbient = g_vLightAmbient * g_vMtrlAmbient;
    if (g_isSSAO)
        vAmbient *= vAmbientDesc;
    
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + vAmbient);
    Out.vLightMap = g_vLightDiffuse;
    /*
    if (g_isPBR)
    {

        vector vWorldPos;

        vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
        vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
        vWorldPos.z = vDepthDesc.x;
        vWorldPos.w = 1.f;

        vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);
        
        vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
        
        vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
        
        float3 vLook = normalize(g_vCamPosition - vWorldPos).xyz;
        

    }
    */
    //Grass
    /*
    vector vGlassNormalDesc = g_GlassNormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassNormal = vector(vGlassNormalDesc.xyz * 2.f - 1.f, 0.f);
    if (vGlassNormalDesc.a == 0.f)
    {
        vector vGlassDepthDesc = g_GlassDepthTexture.Sample(PointSampler, In.vTexcoord);
    
        vector vWorldPos;

        vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
        vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
        vWorldPos.z = vGlassDepthDesc.x;
        vWorldPos.w = 1.f;

        vWorldPos = vWorldPos * (vGlassDepthDesc.y * g_fFar);
    
        vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
        vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

        vector vReflect = reflect(normalize(g_vLightDir), normalize(vGlassNormal));
        vector vLook = g_vCamPosition - vWorldPos;
    
        Out.vSpecular = pow(max(dot(normalize(vReflect), normalize(vLook)), 0.f), 30.f);
    }
    */
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);

    float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

    Out.vShade = g_vLightDiffuse *
		saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    
    Out.vLightMap = g_vLightDiffuse * fAtt;
    
    return Out;
}

float PCF_Shadow(vector vWorldPos)
{
    for (int i = 0; i < 3; ++i)
    {
        vector vLightPos = mul(vWorldPos, g_ViewMatrixArray[i]);
        vLightPos = mul(vLightPos, g_ProjMatrixArray[i]);
        vLightPos = vector(vLightPos.xyz / vLightPos.w, vLightPos.w);
            
        float2 vTexcoord;
        vTexcoord.x = vLightPos.x * 0.5f + 0.5f;
        vTexcoord.y = vLightPos.y * -0.5f + 0.5f;
        if (vTexcoord.x < 0 || vTexcoord.x > 1 || vTexcoord.y < 0 || vTexcoord.y > 1)
            continue;
            
        int dx = 1 / 1280;
        int dy = 1 / 720;
            
        int2 vOffset[9] =
        {
            int2(-dx, -dy), int2(0, -dy), int2(dx, -dy),
                int2(-dx, 0), int2(0, 0), int2(dx, 0),
                int2(-dx, dy), int2(0, dy), int2(dx, dy)
        };
            
        float fResult = 0.f;
            
            [unroll]
        for (int j = 0; j < 9; j++)
            fResult += g_LightDepthTextureArray.SampleCmpLevelZero(ShadowSampler, float3(vTexcoord, i), vLightPos.z - 0.001f, vOffset[j]).r;

        fResult /= 9.f;
            
        return fResult;
    }
    
    return 1.f;
}

PS_OUT PS_MAIN_COPY_BACKBUFFER_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (g_isPBR)
    {
        vector vSpeculerRM = g_RMTexture.Sample(LinearSampler, In.vTexcoord);
        vector vLightmap = g_LightMapTexture.Sample(LinearSampler, In.vTexcoord);
        vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
        vector vNeoShader = vector(vSpeculerRM.xyz, 1.f) * vShade;
        
        Out.vColor = (vNeoShader + vSpecular);
    }
    else
    {
        Out.vColor = vector(vDiffuse.xyz, 1.f) * vShade;
    }
    
    if (g_isShadow)
    {
        vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
        
        vector vWorldPos;

        vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
        vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
        vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
        vWorldPos.w = 1.f;

        vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);
        
        vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
        vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
        
        float fShadowFactor = PCF_Shadow(vWorldPos);
        
        Out.vColor = vector(lerp(Out.vColor.xyz * 0.5f, Out.vColor.xyz, fShadowFactor), 1.f);
    }
    
    return Out;
}

PS_OUT PS_MAIN_DEFERRED_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_BackBufferTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse;

    return Out;
}

PS_OUT PS_INCLUDE_GLASS(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vGlassDiffuseColor = g_GlassDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (0 != vGlassDiffuseColor.a)
    {
        // Nonblend
        Out.vColor = vGlassDiffuseColor;
    }
    else
    {
        Out.vColor = vDiffuseColor;
    }

    
    return Out;
}

//BOF
PS_OUT PS_MAIN_BOF(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuseDesc = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDiffuseBlurDesc = g_BackBlurTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    float fDistance = distance(vWorldPos.xyz, g_vCamPosition.xyz);
    
    Out.vColor = lerp(vDiffuseDesc, vDiffuseBlurDesc, min(fDistance / g_fFar * 7.f, 1.f));
    
    return Out;
}


PS_OUT PS_ADD_PUDDLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vPuddle = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (0 == vPuddle.r && 0 == vPuddle.g && 0 == vPuddle.b)
        discard;
    
    Out.vColor = vPuddle;

    
    return Out;
}


PS_OUT PS_MAIN_NonBlurNonLight_Final(PS_IN In)
{
    // NonLightNonBlur와 뒷배경 합치기
    PS_OUT Out = (PS_OUT) 0;
    
    vector vNonLightNonBlur = g_NonLightNonBlurTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vFinal;
    
    if (0 != vNonLightNonBlur.r || 0 != vNonLightNonBlur.g || 0 != vNonLightNonBlur.b)
    {
        vFinal = vNonLightNonBlur;
    }
    else
    {
        discard;
    }
    
    Out.vColor = vFinal;

    return Out;
}



PS_OUT PS_CONTAIN_DECAL(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vDecal = g_DecalTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDecal.r != 0 && vDecal.g != 0 && vDecal.b != 0)
    {
        vDiffuseColor = lerp(vDiffuseColor, vDecal, vDecal.a);
    }
        
    Out.vColor = vDiffuseColor;
    
    
    return Out;
}



technique11 DefaultTechnique
{

	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
    pass DebugRender // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Copy_BackBuffer // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COPY_BACKBUFFER_RESULT();
    }

    pass Deferred_Result // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_RESULT();
    }

    pass Luminance // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM();
    }

    pass Luminance_Roof // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM_LOOP();
    }

    pass Luminance_Final // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE();
    }

    pass CopyLuminance // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COPYLUMINANCE();
    }

    pass ToneMapping // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TONEMAPPING();
    }

    pass Luminance_1x1 // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMINANCE_SUM_FINAL();
    }

    pass Blur_X //11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass Blur_Y //12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_REVERSE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass Result //13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RESULT();
    }


    pass SSAO //14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();
    }

    pass OITRender //15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OIT_RESULT();
    }
    
    pass IncludeGlassRender //16 - diffuse와 Glass 합치기
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_INCLUDE_GLASS();
    }

    pass BOF //17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOF();
    }


    pass DiffusePuddleAdd //18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ADD_PUDDLE();
    }

    pass RimLight //19
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RIMLIGHT();
    }

    pass Distortion //20
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }


    pass NonLightNonBlurFinal //21
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NonBlurNonLight_Final();
    }

    pass RadialBlur //22
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RADIALBLUR();
    }

    pass ScreenCrack //23
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SCREENCRACK();
    }

    pass InvertColor //24
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INVERTCOLOR();
    }

    pass Vignette //25
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VIGNETTE();
    }

    pass DecalContain //26
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CONTAIN_DECAL();
    }

    pass InvertLightness//27
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_INVERTSATURATION();
    }

    pass AdjustColor //28
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ABJECTCOLOR();
    }
}