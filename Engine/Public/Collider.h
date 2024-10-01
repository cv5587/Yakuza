#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CCollider :
    public CComponent
{
public:
    enum TYPE { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };

    typedef struct tColliderDesc {
        TYPE eType;
    }COLLIDER_DESC;

private:
    CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCollider(const CCollider& rhs);
    virtual ~CCollider() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_fmatrix WorldMatirx);

public:
    TYPE Get_Type() { return m_ColliderType; }       //0625 Çý¿ø Ãß°¡
    void* Get_Desc();       //0625 Çý¿ø Ãß°¡
    void* Get_OriginDesc();       //0625 Çý¿ø Ãß°¡
    const _float3& Get_Center();

public:
    void Set_Value(void* pDesc);
    void Set_Center(const _float3& vCenter);
    void Set_Rotation(const _float3& vRotation);

#ifdef _DEBUG
    void Set_Color(const _float4& vColor);
    void Reset_Color();
#endif // _DEBUG

public:
    _bool Intersect(CCollider* pTargetCollider, _float fDistance = 0.5f);
    const _float3& ImpulseResolution(CCollider* pTargetCollider, _float fDistance = 0.5f);

#ifdef _DEBUG
public:
    HRESULT Render();
#endif // _DEBUG

private:
    TYPE m_ColliderType = { COLLIDER_END };

    class CBounding* m_pCurrentBounding = { nullptr };

    _bool m_isColl = { false };

#ifdef _DEBUG
private:
    PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
    BasicEffect* m_pShader = { nullptr };
    ID3D11InputLayout* m_pInputLayout = { nullptr };

    _float4 m_vColor;
#endif

public:
    static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};
END
