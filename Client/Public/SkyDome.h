#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CSkyDome final : public CGameObject
{
public:
	typedef struct tSkyObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX			vStartPos;
		wstring				wstrModelName;
	}SkyObj_Desc;

private:
	CSkyDome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkyDome(const CSkyDome& rhs);
	virtual ~CSkyDome() = default;

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
	static CSkyDome* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END