#pragma once

#include "SocketModel.h"

BEGIN(Engine)
class CModel;
class CAnim;
END

BEGIN(Client)

class CRcktGun : public CSocketModel
{
protected:
	CRcktGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRcktGun(const CRcktGun& rhs);
	virtual ~CRcktGun() = default;

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
	static CRcktGun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END