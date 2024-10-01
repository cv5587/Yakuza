#pragma once

#include "GameObject.h"
#include "Decal.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CConstruction final : public CGameObject
{
//public:
//	enum OBJECT_TYPE {
//		INTERACT_OBJ,
//		NONINTERACT_OBJ,
//		MONSTER,
//		OBJ_END
//	};

public:
	typedef struct tMapObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX			vStartPos;
		int					iLayer;
		wstring				wstrModelName;
		int					iShaderPass;
		int					iObjType;
		int					iObjPropertyType;


		int					iGroupMonster; //bool 역할
		int					iGroupNum;

		int					iNPCDirection;

		int					iNaviNum;
		int					iRouteNum;


		int					iDecalNum;
		DECAL_DESC_IO*		pDecal;

		int					iColliderNum;
		OBJCOLLIDER_DESC*	pColliderDesc;

		XMFLOAT4X4			vOffsetMatrix;
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

public:
	CTransform*										Get_Transform() { return m_pTransformCom; }
	CModel*											Get_Model() { return m_pModelCom; }
	vector<CDecal*>&								Get_Decals() { return m_vDecals; }
	vector<CCollider*>								Get_Colliders() { return m_vColliders; }
	OBJCOLLIDER_DESC								Get_ColliderDesc(int iIndex) { return m_vColliderDesc[iIndex]; }

public:
	HRESULT											Add_Components(void* pArg);
	HRESULT											Bind_ShaderResources();

public:
	int												Get_ObjPlaceDesc(OBJECTPLACE_DESC*	objplaceDesc);
	MAPOBJ_DESC										Get_MapObjDesc_For_AddList();

	/* 맵 로딩 후 배치된 게임오브젝트 수정을 위한 함수들 */
	void											Edit_GameObject_Information(CConstruction::MAPOBJ_DESC	mapDesc);
	CConstruction::MAPOBJ_DESC						Send_GameObject_Information();


	/* 맵툴에서만 필요한 기능 - Decal 관련*/
	void											On_Find_DecalMesh(int* iNumMesh, int iCnt);
	void											Off_Find_DecalMesh();

	void											Add_Decal(CDecal*	pDecal);
	void											Delete_Decal(int	iIndex);
	

	void											Add_Collider(OBJCOLLIDER_DESC tCollider);
	void											Rewrite_Collider(OBJCOLLIDER_DESC tCollider, int iIndex);
	void											Delete_Collider(int		iIndex);
	void											Delete_AllCollider();

private:
	CShader*										m_pShaderCom = { nullptr };
	CModel*											m_pModelCom = { nullptr };

private:
	vector<CDecal*>									m_vDecals;
	vector<CCollider*>								m_vColliders;
	vector<OBJCOLLIDER_DESC>						m_vColliderDesc;


	vector<int>										m_vRenderMeshIndexes;

	int												m_iLayerNum = { 0 };
	wstring											m_wstrModelName;
	int												m_iShaderPassNum = { 0 };
	int												m_iObjectType = { 0 };
	int												m_iObjectPropertyType = { 0 };
	int												m_iNaviNum = { -1 };
	int												m_iRouteNum = { 0 };
	int												m_iNPCDirection = { 0 };
	int												m_iGroupMonster = { 0 }; //bool 역할하는
	int												m_iGroupNum = { 0 };

	XMFLOAT4X4										m_vOffsetMatrix;

	int*											m_pDecalMeshIndex = { 0 };
	int												m_iDecalMeshCnt = { 0 };
	int												m_bFindDecalMesh = { false };

	bool											m_bOrigin = { false };

public:
	static CConstruction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END