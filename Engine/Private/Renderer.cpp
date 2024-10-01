#include "Renderer.h"

#include "GameInstance.h"
#include "GameObject.h"

#include "BlendObject.h"

//_uint		g_iSizeX = 6400;//8192 / 2;
//_uint		g_iSizeY = 3600;//4608 / 2;

/*
셰이더 전체 흐름
!! 수정중

[Diferred Shader]

	Priority ->  Shadow -> NonBlend -> Decal -> Glass ->

	[SSAO]
	SSAO -> SSAOBlur ->

	LightAcc -> CopyBackBuffer -> DeferredResult ->

[후처리 Shader]

	[HDR]
	Luminance -> AvgLuminance -> CopyLuminance -> HDR -> LuminanceResult ->

	[BOF]
	DeferredBlur -> Puddle -> BOF ->

	[Effect]
	NonLight -> Bloom -> FinalEffectBlend -> Blend -> Effect ->FinalOIT ->

	UI (최종)
*/

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
	if (FAILED(Ready_Targets()))
		return E_FAIL;

	if (FAILED(Ready_MRTs()))
		return E_FAIL;


	if (FAILED(Ready_OcculusionDepth()))
		return E_FAIL;


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pComputeShader[DOWNSAMPLING] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Downsampling.hlsl"));
	if (nullptr == m_pComputeShader[DOWNSAMPLING])
		return E_FAIL;
	m_pComputeShader[BLURX] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_BlurX.hlsl"));
	if (nullptr == m_pComputeShader[BLURX])
		return E_FAIL;
	m_pComputeShader[BLURY] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_BlurY.hlsl"));
	if (nullptr == m_pComputeShader[BLURY])
		return E_FAIL;
	m_pComputeShader[DOWNSAMPLING_DEPTH] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Downsampling_Depth.hlsl"));
	if (nullptr == m_pComputeShader[DOWNSAMPLING_DEPTH])
		return E_FAIL;
	m_pComputeShader[SSAO] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_SSAO.hlsl"));
	if (nullptr == m_pComputeShader[SSAO])
		return E_FAIL;
	m_pComputeShader[PBR] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_PBR.hlsl"));
	if (nullptr == m_pComputeShader[PBR])
		return E_FAIL;

	/*SSAO를 위한 버퍼*/
	D3D11_BUFFER_DESC SSAODesc{};
	SSAODesc.Usage = D3D11_USAGE_DEFAULT;
	SSAODesc.ByteWidth = sizeof(SSAO_BUFFER);
	SSAODesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	SSAODesc.CPUAccessFlags = 0;
	SSAODesc.StructureByteStride = sizeof(SSAO_BUFFER);

	if (FAILED(m_pDevice->CreateBuffer(&SSAODesc, nullptr, &m_pSSAOBuffer)))
		return E_FAIL;

	/*PBR를 위한 버퍼*/
	D3D11_BUFFER_DESC PBRDesc{};
	PBRDesc.Usage = D3D11_USAGE_DEFAULT;
	PBRDesc.ByteWidth = sizeof(PBR_BUFFER);
	PBRDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	PBRDesc.CPUAccessFlags = 0;
	PBRDesc.StructureByteStride = sizeof(PBR_BUFFER);

	if (FAILED(m_pDevice->CreateBuffer(&PBRDesc, nullptr, &m_pPBRBuffer)))
		return E_FAIL;

	/* 화면을 꽉 채워주기 위한 월드변환행렬. */
	D3D11_VIEWPORT ViewPort{};
	_uint iNumViewPort = 1;
	m_pContext->RSGetViewports(&iNumViewPort, &ViewPort);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewPort.Width, ViewPort.Height, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f));

	if (FAILED(Ready_LightDepth()))
		return E_FAIL;

	if (FAILED(Ready_SSAONoiseTexture()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Diffuse"), 50.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Normal"), 50.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Depth"), 50.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Surface"), 50.f, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_OEShader"), 50.f, 450.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_LightDepth"), ViewPort.Width - 150.0f, 150.0f, 300.f, 300.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Shade"), 150.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_SSAO"), 150.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_PBR"), 150.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_BackBuffer"), 150.f, 350.f, 100.f, 100.f)))
		return E_FAIL;

	//Effect 정렬
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Effect"), 250.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_AccumColor"), 250.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_AccumAlpha"), 250.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Distortion"), 250.f, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_RimLight"), 250.f, 450.f, 100.f, 100.f)))
		return E_FAIL;

	//최종 셰이더
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_FinalResult"), 350.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Luminance"), 350.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_CopyLuminance"), 350.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_ToneMapping"), 350.f, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_BOF"), 350.f, 450.f, 100.f, 100.f)))
		return E_FAIL;

	//깊이 정렬
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_OcculusionDepth"), 650.f, 50.f, 100.f, 100.f)))
		return E_FAIL;

#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Ready_Targets()
{
	D3D11_VIEWPORT ViewPort{};
	_uint iNumViewPort = 1;
	m_pContext->RSGetViewports(&iNumViewPort, &ViewPort);

	//20240712_NonBlendDiffuse 의 백버퍼 칼라(알파 빼고 )가 기본 스카이 박스에 곱해져서 색이 핑크핑크로 나왔었음 일단 0,0,0,0 으로 바꾸면 수습가능 (만약 다른색 원하면 구조 고쳐야됨.
	/*Target_Diffuse*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_Normal*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/*Target_Depth*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Surface */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Surface"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_OEShader */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_OEShader"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_LightDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), false, 3)))
		return E_FAIL;

	/* Target_LightMap */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightMap"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_PBR */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PBR"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;

	/*Target_Shader*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;

	/*Target_Ambient*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Ambient"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 0.f, 0.f, 1.f), true)))
		return E_FAIL;
	/*Target_SSAO*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CopyAmbient"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f), true)))
		return E_FAIL;

	/*Target_BackBuffer*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BackBuffer"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_Diffuse*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PuddleDiffuse"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_ToneMapping*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ToneMapping"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/*Target_BackBlur*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BackBlur"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f), true)))
		return E_FAIL;

	/*Target_BackBlur*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BackBlurReverse"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/*Target_BOF*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BOF"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	//Luminance
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_640x360"), 640, 360, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_320x180"), 320, 180, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_160x90"), 160, 90, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_80x45"), 80, 45, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_40x23"), 40, 23, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_20x12"), 20, 12, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_10x6"), 10, 6, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_5x3"), 5, 3, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_3x2"), 3, 2, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_1x1"), 1, 1, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CopyLuminance"), 1, 1, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Luminance"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;


#pragma region MRT_Bloom
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
#pragma endregion

	//ID3D11BlendState
#pragma region MRT_OIT
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AccumColor"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AccumAlpha"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
#pragma endregion

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_RimLight"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/*Target_NonLightNonBlur*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_NonLightNonBlur"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_Decal*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Decal"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_Glass*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glass"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_DecalContainDiffuse*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DecalContainDiffuse"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_GlassContainDiffuse*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_GlassContainDiffuse"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_FinalEffect*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_FinalEffect"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	
	/*Target_RadialBlur*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_RadialBlur"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_SceneCrack*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SceneCrack"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_InvertColor*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_InvertColor"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_InvertLightnessColor*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_InvertLightnessColor"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_Vignette*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Vignette"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Target_FinalResult*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_FinalResult"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma region MRT_Occulusion
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_OcculusionDepth"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
#pragma endregion



	return S_OK;
}

HRESULT CRenderer::Ready_MRTs()
{
	/*MRT_NonBlend*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_NonBlend"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_NonBlend"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_NonBlend"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_NonBlend"), TEXT("Target_Surface"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_NonBlend"), TEXT("Target_OEShader"))))
		return E_FAIL;

	/*MRT_PBR*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PBR"), TEXT("Target_PBR"))))
		return E_FAIL;


	/* MRT_ShadowObject */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObjects"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	/*MRT_LightAcc*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_LightMap"))))
		return E_FAIL;

	/*MRT_SSAO*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_Ambient"))))
		return E_FAIL;

	/*MRT_SSAOBlur*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAOBlur"), TEXT("Target_SSAO"))))
		return E_FAIL;

	/*MRT_CopyBackBuffer*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_CopyBackBuffer"), TEXT("Target_BackBuffer"))))
		return E_FAIL;


	/*MRT_Puddle*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Puddle"), TEXT("Target_PuddleDiffuse"))))
		return E_FAIL;

	/*MRT_DiffuseAddPuddle*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DiffuseAddPuddle"), TEXT("Target_BackBuffer"))))
		return E_FAIL;

	/*MTR_DeferredBlur*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MTR_DeferredBlur"), TEXT("Target_BackBlur"))))
		return E_FAIL;

	/*MTR_DeferredBlurReverse*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MTR_DeferredBlur"), TEXT("Target_BackBlurReverse"))))
		return E_FAIL;

	/*MTR_BOF*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MTR_BOF"), TEXT("Target_BOF"))))
		return E_FAIL;

#pragma region HDR
	/*MRT_HDR*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_ToneMapping"))))
		return E_FAIL;

	/*MRT_1*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSampling"), TEXT("Target_640x360"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSampling"), TEXT("Target_320x180"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSampling"), TEXT("Target_160x90"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSampling"), TEXT("Target_10x6"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSampling"), TEXT("Target_5x3"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSampling"), TEXT("Target_3x2"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSampling"), TEXT("Target_1x1"))))
		return E_FAIL;

	/*MRT_Luminance*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Luminance"), TEXT("Target_Luminance"))))
		return E_FAIL;

	/*MRT_CopyLuminance*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_CopyLuminance"), TEXT("Target_CopyLuminance"))))
		return E_FAIL;
#pragma endregion

	/*MRT_Effect*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
		return E_FAIL;

	/*MRT_Blur_X*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;

	/*MRT_Blur_Y*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
		return E_FAIL;

	/*MRT_Accum*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Accum"), TEXT("Target_AccumColor"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Accum"), TEXT("Target_AccumAlpha"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Accum"), TEXT("Target_Distortion"))))
		return E_FAIL;

	/*MRT_RimLight*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_RimLight"), TEXT("Target_RimLight"))))
		return E_FAIL;

	/* MRT_NonLightNonBlur */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_NonLightNonBlur"), TEXT("Target_NonLightNonBlur"))))
		return E_FAIL;

	/* MRT_Decals */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Decals"), TEXT("Target_Decal"))))
		return E_FAIL;

	/* MRT_Glass */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glass"), TEXT("Target_Glass"))))
		return E_FAIL;


	/* MRT_DecalsContainDiffuse - Decal이랑 연산한 후의 diffuse */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DecalsContainDiffuse"), TEXT("Target_DecalContainDiffuse"))))
		return E_FAIL;

	/* MRT_GlassContainDiffuse - Decal이랑 연산한 후의 diffuse */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GlassContainDiffuse"), TEXT("Target_GlassContainDiffuse"))))
		return E_FAIL;


	/* MRT_Distortion */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_FinalEffect"))))
		return E_FAIL;

#pragma region PostProcess
	/* MRT_RadialBlur */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_RadialBlur"), TEXT("Target_RadialBlur"))))
		return E_FAIL;

	/* MRT_SceneCrack */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SceneCrack"), TEXT("Target_SceneCrack"))))
		return E_FAIL;

	/* MRT_InvertColor */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_InvertColor"), TEXT("Target_InvertColor"))))
		return E_FAIL;

	/* MRT_InvertLightnessColor */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_InvertLightnessColor"), TEXT("Target_InvertLightnessColor"))))
		return E_FAIL;

	/* MRT_InvertColor */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Vignette"), TEXT("Target_Vignette"))))
		return E_FAIL;
#pragma endregion

	/* MRT_FinalResult */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_FinalResult"), TEXT("Target_FinalResult"))))
		return E_FAIL;
	

	/* MRT_Occulusion */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Occulusion"), TEXT("Target_OcculusionDepth"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Ready_LightDepth()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pLightTextureView = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 1280;
	TextureDesc.Height = 720;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 3;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pLightTextureView)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthDesc{ DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_DSV_DIMENSION_TEXTURE2DARRAY, 0 };
	DepthDesc.Texture2DArray.ArraySize = 3;
	DepthDesc.Texture2DArray.FirstArraySlice = 0;
	DepthDesc.Texture2DArray.MipSlice = 0;
	if (FAILED(m_pDevice->CreateDepthStencilView(pLightTextureView, &DepthDesc, &m_pLightDepthStencilView)))
		return E_FAIL;

	Safe_Release(pLightTextureView);

	return S_OK;
}

HRESULT CRenderer::Ready_SSAONoiseTexture() // SSAO 연산에 들어갈 랜덤 벡터 텍스쳐 생성
{
	ID3D11Texture2D* pSSAONoiseTexture = { nullptr };
	D3D11_TEXTURE2D_DESC		TextureDesc{};

	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 4;
	TextureDesc.Height = 4;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_float3* pPixel = new _float3[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pPixel, sizeof(_float3) * TextureDesc.Width * TextureDesc.Height);

	D3D11_SUBRESOURCE_DATA		InitialData{};

	for (int i = 0; i < 16; i++)
	{
		_float3 vNoise = {
			m_pGameInstance->Get_Random(0.f, 1.f),
			m_pGameInstance->Get_Random(0.f, 1.f),
			0.f
		};
		pPixel[i] = vNoise;
	}

	InitialData.pSysMem = pPixel;
	InitialData.SysMemPitch = 4 * sizeof(_float3);

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pSSAONoiseTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pSSAONoiseTexture, nullptr, &m_pSSAONoiseView)))
		return E_FAIL;

	Safe_Release(pSSAONoiseTexture);

	Safe_Delete_Array(pPixel);

	/* 랜덤 커널 만들기 */
	m_vSSAOKernal = new _float4[64];
	for (int i = 0; i < 64; i++)
	{
		_float4 vRandom = {
			m_pGameInstance->Get_Random(0.f, 1.f) * 2.f - 1.f,
			m_pGameInstance->Get_Random(0.f, 1.f) * 2.f - 1.f,
			m_pGameInstance->Get_Random(0.f, 1.f),
			0.f
		};

		XMStoreFloat4(&vRandom, XMVector4Normalize(XMLoadFloat4(&vRandom)));
		//XMStoreFloat3(&vRandom, XMLoadFloat3(&vRandom) * m_pGameInstance->Get_Random(0.f, 1.f));
		float vScale = (_float)i / 64.f;
		vScale = 0.3f + (vScale * vScale) * (0.8f - 0.3f);
		XMStoreFloat4(&vRandom, XMLoadFloat4(&vRandom) * vScale);

		m_vSSAOKernal[i] = vRandom;
	}

	return S_OK;
}

HRESULT CRenderer::Ready_OcculusionDepth()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthTextureView = nullptr;

	D3D11_TEXTURE2D_DESC depthDesc = {};
	ZeroMemory(&depthDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthDesc.Width = 1280;
	depthDesc.Height = 720;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;


	if (FAILED(m_pDevice->CreateTexture2D(&depthDesc, nullptr, &pDepthTextureView)))
		return E_FAIL;


	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 뷰의 포맷
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // 2D 텍스처로 뷰 설정
	depthStencilViewDesc.Flags = 0; // 추가 옵션 없음

	depthStencilViewDesc.Texture2D.MipSlice = 0; // Mip 레벨 0 사용

	// 깊이/스텐실 뷰 생성
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthTextureView, &depthStencilViewDesc, &m_pOcculusionDepthView)))
		return E_FAIL;

	Safe_Release(pDepthTextureView);

	return S_OK;
}

void CRenderer::Add_Renderer(RENDERER_STATE eRenderState, CGameObject* pGameObject)
{
	m_RenderObject[eRenderState].push_back(pGameObject);

	Safe_AddRef(pGameObject);
}

void CRenderer::Draw()
{
	Render_Priority();
	//Render_AnimSkinning();

	//Render_OcculusionDepth(); // 오쿨루젼 컬링 (먼저 연산해놓는다)

	if (m_isShadow)
	{
		Render_ShadowObjects();
	}

	Render_NonBlender();

	Render_Glass();

	Render_Decal();


	if (m_isSSAO)
	{
		Render_SSAO();
		Render_SSAOBlur();
	}

	if (m_isPBR)
	{
		Render_PBR();
	}

	Render_LightAcc();

	Render_CopyBackBuffer(); // 최종으로 그려서 백버퍼에 올라갈 이미지 복사
	Render_DeferredResult(); // 복사한 이미지를 백버퍼에 넣어줌. (Deferred 최종)

	if (m_isRimLight)
		Render_RimLight();

	// 간판을 위해 임의로 남겨두기
	Render_NonLight_NonBlur();

	if (m_isHDR)
		Render_DownSampling();

	

	// NonLight랑 Bloom은 같이 붙어있어야함
	Render_NonLight();
	Render_Bloom();
	Render_FinalEffectBlend();

	Render_Blender();
	Render_Effect();
	Render_FinlaOIT();
	Render_Distortion();

	/* PostProcessing*/
	if (m_isBOF)
	{
		Render_DeferredBlur();

		//Render_Puddle();

		Render_BOF();
	}

	//톤매핑 & 평균 휘도
	if (m_isHDR)
	{
		Render_AvgLuminance();
		Render_CopyLuminance();
		Render_LuminanceResult();
	}

	if (m_isRadialBlur)
		Render_RadialBlur();

	if (m_isInvertColor)
		Render_InvertColor();
	else m_fInvertTime = 0.f;

	if (m_isInvertSaturationColor)
		Render_InvertSaturationColor();

	if (m_isVignette)
		Render_Vignette();

	Render_FinalResult(); // 최종 후처리 

	Render_AdjustColor(); // 색감보정

	Render_UI();

#ifdef _DEBUG
	Render_Debug();
#endif // _DEBUG
}

void CRenderer::Clear()
{
	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto& pGameObject : m_RenderObject[i])
			Safe_Release(pGameObject);

		m_RenderObject[i].clear();
	}

#ifdef _DEBUG
	for (auto& iter : m_DebugComponents)
		Safe_Release(iter);

	m_DebugComponents.clear();

#endif // _DEBUG
}

void CRenderer::Occulusion_Culling_Draw()
{
	// 처음 Depth 비교
	Render_OcculusionDepth();

	// 다운 샘플링
	// Render_OcculusionDownSampling();

	// 다운 샘플링 한 것과 depth 비교
	//Check_OcculusionCulling();
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}
#endif // _DEBUG

void CRenderer::Render_Priority()
{
	m_iRenderState = RENDER_PRIORITY;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyBackBuffer"))))
		return;

	for (auto& iter : m_RenderObject[RENDER_PRIORITY])
	{
		if (nullptr != iter)
			iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_PRIORITY].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_AnimSkinning()
{
	for (auto& iter : m_RenderObject[RENDER_NONBLENDER])
	{
		iter->Render_BoneCompute();
	}
}

void CRenderer::Render_ShadowObjects()
{
	m_iRenderState = RENDER_SHADOWOBJ;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObjects"), m_pLightDepthStencilView)))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_SHADOWOBJ])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_SHADOWOBJ].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_NonBlender()
{
	m_iRenderState = RENDER_NONBLENDER;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_NonBlend"))))
		return;


	for (auto& iter : m_RenderObject[RENDER_NONBLENDER])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_NONBLENDER].clear();


	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Decal()
{
	m_iRenderState = RENDER_DECAL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Decals"))))
		return;

	for (auto& iter : m_RenderObject[RENDER_DECAL])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_DECAL].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DecalsContainDiffuse"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;


	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_GlassContainDiffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Decal"), m_pShader, "g_DecalTexture")))
		return;

	m_pShader->Begin(26);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Glass()
{
	m_iRenderState = RENDER_GLASS;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glass"))))
		return;

	for (auto& iter : m_RenderObject[RENDER_GLASS])
	{
		iter->Render();

		Safe_Release(iter);
	}

	m_RenderObject[RENDER_GLASS].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GlassContainDiffuse"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;


	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Glass"), m_pShader, "g_GlassDiffuseTexture")))
		return;

	m_pShader->Begin(16);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_SSAO()
{
	SSAO_BUFFER BufferDesc{};
	BufferDesc.fSSAOBise = m_fSSAOBiae;
	BufferDesc.fRadiuse = m_fSSAORadiuse;
	BufferDesc.fFar = *(m_pGameInstance->Get_CamFar());
	memcpy(BufferDesc.vSSAOKernal, m_vSSAOKernal, sizeof(_float4) * 64);

	BufferDesc.WorldMatrix = XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix));
	BufferDesc.ViewMatrix = XMMatrixTranspose(XMLoadFloat4x4(&m_ViewMatrix));
	BufferDesc.ProjMatrix = XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix));
	BufferDesc.ViewMatrixInv = XMMatrixTranspose(m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW));
	BufferDesc.ProjMatrixInv = XMMatrixTranspose(m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ));
	BufferDesc.CamViewMatrix = XMMatrixTranspose(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	BufferDesc.CamProjMatrix = XMMatrixTranspose(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pContext->UpdateSubresource(m_pSSAOBuffer, 0, nullptr, &BufferDesc, 0, 0);

	m_pContext->CSSetConstantBuffers(0, 1, &m_pSSAOBuffer);

	m_pContext->CSSetShaderResources(0, 1, &m_pSSAONoiseView);
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Depth"), 1);
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Normal"), 2);
	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_Ambient"));

	_uint GroupX = (1280 + 15) / 16;
	_uint GroupY = (720 + 15) / 16;

	m_pComputeShader[SSAO]->Render(GroupX, GroupY, 1);
}

void CRenderer::Render_SSAOBlur()
{
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Ambient"));
	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_Blur_X"));

	_uint GroupX = (1280 + 255) / 256;

	m_pComputeShader[BLURX]->Render(GroupX, 720, 1);
	
	_uint GroupY = (720 + 255) / 256;
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Blur_X"));
	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_SSAO"));

	m_pComputeShader[BLURY]->Render(1280, GroupY, 1);
}

void CRenderer::Render_PBR()
{
	PBR_BUFFER BufferDesc{};
	BufferDesc.fFar = *(m_pGameInstance->Get_CamFar());
	BufferDesc.vCamPosition = *(m_pGameInstance->Get_CamPosition_Float4());

	BufferDesc.WorldMatrix = XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix));
	BufferDesc.ViewMatrix = XMMatrixTranspose(XMLoadFloat4x4(&m_ViewMatrix));
	BufferDesc.ProjMatrix = XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix));
	BufferDesc.ViewMatrixInv = XMMatrixTranspose(m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW));
	BufferDesc.ProjMatrixInv = XMMatrixTranspose(m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ));
	BufferDesc.CamViewMatrix = XMMatrixTranspose(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	BufferDesc.CamProjMatrix = XMMatrixTranspose(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pContext->UpdateSubresource(m_pPBRBuffer, 0, nullptr, &BufferDesc, 0, 0);

	m_pContext->CSSetConstantBuffers(0, 1, &m_pPBRBuffer);
	//LightBuffer
	if (FAILED(m_pGameInstance->Bind_LightComputeBuffer(1)))
		return;

	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Depth"), 0);
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Normal"), 1);
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Surface"), 2);
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_DecalContainDiffuse"), 3);
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_OEShader"), 4);

	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_PBR"), 0);
	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_Specular"), 1);
	
	_uint GroupX = (1280 + 15) / 16;
	_uint GroupY = (720 + 15) / 16;

	m_pComputeShader[PBR]->Render(GroupX, GroupY, 1);
}

void CRenderer::Render_LightAcc()
{
	/* Light + PBR */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamDir", m_pGameInstance->Get_ComLook_Float4(), sizeof(_float4))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_isSSAO", &m_isSSAO, sizeof(_bool))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_isPBR", &m_isPBR, sizeof(_bool))))
		return;

	//오류생기면 
	/*if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_DecalContainDiffuse"), m_pShader, "g_DiffuseTexture")))
		return;*/
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_SSAO"), m_pShader, "g_AmbientTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;
	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_OEShader"), m_pShader, "g_OEShaderTexture")))
	//	return;
	/*if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_GlassNormal"), m_pShader, "g_GlassNormalTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_GlassDepth"), m_pShader, "g_GlassDepthTexture")))
		return;*/

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);


	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_CopyBackBuffer()
{
	//20240712 priority  에서 copybackbuffer  를 사용하므로 false로 둬야됨
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyBackBuffer"), nullptr, false)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return;

	const _float4x4* ViewMatrix;
	const _float4x4* ProjMatrix;

	ViewMatrix = m_pGameInstance->Get_Shadow_Transform_View_Float4x4();
	ProjMatrix = m_pGameInstance->Get_Shadow_Transform_Proj_Float4x4();

	if (FAILED(m_pShader->Bind_Matrices("g_ViewMatrixArray", ViewMatrix, 3)))
		return;
	if (FAILED(m_pShader->Bind_Matrices("g_ProjMatrixArray", ProjMatrix, 3)))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_isPBR", &m_isPBR, sizeof(_bool))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_isShadow", &m_isShadow, sizeof(_bool))))
		return;

	_vector vCamDir = m_pGameInstance->Get_CamLook();
	if (FAILED(m_pShader->Bind_RawValue("g_vCamDir", &vCamDir, sizeof(_vector))))
		return;

	//오류생기면 Target_Diffuse로 변경
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_DecalContainDiffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_PBR"), m_pShader, "g_RMTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_LightDepth"), m_pShader, "g_LightDepthTextureArray")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_LightMap"), m_pShader, "g_LightMapTexture")))
		return;

	m_pShader->Begin(3);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

}

void CRenderer::Render_Puddle()
{
	m_iRenderState = RENDER_PUDDLE;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Puddle"))))
		return;

	for (auto& iter : m_RenderObject[RENDER_PUDDLE])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_PUDDLE].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DiffuseAddPuddle"), nullptr, false)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_PuddleDiffuse"), m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(18);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_DeferredResult() // 백버퍼에 Diffuse와 Shade를 더해서 그려줌
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBuffer"), m_pShader, "g_BackBufferTexture")))
		return;

	m_pShader->Begin(4);

	m_pVIBuffer->Render();
}

void CRenderer::Render_DownSampling()
{
	//컴퓨트 다운샘플링 진행
	for (size_t i = 0; i < 10; ++i)
	{
		_uint threadGroupX = 1;
		_uint threadGroupY = 1;

		if (i == 0)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_BackBuffer"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_640x360"));
			threadGroupX = (640 + 15) / 16;
			threadGroupY = (360 + 15) / 16;
		}
		else if (i == 1)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_640x360"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_320x180"));
			threadGroupX = (320 + 15) / 16;
			threadGroupY = (180 + 15) / 16;
		}
		else if (i == 2)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_320x180"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_160x90"));
			threadGroupX = (160 + 15) / 16;
			threadGroupY = (90 + 15) / 16;
		}
		else if (i == 3)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_160x90"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_80x45"));
			threadGroupX = (80 + 15) / 16;
			threadGroupY = (45 + 15) / 16;
		}
		else if (i == 4)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_80x45"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_40x23"));
			threadGroupX = (40 + 15) / 16;
			threadGroupY = (23 + 15) / 16;
		}
		else if (i == 5)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_40x23"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_20x12"));
			threadGroupX = (20 + 15) / 16;
			threadGroupY = (12 + 15) / 16;
		}
		else if (i == 6)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_20x12"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_10x6"));
			threadGroupX = (10 + 15) / 16;
			threadGroupY = (6 + 15) / 16;
		}
		else if (i == 7)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_10x6"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_5x3"));
			threadGroupX = (5 + 15) / 16;
			threadGroupY = (3 + 15) / 16;
		}
		else if (i == 8)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_5x3"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_3x2"));
			threadGroupX = (3 + 15) / 16;
			threadGroupY = (2 + 15) / 16;
		}
		else if (i == 9)
		{
			m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_3x2"));
			m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_1x1"));
			threadGroupX = (1 + 15) / 16;
			threadGroupY = (1 + 15) / 16;
		}

		m_pComputeShader[DOWNSAMPLING]->Render(threadGroupX, threadGroupY, 1);
	}

}

void CRenderer::Render_NonLight_NonBlur()
{
	m_iRenderState = RENDER_NONLIGHT_NONBLUR;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_NonLightNonBlur"))))
		return;

	for (auto& iter : m_RenderObject[RENDER_NONLIGHT_NONBLUR])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_NONLIGHT_NONBLUR].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyBackBuffer"), nullptr, false)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_NonLightNonBlur"), m_pShader, "g_NonLightNonBlurTexture")))//최종블러텍스처
		return;

	m_pShader->Begin(21);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_RimLight()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyBackBuffer"), nullptr, false)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBuffer"), m_pShader, "g_BackBufferTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	m_pShader->Begin(19);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

/*NonLight*/
#pragma region NonLight

void CRenderer::Render_NonLight()
{
	m_iRenderState = RENDER_NONLIGHT;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect"))))
		return;

	for (auto& iter : m_RenderObject[RENDER_NONLIGHT])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_NONLIGHT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Bloom()
{

	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Effect"));
	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_Blur_X"));

	UINT GroupX = (1280 + 255) / 256;

	m_pComputeShader[BLURX]->Render(GroupX, 720, 1);

	UINT GroupY = (720 + 255) / 256;

	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Blur_X"));
	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_Blur_Y"));

	m_pComputeShader[BLURY]->Render(1280, GroupY, 1);


	/*
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Effect"), m_pShader, "g_EffectTexture")))
		return;

	m_pShader->Begin(11);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Blur_X"), m_pShader, "g_EffectTexture")))
		return;

	m_pShader->Begin(12);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
		*/
}

void CRenderer::Render_FinalEffectBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyBackBuffer"), nullptr, false)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Blur_Y"), m_pShader, "g_BlurTexture")))//최종블러텍스처
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Effect"), m_pShader, "g_EffectTexture")))//이펙트 텍스처 원본
		return;

	//	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(m_isBOF ? TEXT("Target_BOF") : TEXT("Target_BackBuffer"), m_pShader, "g_ResultTexture")))//원본 최종
		//	return;

	m_pShader->Begin(13);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

}
#pragma endregion

/*Blender*/
#pragma region Blender
void CRenderer::Render_Blender()
{
	m_iRenderState = RENDER_BLENDER;

	m_RenderObject[RENDER_BLENDER].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return dynamic_cast<CBlendObject*>(pSour)->Get_ViewZ() > dynamic_cast<CBlendObject*>(pDest)->Get_ViewZ();
		});


	for (auto& iter : m_RenderObject[RENDER_BLENDER])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_BLENDER].clear();

}

void CRenderer::Render_Effect()// 새로운 타겟에 파티클 그리기
{
	m_iRenderState = RENDER_EFFECT;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Accum"))))
		return;

	for (auto& iter : m_RenderObject[RENDER_EFFECT])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_EFFECT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_FinlaOIT() //파티클 그린 타겟 병합
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyBackBuffer"), nullptr, false)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_AccumColor"), m_pShader, "g_AccumTexture")))//이펙트 텍스처 원본
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_AccumAlpha"), m_pShader, "g_AccumAlpha")))//이펙트 텍스처 원본
		return;


	m_pShader->Begin(15);

	m_pVIBuffer->Render();


	if (FAILED(m_pGameInstance->End_MRT()))
		return;

}

//최종 파티클
void CRenderer::Render_Distortion()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Distortion"), m_pShader, "g_Distortion")))//이펙트 텍스처 원본
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBuffer"), m_pShader, "g_ResultTexture")))//원본 최종
		return;

	m_pShader->Begin(20);//디스토션 제작

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
#pragma endregion

/*Post Processing*/
#pragma region PostProcessing

void CRenderer::Render_DeferredBlur()
{
	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_FinalEffect"));
	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_Blur_X"));

	UINT GroupX = (1280 + 255) / 256;

	m_pComputeShader[BLURX]->Render(GroupX, 720, 1);

	UINT GroupY = (720 + 255) / 256;

	m_pGameInstance->Bind_ComputeRenderTargetSRV(TEXT("Target_Blur_X"));
	m_pGameInstance->Bind_ComputeRenderTargetUAV(TEXT("Target_BackBlur"));

	m_pComputeShader[BLURY]->Render(1280, GroupY, 1);

	m_pContext->Flush();
}

void CRenderer::Render_BOF()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MTR_BOF"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_FinalEffect"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBlur"), m_pShader, "g_BackBlurTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	m_pShader->Begin(17);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

/*HDR*/
#pragma region HDR
void CRenderer::Render_AvgLuminance()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Luminance"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_1x1"), m_pShader, "g_LuminanceTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_CopyLuminance"), m_pShader, "g_CopyLuminanceTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(m_isBOF ? TEXT("Target_BOF") : TEXT("Target_FinalEffect"), m_pShader, "g_BackBufferTexture")))
		return;

	m_pShader->Begin(7);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_CopyLuminance()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyLuminance"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_1x1"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(8);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

}

void CRenderer::Render_LuminanceResult()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_HDR"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_fLumVar", &m_fHDRLight, sizeof(_float))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(m_isBOF ? TEXT("Target_BOF") : TEXT("Target_FinalEffect"), m_pShader, "g_BackBufferTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Luminance"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(9);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
#pragma endregion


void CRenderer::Render_RadialBlur()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_RadialBlur"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_iSample", &m_iRadialSample, sizeof(_int))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_fPower", &m_fRadialPower, sizeof(_float))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(m_isHDR ? TEXT("Target_ToneMapping") : m_isBOF ? TEXT("Target_BOF") : TEXT("Target_FinalEffect"), m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(22);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Crack()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SceneCrack"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	m_pShader->Begin(22);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_InvertColor()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_InvertColor"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	m_fInvertTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Game"));
	if (FAILED(m_pShader->Bind_RawValue("g_fTimeDelta", &m_fInvertTime, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_fInvertDuration", &m_fInvertDuration, sizeof(_float))))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_FinalEffect"), m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(24);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_InvertSaturationColor()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_InvertLightnessColor"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_fSaturationFactor", &m_fSaturationFactor, sizeof(_float))))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(m_isRadialBlur ? TEXT("Target_RadialBlur") : m_isHDR ? TEXT("Target_ToneMapping") : m_isBOF ? TEXT("Target_BOF") : TEXT("Target_FinalEffect"), m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(27);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Vignette()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Vignette"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_fVignetteIntensity", &m_fVignetteIntensity, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_fVignetteSmootness", &m_fVignetteSmootness, sizeof(_float))))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(m_isHDR ? TEXT("Target_ToneMapping") : m_isBOF ? TEXT("Target_BOF") : TEXT("Target_FinalEffect"), m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(25);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
#pragma endregion

void CRenderer::Render_FinalResult()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_FinalResult"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	wstring strTexture = TEXT("Target_FinalEffect");

	if (m_isBOF) strTexture = TEXT("Target_BOF");
	if (m_isHDR) strTexture = TEXT("Target_ToneMapping");
	if (m_isRadialBlur) strTexture = TEXT("Target_RadialBlur");
	if (m_isInvertSaturationColor) strTexture = TEXT("Target_InvertLightnessColor");
	if (m_isInvertColor) strTexture = TEXT("Target_InvertColor");
	if (m_isVignette) strTexture = TEXT("Target_Vignette");

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(strTexture, m_pShader, "g_BackBufferTexture")))
		return;

	m_pShader->Begin(4);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

}


void CRenderer::Render_AdjustColor()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_fBrightness", &m_fAdjectBrightness, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_fContrast", &m_fAdjectContrast, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_fSaturation", &m_fAdjectSaturation, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_vTint", &m_vAdjectTint, sizeof(_float4))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_FinalResult"), m_pShader, "g_BackBufferTexture")))
		return;

	m_pShader->Begin(28);

	m_pVIBuffer->Render();
}

void CRenderer::Render_UI()
{
	for (auto& iter : m_RenderObject[RENDER_UI])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_UI].clear();
}

void CRenderer::Render_OcculusionDepth()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Occulusion"), m_pOcculusionDepthView)))
		return;

	m_RenderObject[RENDER_OCCULUSION].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return dynamic_cast<CGameObject*>(pSour)->Get_CamDistance() < dynamic_cast<CGameObject*>(pDest)->Get_CamDistance();
		});

	for (auto& pGameObject : m_RenderObject[RENDER_OCCULUSION])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_OcculusionDepth();
		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_OCCULUSION].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

#ifdef _DEBUG
void CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}

	m_DebugComponents.clear();
	
	if (!m_isDebugView) return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_NonBlend"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_CopyBackBuffer"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MTR_DeferredBlur"), m_pShader, m_pVIBuffer)))
		return;

	if (m_isShadow)
	{
		if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer)))
			return;
	}
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_Puddle"), m_pShader, m_pVIBuffer)))
		return;

	if (m_isSSAO)
	{
		if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_SSAO"), m_pShader, m_pVIBuffer)))
			return;
		if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_SSAOBlur"), m_pShader, m_pVIBuffer)))
			return;
	}

	if (m_isHDR)
	{
		if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_HDR"), m_pShader, m_pVIBuffer)))
			return;
		if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_Luminance"), m_pShader, m_pVIBuffer)))
			return;
		if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_CopyLuminance"), m_pShader, m_pVIBuffer)))
			return;
		if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_DownSampling"), m_pShader, m_pVIBuffer)))
			return;
	}

	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_Effect"), m_pShader, m_pVIBuffer)))
		return;
	//	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_Blur_X"), m_pShader, m_pVIBuffer)))
	//		return;
	//if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_Blur_Y"), m_pShader, m_pVIBuffer)))
	//	return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_Accum"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_RimLight"), m_pShader, m_pVIBuffer)))
		return;

	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_RimLight"), m_pShader, m_pVIBuffer)))
		return;


	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_RadialBlur"), m_pShader, m_pVIBuffer)))
		return;

	//MRT_DecalsContainDiffuse
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_DecalsContainDiffuse"), m_pShader, m_pVIBuffer)))
		return;
	//MRT_DecalsContainDiffuse
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_FinalResult"), m_pShader, m_pVIBuffer)))
		return;

	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_Occulusion"), m_pShader, m_pVIBuffer)))
		return;

	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_PBR"), m_pShader, m_pVIBuffer)))
		return;
}
#endif // DEBUG

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{

#ifdef _DEBUG
	for (auto& iter : m_DebugComponents)
		Safe_Release(iter);
	m_DebugComponents.clear();
#endif // _DEBUG

	for (int i = 0; i < RENDER_END; ++i)
	{
		for (auto& iter : m_RenderObject[i])
			Safe_Release(iter);

		m_RenderObject[i].clear();
	}

	Safe_Delete_Array(m_vSSAOKernal);

	Safe_Release(m_pOcculusionDepthView);

	Safe_Release(m_pLightDepthStencilView);
	Safe_Release(m_pSSAONoiseView);

	Safe_Release(m_pSSAOBuffer);
	Safe_Release(m_pPBRBuffer);

	Safe_Release(m_pShader);
	for(auto& pShader : m_pComputeShader)
		Safe_Release(pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}