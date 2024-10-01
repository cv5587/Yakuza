#pragma once
#include "VIBuffer_Instance.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Instance_Mesh final :
    public CVIBuffer_Instance
{
public:
    typedef struct tINSTANCE_MESH_DESC :public CVIBuffer_Instance::INSTANCE_DESC
    {
        class CMesh* pMeshs;
        INSTANCE_DESC* InstanceDesc ;
    }INSTANCE_MESH_DESC;
private:
    CVIBuffer_Instance_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Instance_Mesh(const CVIBuffer_Instance_Mesh& rhs);
    virtual ~CVIBuffer_Instance_Mesh() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    
private:
    _char				m_szName[MAX_PATH] = "";
    _uint				m_iMaterialIndex = { 0 };


public:
    static CVIBuffer_Instance_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};
END
