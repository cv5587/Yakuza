#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CNeoShader :
    public CComponent
{
private:
    CNeoShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNeoShader(const CNeoShader& rhs);
    virtual ~CNeoShader() = default;

public:
    HRESULT Initialize_Prototype(const char* strFilePath);
    virtual HRESULT Initialize(void* pArg);
    HRESULT Bind_Shader(class CShader* pShader, const char* pMaterialName);

private:
    map<string, class CMaterial*> m_Materials;

private:
    HRESULT File_Open(const char* strFilePath);
    CMaterial* Fine_Materials(const char* pName);

public:
    static CNeoShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* strFilePath);
    virtual CComponent* Clone(void* pArg);
    virtual void Free();
};
END
