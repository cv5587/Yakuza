#pragma once
#include "Monster.h"


BEGIN(Client)
class CRushYakuza :
    public CMonster
{

private:
    CRushYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRushYakuza(const CRushYakuza& rhs);
    virtual ~CRushYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;

private:
    _float          m_fPrevSpeed = { 0.f };
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;


private:
    virtual HRESULT Add_Components() override;
    virtual void Change_Animation();


public:
    static CRushYakuza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
