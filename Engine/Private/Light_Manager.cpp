#include "Light_Manager.h"

#include "Light.h"

CLight_Manager::CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice},
	m_pContext{ pContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	if (iIndex >= m_Lights.size())
		return nullptr;

	auto iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{
	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
	CLight* pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer);

	return S_OK;
}

void CLight_Manager::Edit_Light(int iLightIndex, LIGHT_DESC lightDesc)
{
	m_Lights[iLightIndex]->Set_LightDesc(lightDesc);
}

void CLight_Manager::Delete_Light(int iLightIndex)
{
	vector<CLight*>::iterator	iter = m_Lights.begin();

	for (int i = 0; i < iLightIndex; i++)
	{
		iter++;
	}

	Safe_Release(*iter);
	m_Lights.erase(iter);

}

void CLight_Manager::Delete_AllLights()
{
	for (auto& iter : m_Lights)
		Safe_Release(iter);
	m_Lights.clear();
}

HRESULT CLight_Manager::Bind_ComputeBuffer(_uint iSlot)
{
	if (m_Lights.size() <= 0)
		return E_FAIL;

	return m_Lights[0]->Bind_LightBuffer(iSlot); // 0번이 디렉션이라고 가정한다.
}

CLight_Manager* CLight_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight_Manager* pInstance = new CLight_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLight_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Manager::Free()
{
	for (auto& iter : m_Lights)
		Safe_Release(iter);
	m_Lights.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
