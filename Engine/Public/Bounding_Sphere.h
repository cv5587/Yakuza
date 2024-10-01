#pragma once
#include "Bounding.h"

BEGIN(Engine)
class CBounding_Sphere final:
    public CBounding
{
public:
    typedef struct tSphereDesc : public CBounding::BOUNDING_DESC {
        _float		fRadius;
    } BOUNDING_SPHERE_DESC;
private:
    CBounding_Sphere();
    virtual ~CBounding_Sphere() = default;

public:
    virtual void* Get_Desc() { return m_pBoundingBox; }
    virtual void* Get_OriginDesc() { return m_pOriginalBox; }       //0625 Çý¿øÃß°¡

public:
    virtual void Set_Value(void* pDesc) override;
    virtual void Set_Center(const _float3& vCenter) override;

public:
    virtual HRESULT Initialize(const void* pArg);
    virtual void Tick(_fmatrix WorldMatrix) override;
    virtual _bool Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding, _float fDistance = 0.5f) override;

#ifdef _DEBUG
    virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif // _DEBUG

private:
    BoundingSphere* m_pOriginalBox = { nullptr };
    BoundingSphere* m_pBoundingBox = { nullptr };
    
    _float m_fRadius = {0.f};

public:
    static CBounding_Sphere* Create(const void* pArg);
    virtual void Free() override;
};

END