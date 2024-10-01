#pragma once
#include "Bounding.h"

BEGIN(Engine)
class CBounding_AABB :
    public CBounding
{
public:
    typedef struct tAABBDesc : public CBounding::BOUNDING_DESC {
        _float3 vExtents;
    } BOUNDING_AABB_DESC;
private:
    CBounding_AABB();
    virtual ~CBounding_AABB() = default;

public:
    virtual void* Get_Desc() { return m_pBoundingBox; }
    virtual void* Get_OriginDesc() { return m_pOriginalBox; }       //0625 Çý¿øÃß°¡

public:
    virtual void Set_Value(void* pDesc) override;
    virtual void Set_Center(const _float3& vCenter) override;
    virtual void Set_Rotation(const _float3& vRotation) override;

public:
    virtual HRESULT Initialize(const void* pArg);
    virtual void Tick(_fmatrix WorldMatrix) override;
    virtual _bool Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance = 0.5f) override;
    virtual const _float3& ImpulseResolution(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance = 0.5f) override;

#ifdef _DEBUG
    virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif // _DEBUG

private:
    BoundingBox* m_pOriginalBox = { nullptr };
    BoundingBox* m_pBoundingBox = { nullptr };

    _float3 m_vExtents = {};

public:
    static CBounding_AABB* Create(const void* pArg);
    virtual void Free() override;
};
END
