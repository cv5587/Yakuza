#pragma once
#include "UI_Texture.h"
#include "UITool_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(UITool)
class CHeadUI :
	public CUI_Texture
{
public:
	typedef struct tHeadUI_Desc : public CUI_Texture::UI_TEXTURE_DESC
	{

	}HEADUI_DESC;
private:
	CHeadUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHeadUI(const CHeadUI& rhs);
	virtual ~CHeadUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	HRESULT Initialize_Prototype(ifstream& in);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();
public:
	virtual HRESULT Save_binary(const string strDirectory)override;
	virtual HRESULT Save_Groupbinary(ofstream& out)override;
	virtual HRESULT Load_binary(ifstream& in)override;
public:
	static CHeadUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CHeadUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
