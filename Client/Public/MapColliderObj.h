#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CModel;
class CShader;
END

BEGIN(Client)

// 테스트용. 테스트 후엔 생성안할예정

class CMapColliderObj final : public CGameObject
{
public:
	typedef struct tColliderObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3			vCenter;
		_float3			vQuaternion;
		_float3			vExtents;
	}COLLIDEROBJ_DESC;

private:
	CMapColliderObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapColliderObj(const CMapColliderObj& rhs);
	virtual ~CMapColliderObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CMapColliderObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END