#include "ComputeShader.h"

#include"GameInstance.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CComputeShader::CComputeShader(const CComputeShader& rhs)
    : CComponent{ rhs },
    m_pComputeShader{ rhs.m_pComputeShader }
{
    Safe_AddRef(m_pComputeShader);
}

HRESULT CComputeShader::Initialize_Prototype(const wstring& strShaderFilePath)
{
    _uint iHlslFlag = 0;
#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL2;
#endif // DEBUG

    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS_Main", "cs_5_0", iHlslFlag, 0, &pShaderBlob, &pErrorBlob)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pComputeShader)))
    {
        Safe_Release(pShaderBlob);
        return E_FAIL;
    }

    Safe_Release(pShaderBlob);

    return S_OK;
}

HRESULT CComputeShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CComputeShader::Render(_uint iGroupX, _uint iGroupY, _uint iGroupZ)
{
    m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);

    m_pContext->Dispatch(iGroupX, iGroupY, iGroupZ);

    ID3D11UnorderedAccessView* pUAV = { nullptr };

    m_pContext->Flush();

    //ÃÊ±âÈ­
    m_pContext->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);

    return S_OK;
}

CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath)
{
    CComputeShader* pInstance = new CComputeShader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath)))
    {
        MSG_BOX("Failed To Created : CComputeShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CComputeShader::Clone(void* pArg)
{
    CComputeShader* pInstance = new CComputeShader(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CComputeShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CComputeShader::Free()
{
    __super::Free();

    Safe_Release(m_pComputeShader);
}