#include "NeoShader.h"

#include "Material.h"
#include "Shader.h"

CNeoShader::CNeoShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{pDevice, pContext}
{
}

CNeoShader::CNeoShader(const CNeoShader& rhs)
	: CComponent{rhs},
	m_Materials{ rhs.m_Materials}
{
	for (auto& Pair : m_Materials)
		Safe_AddRef(Pair.second);
}

HRESULT CNeoShader::Initialize_Prototype(const char* strFilePath)
{
	if (FAILED(File_Open(strFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNeoShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CNeoShader::Bind_Shader(CShader* pShader, const char* pMaterialName)
{
	CMaterial* pMeterial = Fine_Materials(pMaterialName);

	if (nullptr == pMeterial)
		return E_FAIL;

	return pMeterial->Bind_Shader(pShader);
}

HRESULT CNeoShader::File_Open(const char* strFilePath)
{
	ifstream in(strFilePath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}
	//머테리얼 개수
	_int iNumMeterial = 0;
	in.read((char*)&iNumMeterial, sizeof(_int));

	for (size_t i = 0; i < iNumMeterial; ++i)
	{
		CMaterial::METERIAL_DESC Desc{};

		// 머테리얼 정보 읽기
		char strMeterialName[MAX_PATH] = "";
		_int iNumMeterialName = 0;
		in.read((char*)&iNumMeterialName, sizeof(_int));
		in.read(strMeterialName, iNumMeterialName);
		memcpy(Desc.strNeterialName, strMeterialName, sizeof(strMeterialName));


		in.read((char*)&Desc.fOpacity, sizeof(_float));
		in.read((char*)&Desc.vSpecularColor.x, sizeof(_float));
		in.read((char*)&Desc.vSpecularColor.y, sizeof(_float));
		in.read((char*)&Desc.vSpecularColor.z, sizeof(_float));
		in.read((char*)&Desc.vSpecularColor.w, sizeof(_float));
		in.read((char*)&Desc.fSpecularPower, sizeof(_float));
		in.read((char*)&Desc.fAssetShader, sizeof(_float));
		in.read((char*)&Desc.fDisableRDRS, sizeof(_float));
		in.read((char*)&Desc.fEngine, sizeof(_float));
		in.read((char*)&Desc.fIsClothShader, sizeof(_float));
		in.read((char*)&Desc.fSkinShader, sizeof(_float));
		in.read((char*)&Desc.fRough, sizeof(_float));
		in.read((char*)&Desc.fIsY3Shader, sizeof(_float));
		in.read((char*)&Desc.Imperfection, sizeof(_float));
		in.read((char*)&Desc.SPShader, sizeof(_float));

		in.read((char*)&Desc.isUVShader, sizeof(_bool));

		if (Desc.isUVShader)
		{
			in.read((char*)&Desc.fRTX, sizeof(_float));
			in.read((char*)&Desc.fRTY, sizeof(_float));
			in.read((char*)&Desc.fRDRMRS_X, sizeof(_float));
			in.read((char*)&Desc.fRDRMRS_Y, sizeof(_float));
			in.read((char*)&Desc.fImperfection_UV, sizeof(_float));
			in.read((char*)&Desc.fEngine_UV, sizeof(_float));
		}

		CMaterial* pMeterial = CMaterial::Create(&Desc);
		if (nullptr == pMeterial)
			return E_FAIL;

		m_Materials.emplace(Desc.strNeterialName, pMeterial);
	}
	in.close();

	return S_OK;
}

CMaterial* CNeoShader::Fine_Materials(const char* pName)
{
	auto pPair = m_Materials.find(pName);

	if (pPair == m_Materials.end())
		return nullptr;

	return pPair->second;
}

CNeoShader* CNeoShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* strFilePath)
{
	CNeoShader* pInstance = new CNeoShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* CNeoShader::Clone(void* pArg)
{
	CNeoShader* pInstance = new CNeoShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("머테리얼 생성 실패");
		Safe_Release(pInstance);
	}
		

	return pInstance;
}

void CNeoShader::Free()
{
	__super::Free();

	for (auto& Pair : m_Materials)
		Safe_Release(Pair.second);
	m_Materials.clear();
}
