#pragma once


#include "GameObject.h"
#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNeoShader;
END

BEGIN(Client)

class CConstruction final : public CGameObject
{
public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* 그냥 건축물 */
		ROAD,
		ITEM,
		MONSTER_RUSH,
		PLAYER,
		SKY,
		LIGHT,
		MONSTER_WPA,
		MONSTER_SHAKEDOWN,
		MONSTER_YONEDA,
		MONSTER_KUZE,
		LARGE_CONSTRUCTION,
		OBJ_END
	};

	enum SHADER_TYPE
	{
		SHADER_IDLE,
		SHADER_GLASS,
		SHADER_WATER,
		SHADER_MASK,
		SHADER_CWPASS,
		SHADER_LIGHTDEPTH,
		SHADER_END
	};

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
	CConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConstruction(const CConstruction& rhs);
	virtual ~CConstruction() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	CTransform*					Get_Transform() { return m_pTransformCom; }

public:
	int							Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc);

	void						Edit_GameObject_Information(MAPOBJ_DESC	mapDesc);
	CConstruction::MAPOBJ_DESC	Send_GameObject_Information();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };
	CNeoShader* m_pMaterialCom = { nullptr };

private:
	_bool m_isFirst = { true };
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
	static CConstruction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END