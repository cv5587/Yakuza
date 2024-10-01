#pragma once

#include "GameObject.h"
#include "GameInstance.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CTrigger abstract : public CGameObject
{
public:
	typedef struct tTriggerDesc : public CGameObject::GAMEOBJECT_DESC
	{
		TRIGGER_DESC		tTriggerDesc;
	}TRIGGEROBJ_DESC;

protected:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual bool				Move_Scene(int& iLevelNum);

protected:
	CShader*						m_pShaderCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };

protected:
	_matrix							m_WorldMatrix;
	TRIGGER_DESC					m_tTriggerDesc;

	bool							m_bMoveScene = { false };
	bool							m_bTriggerDead = { false };

protected:
	virtual HRESULT Add_Components(void* pArg);

public:
	virtual void Free() override;
};

END