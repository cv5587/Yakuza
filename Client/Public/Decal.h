#pragma once

#include "GameObject.h"

#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CVIBuffer_Cube;
class CCollider;
END

BEGIN(Client)

class CDecal final :
    public CGameObject
{
public:
    typedef struct tDecalObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        int         iMaterialNum;
        CTexture* pTexture;
        XMMATRIX    vStartPos;
    }DECALOBJ_DESC;

protected:
    CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDecal(const CDecal& rhs);
    virtual ~CDecal() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;


private:
    CTexture*       m_pTextureCom = { nullptr };
    CShader*        m_pShaderCubeCom = { nullptr };
    CVIBuffer_Cube* m_pVIBufferCubeCom = { nullptr };

private:
    int                 m_iMaterialNum;

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

END