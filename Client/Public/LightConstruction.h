#pragma once


#include "GameObject.h"
#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLightConstruction final : public CGameObject
{

public:
	typedef struct tMapObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX			vStartPos;
		int					iLayer;
		wstring				wstrModelName;
		int					iShaderPass;
		int					iObjType;

		int					iDecalNum;
		DECAL_DESC_IO*		pDecal;

		int					iColliderNum;
		OBJCOLLIDER_DESC*	pColliderDesc;
	}MAPOBJ_DESC;


private:
	CLightConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightConstruction(const CLightConstruction& rhs);
	virtual ~CLightConstruction() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };

private:
	_bool					m_isFirst = { true };
	vector<CDecal*>			m_vDecals;
	vector<CCollider*>		m_vColliders;
	int						m_iLayerNum;
	wstring					m_wstrModelName;
	int						m_iShaderPassNum = { 0 };
	int						m_iObjectType = { 0 };
	float					m_fWaterDeltaTime = { 0 };

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CLightConstruction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END