#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CNeoShader;
END

BEGIN(Client)
class CNPC abstract:
    public CLandObject
{
public:
    typedef struct tMapNPCObjDesc : public CLandObject::LANDOBJ_MAPDESC
    {

    }NPC_IODESC;

protected:
    CNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNPC(const CNPC& rhs);
    virtual ~CNPC() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    _bool m_isAnimLoop = { false };
    _uint m_iState = { 0 };
    _float m_fChangeInterval = { 4.f };
    _float m_fOffset = { 0.8f };

    _float          m_fPrevSpeed;
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;

protected:
    virtual void Change_Animation();
    virtual string Get_CurrentAnimationName() override;

protected:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;
    void Synchronize_Root(const _float& fTimeDelta);

public:
    virtual void Free();
};
END
