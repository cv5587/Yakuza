#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CCameraObj final : public CGameObject
{
public:


	typedef struct tCameraObjDescInput : public CGameObject::GAMEOBJECT_DESC
	{
		CAMERAOBJ_DESC		tObjDesc;
	}CAMERAOBJINPUT_DESC;

private:
	CCameraObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraObj(const CCameraObj& rhs);
	virtual ~CCameraObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CTransform*						Get_Transform() { return m_pTransformCom; }
	CAMERAOBJ_DESC					Get_CameraObjDesc() { return m_tCameraDesc; }
	void							Edit_CameraObjDesc(CAMERAOBJ_DESC cameraDesc) { m_tCameraDesc = cameraDesc; }

private:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CAMERAOBJ_DESC					m_tCameraDesc;

public:
	HRESULT							Add_Components(void* pArg);
	HRESULT							Bind_ShaderResources();

public:
	static CCameraObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

END