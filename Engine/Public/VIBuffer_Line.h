#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Line :
    public CVIBuffer
{
private:
    CVIBuffer_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Line(const CVIBuffer_Line& rhs);
    virtual ~CVIBuffer_Line() = default;

public:
    virtual HRESULT Initialize_Prototype(list<class CCell*> Cells);
    virtual HRESULT Initialize_Prototype(vector<ROUTE_IO>    routes);
    virtual HRESULT Initialize(void* pArg) override;

public:
    static CVIBuffer_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , list<class CCell*> Cells);
    static CVIBuffer_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<ROUTE_IO>    routes);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};
END

