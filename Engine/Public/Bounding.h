#pragma once
#include "Base.h"
#include "Collider.h"

BEGIN(Engine)
class CBounding :
    public CBase
{
public:
    typedef struct tBoundingDesc : public CCollider::COLLIDER_DESC {
        _float3 vCenter;
    }BOUNDING_DESC;

protected:
    CBounding();
    virtual ~CBounding();

public:
    virtual void* Get_Desc() = 0;
    virtual void* Get_OriginDesc() = 0;                 //0625 Çý¿øÃß°¡
    virtual const _float4& Get_Color() {
        return m_vColor;
    }
    virtual const _float3& Get_Center() {
        return m_vCenter;
    }

public:
    virtual void Set_Value(void* pDesc) {};
    virtual void Set_Center(const _float3& vCenter) {};
    virtual void Set_Rotation(const _float3& vRotation) {};
    void Set_Color(const _float4& vColor) { m_vColor = vColor; }

public:
    virtual HRESULT Initialize(const void* pArg) = 0;
    virtual void Tick(_fmatrix WorldMatrix);
    virtual _bool Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance = 0.5f) = 0;
    virtual const _float3& ImpulseResolution(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance = 0.5f) { return _float3(); }
#ifdef _DEBUG
public:
    virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);
#endif // _DEBUG

protected:
    _float3 m_vCenter;
    _float4 m_vColor;

public:
    virtual void Free() override;
    
};

END