#pragma once


#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CPartObject abstract : public CGameObject
{
public:
	struct PARTOBJ_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4*		pParentMatrix;
	};

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	const _float4x4* Get_PartWorldMatrix() const {
		return &m_WorldMatrix;
	}

protected:
	const _float4x4*		m_pParentMatrix = { nullptr };		// 부모로 가지는 객체의 행렬

	_float4x4				m_WorldMatrix;						// 본인 객체의 행렬

public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END