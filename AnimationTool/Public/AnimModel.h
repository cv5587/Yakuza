#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CAnimation;
class CAnim;
class CBone;
class CMesh;
END

BEGIN(Client)

class CAnimModel final : public CGameObject
{
public:
	struct ANIMMODEL_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_matrix matWorldMatrix;
	};

	struct Trail_Desc
	{
		class CEffect* pTrail;
		_bool isOn = { false };
	};

private:
	CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimModel(const CAnimModel& rhs);
	virtual ~CAnimModel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Priority_Tick(const _float& fTimeDelta);
	virtual void Tick(const _float& fTimeDelta);
	virtual void Late_Tick(const _float& fTimeDelta);

	virtual HRESULT Render();

public:
	void Set_Position(_fvector vPosition);
	void Set_Scaled(_float x, _float y, _float z);
	void Set_Rotation(_uint iAxis, _float vRadian, _float fTimeDelta);

	void Trail_On(string& strBoneName, _bool isOn);

public:
	const vector<CAnimation*>& Get_Animations();
	const vector<CBone*>& Get_Bones();
	const vector<CMesh*>& Get_Meshes();
	const vector<class CModelBoneSphere*>& Get_BoneSpheres() {
		return m_BoneSpheres;
	}

	_bool Get_AnimLoop(_uint iAnimIndex);
	CModel* Get_Model() {
		return m_pModelCom;
	}

	CAnim* Get_AnimComponent() {
		return m_pAnimCom;
	}


public:
	void Change_Model(wstring strModelName, wstring strAnimName = TEXT(""));
	void Change_Animation(_uint iAnimIndex, _bool isMonster = false);
	void Select_Bone(_uint iBoneIndex);
	void Select_Mesh(_uint iMeshIndex) {
		m_iSelectedMeshIndex = iMeshIndex;
	}

	void Create_BoneCollider(_uint iType, _uint iIndex, const _float3& vCenter, void* pDesc);
	void Create_Trail(string& strBoneName, CGameObject* pEffect);

	void Set_Collider_Center(_uint iIndex, const _float3& vCenter);
	void Set_Collider_Value(_uint iIndex, void* pDesc);

	void Set_RimMeshName(string strMeshName) {
		m_strRimMeshName = strMeshName;
	}

public:
	_bool Created_BoneCollider(_uint iIndex);

	HRESULT Release_BoneCollider(_uint iIndex);

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();
	void	Ready_BoneSphere();

private:
	CAnim* m_pAnimCom = { nullptr }; // 애니메이션만 따로 저장하고있는 애니메이션 컴포넌트
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	vector<class CModelBoneSphere*> m_BoneSpheres;

	wstring m_strModelName;

	_bool	m_isMonster = { false };
	_uint	m_iAnimIndex = { 0 };
	_uint	m_iPassIndex = { 0 };
	_uint	m_iSelectedMeshIndex = { 0 };

	_float4     m_vPrevMove;
	_float4x4   m_ModelWorldMatrix;

	// 잘 안보여서 그냥 러쉬색상으로함
	_float m_isRimLight = 0.2f;// 0.f 꺼짐, 0.1 불한당, 0.2 러쉬, 0.3 파괴자

	_float2 m_fRimTopUV = { 0.0f , 0.3f };//0~1사이 [시작v,끝v]상의
	_float2 m_fRimBotUV = { 0.9f, 1.0f };//0~1사이 [시작v,끝v]하의
	_float2 m_fRimPartsUV = { 0.0f, 1.0f }; //0~1사이 [시작v,끝v]손,발

	string m_strRimMeshName = "";

	// first: 뼈 이름, second: 이펙트
	map<string, Trail_Desc>		m_TrailEffects;

public:
	static CAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
