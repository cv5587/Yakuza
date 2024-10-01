#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLightObj final : public CGameObject
{
public:
	typedef struct tLightObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX		vStartPos;
		LIGHT_DESC		tLightDesc;
	}LIGHTOBJ_DESC;

private:
	CLightObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightObj(const CLightObj& rhs);
	virtual ~CLightObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CTransform*						Get_Transform() { return m_pTransformCom; }
	void							Set_LightDesc(LIGHT_DESC lightDesc);
	LIGHT_DESC						Get_LightDesc() { return m_tLightDesc; }

private:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	LIGHT_DESC						m_tLightDesc;

public:
	HRESULT							Add_Components(void* pArg);
	HRESULT							Bind_ShaderResources();

public:
	static CLightObj*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END