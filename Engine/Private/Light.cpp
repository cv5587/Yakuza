#include "Light.h"

#include "GameInstance.h"


CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice},
	m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	if (m_LightDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL)
		Ready_Buffer();

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint iShaderIndex = { 0 };

	if (m_LightDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL) //방향광원은 어떤 방향으로 빛을 보낼것인지에 대한 방향벡터가 필요하다.
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

		iShaderIndex = 1;
	}
	else if (m_LightDesc.eType == LIGHT_DESC::TYPE_POINT) // 점광원은 점의 위치와 범위가 필요하다. (방향은 필요하지 않다)
	{
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;

		iShaderIndex = 2;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iShaderIndex);
	pVIBuffer->Render();

	return S_OK;
}

HRESULT CLight::Bind_LightBuffer(_uint iSlot)
{
	m_pContext->CSSetConstantBuffers(iSlot, 1, &m_pLightBuffer);
	return S_OK;
}

void CLight::Ready_Buffer()
{
	LIGHT_BUFFER Buffer{};
	Buffer.vDirection = m_LightDesc.vDirection;
	Buffer.vDiffuse = m_LightDesc.vDiffuse;
	Buffer.vAmbient = m_LightDesc.vAmbient;
	Buffer.vSpecular = m_LightDesc.vSpecular;

	D3D11_BUFFER_DESC Desc{};
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.ByteWidth = sizeof(LIGHT_BUFFER);
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.StructureByteStride = sizeof(LIGHT_BUFFER);

	if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pLightBuffer)))
		return;

	m_pContext->UpdateSubresource(m_pLightBuffer, 0, nullptr, &Buffer, 0, 0);
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight(pDevice,	pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pLightBuffer);
}
