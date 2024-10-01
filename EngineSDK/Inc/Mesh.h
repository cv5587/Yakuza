#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	HRESULT Initialize_Prototype(CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones, _bool isTool);
	HRESULT Initialize_Prototype(CModel::MODELTYPE eModelType, const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones, _bool isTool);
	HRESULT Initialize();
	HRESULT Render(_bool isTool);

	HRESULT Ready_Vertices_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_NonAnimMesh(const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_ParticleMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_ParticleMesh(const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_AnimMesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);
	HRESULT Ready_Vertices_For_AnimMesh(const BAiMesh* pAIMesh, const vector<class CBone*>& Bones);

	void	Fill_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices);
	void Bind_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices);

public:
	_uint Get_MaterialIndex() { return m_iMaterialIndex; }
	_uint Get_NumBones() { return m_iNumBones; }

	VTXMESH* Get_Vertices() { return m_pVertices; }
	_uint* Get_Mesh_Indices() { return m_pIndices; }

	const _char* Get_Name() const {
		return m_szName;
	}

	const _bool& Get_AlphaApply() const {
		return m_isAlphaApply;
	}

	_float4x4	Get_LocalMatrix() { return m_localMatrix; }
	_float		Get_MeshScale() { return m_fScale; }
	_float3		Get_MaxPoints() { return m_vMaxPosition; }
	_float3		Get_MinPoints() { return m_vMinPosition; }

public:
	void Set_AlphaApply(_bool isValue) { m_isAlphaApply = isValue; }

private:
	_char				m_szName[MAX_PATH] = "";
	_uint m_iModelType = { CModel::TYPE_END };

	_bool				m_isAlphaApply = { false };

	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 }; /* 이 메시에 영향을 주는 뼈의 갯수 */

	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;

	_float4x4			m_localMatrix;
	_float				m_fScale;

	VTXMESH* m_pVertices = { nullptr };
	_uint* m_pIndices = { nullptr };

	ID3D11Buffer* m_pBoneMatrixBuffer = { nullptr };

	// Occulusion을 위해 Model 자체의 대략적인 크기 파악을 위해
	_float3				m_vMinPosition;
	_float3				m_vMaxPosition;

private:
	_bool m_isTool = { false }; // 메쉬 생성 시 툴인지 아닌지 확인하기 위한 변수. 복사 하지 않음.

private:
	HRESULT Ready_Buffer();

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones, _bool isTool);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const BAiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones, _bool isTool);
	CMesh* Clone();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END