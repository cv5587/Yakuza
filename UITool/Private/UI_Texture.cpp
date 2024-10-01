#include "UI_Texture.h"

#include "GameInstance.h"

CUI_Texture::CUI_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Object { pDevice, pContext}
{
}

CUI_Texture::CUI_Texture(const CUI_Texture& rhs)
	: CUI_Object {rhs },
	m_fX{rhs.m_fX},
	m_fY{rhs.m_fY },
	m_fSizeX{rhs.m_fSizeX },
	m_fSizeY{rhs.m_fSizeY },
	m_WorldMatrix{rhs.m_WorldMatrix },
	m_ViewMatrix{rhs.m_ViewMatrix },
	m_ProjMatrix{rhs.m_ProjMatrix },
	m_strParentName{rhs.m_strParentName },
	m_fStartUV{rhs.m_fStartUV },
	m_fEndUV{rhs.m_fEndUV },
	m_isColor{rhs.m_isColor},
	m_vColor{rhs.m_vColor },
	m_isAnim{rhs.m_isAnim},
	m_fAnimTime{rhs.m_fAnimTime },
	m_vStartPos{rhs.m_vStartPos },
	m_vAnimScale{rhs.m_vAnimScale},
	m_iShaderPass{rhs.m_iShaderPass },
	m_fControlAlpha{rhs.m_fControlAlpha },
	m_isReverse{rhs.m_isReverse },
	m_isAnimLoop{ rhs.m_isAnimLoop },
	m_fUpPoint{ rhs.m_fUpPoint },
	m_fDownPoint{ rhs.m_fDownPoint },
	m_vEndColor{rhs.m_vEndColor }
{
}

HRESULT CUI_Texture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Texture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		UI_TEXTURE_DESC* pDesc = static_cast<UI_TEXTURE_DESC*>(pArg);

		m_strTextureFilePath = pDesc->strTextureFilePath;
		m_strTextureName = pDesc->strTextureFileName;

		m_isParent = pDesc->isParent;
		m_pParentMatrix = pDesc->pParentMatrix;
		m_strParentName = pDesc->strParentName;

		m_WorldMatrix = pDesc->WorldMatrix;

		m_fStartUV = pDesc->fStartUV;
		m_fEndUV = pDesc->fEndUV;
		m_isColor = pDesc->isColor;
		m_vColor = pDesc->vColor;

		m_iShaderPass = pDesc->iShaderPass;

		m_isAnimLoop = pDesc->isAnimLoop;
		m_fAnimTime = pDesc->fAnimTime;
		m_vStartPos = pDesc->vStartPos;
		m_isAnim = pDesc->bAnim;

		m_fControlAlpha = pDesc->fControlAlpha;
		m_isReverse = pDesc->isReverse;

		m_fUpPoint = pDesc->vUpPoint;
		m_fDownPoint = pDesc->vDownPoint;
		m_vEndColor = pDesc->vEndColor;

		m_vAnimScale = pDesc->vAnimScale;

		m_fSizeX = g_iWinSizeX;
		m_fSizeY = g_iWinSizeY;
		m_fX = g_iWinSizeX >> 1;
		m_fY = g_iWinSizeY >> 1;

		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.01f, 1.f));


		if (pDesc->isLoad)
		{
			m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->WorldMatrix));
		}
	}
	//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.0f));
	
	return S_OK;
}

void CUI_Texture::Priority_Tick(const _float& fTimeDelta)
{
}

void CUI_Texture::Tick(const _float& fTimeDelta)
{
#ifdef _TOOL
	if(m_isPlay)
	{
		if (m_isAnim && !m_isReverse)
			m_fAnimTime.x += fTimeDelta;
		else if (m_isAnim  && m_isReverse)
			m_fAnimTime.x -= fTimeDelta;
	}

#else

	if (m_isPlay)
	{
		if (m_isAnim && !m_isReverse)
			m_fAnimTime.x += fTimeDelta;
		else if (m_isAnim && m_isReverse)
			m_fAnimTime.x -= fTimeDelta;
	}

#endif // _TOOL

	//if (m_isAnim)
	//{
	//	_float factor = m_fAnimTime.x / m_fAnimTime.y;
	//	m_pGameInstance->
	//}

}

void CUI_Texture::Late_Tick(const _float& fTimeDelta)
{
	if (m_isPlay)
	{
		if (!m_isReverse)
		{
			if (m_fAnimTime.x >= m_fAnimTime.y)
			{

				m_fAnimTime.x = m_fAnimTime.y;
				if (m_isAnimLoop)
					m_isReverse = true;
			}
		}
		else
		{
			if (0.f >= m_fAnimTime.x)
			{

				m_fAnimTime.x = 0.f;
				if (m_isAnimLoop)
				{
					m_isReverse = false;
				}
			}
		}

	}

	
#ifndef _TOOL
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_UI, this);
#endif // _TOOL


}

HRESULT CUI_Texture::Render()
{
	return S_OK;
}

HRESULT CUI_Texture::Change_UV(_float2 fStartUV, _float2 fEndUV)
{
	m_fStartUV = fStartUV;
	m_fEndUV = fEndUV;

	if (FAILED(m_pVIBufferCom->EditUV(m_fStartUV, m_fEndUV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Texture::Change_Point(_float4 vUpPoint, _float4 vDownPotint)
{

	m_fUpPoint = vUpPoint;
	m_fDownPoint = vDownPotint;

	if (FAILED(m_pVIBufferCom->Edit_Point(m_fUpPoint, m_fDownPoint)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Texture::Show_UI()
{
	m_fAnimTime.x = 0.f;
	m_isReverse = false;
	return S_OK;
}

HRESULT CUI_Texture::Close_UI()
{
	m_fAnimTime.x = m_fAnimTime.y;
	m_isReverse = true;
	return S_OK;
}

HRESULT CUI_Texture::Save_binary(const string strDirectory)
{
	return E_FAIL;
}

HRESULT CUI_Texture::Save_Groupbinary( ofstream& out)
{
	return E_FAIL;
}

HRESULT CUI_Texture::Load_binary(ifstream& in)
{
	return E_FAIL;
}

HRESULT CUI_Texture::Add_Components()
{
	CVIBuffer_Rect::RECT_DESC Desc;

	Desc.fStartUV = m_fStartUV;
	Desc.fEndUV = m_fEndUV;
	Desc.vUpPoint = m_fUpPoint;
	Desc.vDownPoint = m_fDownPoint;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&Desc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, m_strTextureFilePath, 1);
	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Texture::Bind_ResourceData()
{
	

	if (m_isParent)
	{
		_float4x4 ResultWorld;
		XMStoreFloat4x4(&ResultWorld, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ResultWorld)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}

	if (m_isAnim)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartPos", &m_vStartPos, sizeof(_float3))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_AnimScale", &m_vAnimScale, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimTime", &m_fAnimTime, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fControlAlpha", &m_fControlAlpha, sizeof(_float2))))
			return E_FAIL;

	}

	if(m_isColor)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &m_vEndColor, sizeof(_float4))))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
}

void CUI_Texture::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
