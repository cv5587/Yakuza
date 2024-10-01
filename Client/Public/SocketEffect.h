#pragma once


#include "SocketObject.h"
#include "CharacterData.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CSocketEffect final : public CSocketObject
{
public:
	struct SOKET_EFFECT_DESC : public SOCKETOBJECT_DESC
	{
		wstring wstrEffectName;
	};

private:
	CSocketEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSocketEffect(const CSocketEffect& rhs);
	virtual ~CSocketEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void On();
	void Off() {
		m_isOn = false;
	}

	wstring Get_EffectName() {
		return m_wstrEffectName;
	}

private:
	class CEffect*				m_pEffect = { nullptr };
	wstring						m_wstrEffectName = TEXT("");

	_bool						m_isOn = { true };

public:
	HRESULT Add_Components(void* pArg);

public:
	static CSocketEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END