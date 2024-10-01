#pragma once

#include "SocketModel.h"

BEGIN(Engine)
class CModel;
class CAnim;
END

BEGIN(Client)

class CGun_Cz75 : public CSocketModel
{
public:
	struct CZ75_DESC : public SOCKETOBJECT_DESC {
		_float fLocalAngle;
		_float3 vLocalPos;
		_uint iLocalRotAxis;			//R U L
	};


protected:
	CGun_Cz75(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGun_Cz75(const CGun_Cz75& rhs);
	virtual ~CGun_Cz75() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Bind_ResourceData() override;

public:
	static CGun_Cz75* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END