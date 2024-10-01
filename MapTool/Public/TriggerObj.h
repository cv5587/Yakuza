#pragma once

#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

class CTriggerObj : public CGameObject
{
public:
	typedef struct tTriggerObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX			vStartPos;
		TRIGGER_DESC		tTriggerDesc;
	}TRIGGEROBJ_DESC;


private:
	CTriggerObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerObj(const CTriggerObj& rhs);
	virtual ~CTriggerObj() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Priority_Tick(const _float& fTimeDelta) override;
	virtual void					Tick(const _float& fTimeDelta) override;
	virtual void					Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	CTransform*						Get_Transform() { return m_pTransformCom; }
	void							Set_TriggerDesc(TRIGGER_DESC triggerDesc);
	TRIGGER_DESC					Get_TriggerDesc();


private:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };

	TRIGGER_DESC					m_tTriggerDesc;

public:
	HRESULT							Add_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CTriggerObj*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

