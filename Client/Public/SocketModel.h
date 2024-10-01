#pragma once

#include "SocketObject.h"

BEGIN(Engine)
class CModel;
class CAnim;
END

BEGIN(Client)

class CSocketModel abstract : public CSocketObject
{
protected:
	CSocketModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSocketModel(const CSocketModel& rhs);
	virtual ~CSocketModel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Bind_ResourceData() = 0;

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	wstring m_wstrModelName = TEXT("");

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END