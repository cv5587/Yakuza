#pragma once

#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CColliderObj final : public CGameObject
{
public:
	typedef struct tColliderObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX		vStartPos;
		bool			bLoad;
		_float3			vCenter;
		_float3			vQuaternion;
		_float3			vExtents;
	}COLLIDEROBJ_DESC;

private:
	CColliderObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CColliderObj(const CColliderObj& rhs);
	virtual ~CColliderObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	CTransform*						Get_Transform() { return m_pTransformCom; }
	void							Set_ColliderDesc(COLLIDER_DESC lightDesc);
	COLLIDER_DESC					Get_ColliderDesc();
	XMFLOAT4X4*						Get_ColliderWorldMatrix() { return &m_ColliderWorldMatrix; }


private:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };
	COLLIDER_DESC					m_tColliderDesc;

	bool							m_bOrigin = { false };

	_matrix							m_WorldMatrix;
	XMFLOAT4X4						m_ColliderWorldMatrix;

public:
	HRESULT							Add_Components(void* pArg);
	HRESULT							Bind_ShaderResources();

public:
	static CColliderObj*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END