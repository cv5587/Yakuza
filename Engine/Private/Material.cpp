#include "Material.h"

#include "Shader.h"

CMaterial::CMaterial()
	: CBase()
{
}

HRESULT CMaterial::Initialize(METERIAL_DESC* pDesc)
{
	memcpy(&m_Info, pDesc, sizeof(METERIAL_DESC));

	return S_OK;
}

HRESULT CMaterial::Bind_Shader(CShader* pShader)
{
	if (FAILED(pShader->Bind_RawValue("Opacity", &m_Info.fOpacity, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("fSpecularPower", &m_Info.fSpecularPower, sizeof(_float))))
		return E_FAIL;
	_vector vSpecularColor = XMLoadFloat4(&m_Info.vSpecularColor);
	if (FAILED(pShader->Bind_RawValue("vSpecularColor", &vSpecularColor, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("AssetShader", &m_Info.fAssetShader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("DisableRDRT", &m_Info.fDisableRDRS, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("Engine", &m_Info.fEngine, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("IsOEClothShader", &m_Info.fIsClothShader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("SkinShader", &m_Info.fSkinShader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("Rough", &m_Info.fRough, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("IsY3Shader", &m_Info.fIsY3Shader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("Imperfection", &m_Info.Imperfection, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("SPShader", &m_Info.SPShader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_isUVShader", &m_Info.isUVShader, sizeof(_bool))))
		return E_FAIL;

	if (m_Info.isUVShader)
	{
		if (FAILED(pShader->Bind_RawValue("g_fRTX", &m_Info.fRTX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fRTY", &m_Info.fRTY, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fRDRMRS_X", &m_Info.fRDRMRS_X, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fRDRMRS_Y", &m_Info.fRDRMRS_Y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("f_fImperfection_UV", &m_Info.fImperfection_UV, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("f_fEngine_UV", &m_Info.fEngine_UV, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

CMaterial* CMaterial::Create(METERIAL_DESC* pDesc)
{
	CMaterial* pInstance = new CMaterial();
	
	if (FAILED(pInstance->Initialize(pDesc)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMaterial::Free()
{
}
