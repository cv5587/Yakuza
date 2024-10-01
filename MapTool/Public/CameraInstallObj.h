#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CCameraInstallObj final : public CGameObject
{
public:
	enum CAMERA_INSTALL_OBJ_E {
		CAM_FOCUS,
		CAM_EYE,
		CAM_END
	};


	typedef struct tCameraObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX				vStartPos;
		CAMERA_INSTALL_OBJ_E	eInstallDesc;
	}CAMERAINSTALL_DESC;

private:
	CCameraInstallObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraInstallObj(const CCameraInstallObj& rhs);
	virtual ~CCameraInstallObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }


private:
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };	
	CAMERA_INSTALL_OBJ_E		m_eInstallDesc;

public:
	HRESULT							Add_Components(void* pArg);
	HRESULT							Bind_ShaderResources();

public:
	static CCameraInstallObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

END