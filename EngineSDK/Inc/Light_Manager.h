#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight_Manager :
    public CBase
{
private:
    CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLight_Manager() = default;

public:
    const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;

public:
    HRESULT Initialize();
    HRESULT Add_Light(const LIGHT_DESC& LightDesc);
    HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
    void    Edit_Light(int  iLightIndex, LIGHT_DESC lightDesc);
    void    Delete_Light(int iLightIndex);
    void    Delete_AllLights();
    HRESULT Bind_ComputeBuffer(_uint iSlot);
    
private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    vector<class CLight*> m_Lights;

public:
    static CLight_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END
