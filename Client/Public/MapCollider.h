#pragma once

#include "GameObject.h"
#include "MapColliderObj.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CMapCollider final : public CGameObject
{
private:
	CMapCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapCollider(const CMapCollider& rhs);
	virtual ~CMapCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	const vector<CCollider*>& Get_Colliders(){
		return m_vCollider;
	}


private:
	class CCollision_Manager* m_pCollisionManager = { nullptr };

	_matrix							m_WorldMatrix;
	vector<CCollider*>				m_vCollider;

	vector<CMapColliderObj*>		m_ColliderObjs;

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CMapCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END