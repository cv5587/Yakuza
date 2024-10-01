#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Mesh;
class CModel;
END

class CParticle_Mesh :
	public CEffect
{
public:
	typedef struct tPARTICLE_MESH_DESC :public CEffect::EFFECT_DESC
	{
		CVIBuffer_Instance::INSTANCE_DESC BufferInstance;
		wstring strModelTag;
	}PARTICLE_MESH_DESC;

private:
	CParticle_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Mesh(const CParticle_Mesh& rhs);
	virtual ~CParticle_Mesh() = default;
public:
	virtual void* Get_Instance()override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(string strFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta)override;
	virtual void Tick(const _float& fTimeDelta)override;
	virtual void Late_Tick(const _float& fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	wstring Get_ModelTag() { return m_strModelTag; }
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[2] = {nullptr};
	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Instance_Mesh* m_pVIBufferCom = { nullptr };

	CVIBuffer_Instance::INSTANCE_DESC m_BufferInstance;
	wstring m_strModelTag = { TEXT("") };
	_float       m_fCurTime = { 0.f };
public:
	virtual HRESULT Save_Data(const string strDirectory)override;
	virtual HRESULT Load_Data(const string strDirectory)override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CParticle_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CParticle_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

