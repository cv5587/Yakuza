#pragma once
#include "GameObject.h"]
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLand final :
    public CGameObject
{
private:
	CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLand(const CLand& rhs);
	virtual ~CLand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta)override;
	virtual void Tick(const _float& fTimeDelta)override;
	virtual void Late_Tick(const _float& fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = {nullptr};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END