#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBackground :
    public CGameObject
{
private:
    CBackground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBackground(const CBackground& rhs);
    virtual ~CBackground() = default;
    
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
    static CBackground* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

END