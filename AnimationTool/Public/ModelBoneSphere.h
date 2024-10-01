#pragma once

#include "GameObject.h"
#include "Component_Manager.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CModelBoneSphere final : public CGameObject
{
public:
	struct BoneSphereDesc : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pBoneWorldMatrix;
		const _float4x4* pModelWorldMatrix;
	};

private:
	CModelBoneSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelBoneSphere(const CModelBoneSphere& rhs);
	virtual ~CModelBoneSphere() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(const _float& fTimeDelta);
	virtual void Tick(const _float& fTimeDelta);
	virtual void Late_Tick(const _float& fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	void Change_TexutreIndex(_bool isOn = false);
	void Change_ColliderColor(_bool isOn = false);

	void Create_Collider(CCollider::TYPE eType, const CCollider::COLLIDER_DESC* pDesc);
	HRESULT Release_Collider();

public:
	CCollider::TYPE Get_Collider_Type();
	const _float3& Get_Collider_Center();
	void* Get_Collider_Value();

public:
	void Set_Collider_Center(const _float3& vCenter);
	void Set_Collider_Value(void* pDesc);

public:
	_bool Created_Collider();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_uint				m_iTextureIndex = { 0 };
	const _float4x4*	m_pModelWorldMatrix = { nullptr };
	const _float4x4*	m_pBoneWorldMatrix = { nullptr };

	_bool m_isEventPlaying = { false };

public:
	static CModelBoneSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
