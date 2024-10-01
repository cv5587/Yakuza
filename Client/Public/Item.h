#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CTexture;
END

BEGIN(Client)

//Item : 플레이어가 잡고 상호작용하는 것들

class CItem final : public CGameObject
{
private:
	const _float BRIGHT_DISTANCE = 5.f;
	const _float THROW_TIME = 1.f;

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

	enum ITEM_MODE
	{
		ITEM_IDLE, // 그냥 평범
		ITEM_BRIGHT,
		ITEM_GRAB, // 잡았을때
		ITEM_DEAD, 
		ITEM_END
	};

	enum SHADER_NUM {
		SHADER_DEFAULT, //0
		SHADER_DEFAULT_BRIGHT, //1 -> BRIGHT 될때의 NONBELND
		SHADER_DISSOLVE, //2

		// 무조건 제일 아래
		SHADER_LIGHTDEPTH, //3
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
		int					iNaviNum;

		int					iDecalNum;
		DECAL_DESC_IO*		pDecal;

		int					iColliderNum;
		OBJCOLLIDER_DESC*	pColliderDesc;

		const _float4x4*	vPlayerMatrix;
		_float4x4			vOffsetMatrix;
	}MAPOBJ_DESC;

	typedef struct tThrowInfoDesc
	{
		_float4 vThrowDir;
		_float fThrowSpeed;
	}THROW_INFO_DESC;


private:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

	virtual void ImpulseResolution(CGameObject* pTargetObject, _float fDistance = 0.5f) override;
	virtual void Attack_Event(CGameObject* pHitObject, _bool isItem = false) override;

public:
	CTransform*			Get_Transform() { return m_pTransformCom; }

	// 붙이고 싶은 뼈의 행렬을 받아오는 함수
	void				Set_ParentMatrix(const _float4x4* vParentMatrix) { m_vParentMatrix = vParentMatrix;  }

	CCollider*			Get_AABBCollider() { return m_pColliderCom; }
	CCollider*			Get_OBBCollider() { return m_pOBBColliderCom; }

	ITEM_MODE			Get_ItemMode() { return m_eItemMode; }
	void				Set_ItemMode(CItem::ITEM_MODE mode) { m_eItemMode = mode; }
	void				Set_Grab(bool bGrab) { m_bCurGrab = bGrab;  m_eItemMode = ITEM_GRAB;}
	void				Attacking(_bool isOn = true) { m_isAttacking = isOn; }
	void				Set_Dissolve() { m_bDissovle = true; }

public:
	_uint				Get_ItemLife() { return m_iLife; }
	_bool				Decrease_Life();		// 사용횟수 감소 (사용횟수를 전부 소진한 상황이면 false를 리턴한다)
	void				Throw_On(THROW_INFO_DESC& Desc);

private:
	void				Throwing(const _float& fTimeDelta);

private:
	void				Set_Item_Mode();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };
	class CCollision_Manager* m_pCollisionManager = { nullptr };
	CCollider* m_pColliderCom = { nullptr }; //AABB 저장
	CCollider* m_pOBBColliderCom = { nullptr }; //AABB 저장
	CNavigation* m_pNavigationCom = { nullptr };

	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	_bool m_isFirst = { true };
	_bool m_isAttacking = { false };
	vector<CDecal*>			m_vDecals;
	//vector<CCollider*>		m_vColliders;
	int						m_iLayerNum;
	wstring					m_wstrModelName;
	int						m_iShaderPassNum = { 0 };
	int						m_iObjectType = { 0 };

	float					m_fBrightTime = { 0 };
	bool					m_bBright = { false };

	//bool					m_bRimLight;
	//_float2					m_fRimPartsUV;
	//float					m_isRimLight; // 0.f 꺼짐, 0.1 불한당, 0.2 러쉬, 0.3 파괴자 0.4 흰색 => 흰색 사용
	//int						m_iRimLightTime;

	_float4x4				m_vOffsetMatrix; // 위치 조정값
	const _float4x4*		m_vParentMatrix; // 부모 (뼈)
	const _float4x4*		m_pPlayerMatrix; // 플레이어 위치

	_float4x4				m_WorldMatrix;

	ITEM_MODE				m_eItemMode;

	bool					m_bCurGrab = { false }; // 현재 잡고있는지

	_int					m_iLife = { 3 };		// 적을 타격할 수 있는 횟수
	
	_bool					m_isThrowing = { false };
	_float					m_fThrowTimer = { 0.f };
	THROW_INFO_DESC			m_ThrowInfo{};


	_float					m_fDissolveTime = { 0 };
	_bool					m_bDissovle = { false };
	

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END