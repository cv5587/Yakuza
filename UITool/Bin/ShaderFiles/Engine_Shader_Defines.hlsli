
sampler ClampSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = clamp;
    AddressV = clamp;
};


sampler PointSampler = sampler_state
{
    filter = min_mag_mip_Point;
    AddressU = WRAP;
    AddressV = WRAP;
};


sampler LinearSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = WRAP;
    AddressV = WRAP;
};

/*
sampler ShadowSampler = sampler_state
{
    filter = min_mag_mip_Point;
    AddressU = clamp;
    AddressV = clamp;
};
*/

SamplerComparisonState ShadowSampler = sampler_state
{
    filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;

    ComparisonFunc = LESS_equal;
};

RasterizerState RS_Default
{
    FillMode = SOLID;
    CullMode = BACK;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe_Cull_NON_CW
{
    FillMode = WIREFRAME;
    CullMode = NONE;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = SOLID;
    CullMode = FRONT;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_NON_CW
{
    FillMode = SOLID;
    CullMode = NONE;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_None_Test_None_Write
{
    DepthEnable = false;
    DepthWriteMask = zero;
};


//vector		vSrcColor = vector(1.f, 0.f, 0.f, 0.5f);
//vector		vDestColor = vector(0.f, 1.f, 0.f, 1.f);

//vector		vResult = vSrcColor.rgb * vSrcColor.a + 
//	vDestColor * (1.f - vSrcColor.a);

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
    SrcBlendAlpha = Src_Alpha;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = Add;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};

BlendState BS_Blend_Test
{
    BlendEnable[0] = true;
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = INV_SRC_ALPHA;
    BlendOpAlpha = Add;
};

BlendState BS_WeightsBlend
{
    BlendEnable[0] = true;
    SrcBlend[0] = One;
    DestBlend[0] = One;
    BlendOp[0] = Add;


    BlendEnable[1] = true;
    SrcBlend[1] = Zero;
    DestBlend[1] = INV_SRC_ALPHA;
    BlendOp[1] = ADD;

};
BlendState BS_OIT
{
    BlendEnable[0] = true;
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = INV_SRC_ALPHA;
    BlendOpAlpha = Add;
};

BlendState BS_UIMerge
{
    BlendEnable[0] = true;
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = Add;
};

BlendState BS_Screen
{
    BlendEnable[0] = true;
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = Add;
};
//src 소스 픽셀 (현제 그리는 픽셀)
//dest 대상픽셀(이미그려진 픽셀