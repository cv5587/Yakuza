#pragma once

#include "Transform.h"

BEGIN(Engine)
class ENGINE_DLL CGameObject abstract :
    public CBase
{
public:
    typedef struct tGameObjectDesc : public CTransform::TRANSFORM_DESC
    {
        _uint iObjectIndex;
    } GAMEOBJECT_DESC;

protected:
    CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject(const CGameObject& rhs);
    virtual ~CGameObject() = default;

public:
    class CTransform* m_pTransformCom = { nullptr };

public:
    _bool isDead() { return m_isDead; }
    _bool isObjectDead() { return m_isObjectDead; }
    _bool isCloned() { return m_isCloned; }
    _float Get_CamDistance() { return m_fCamDistance; }

public:
    void        Set_Dead() { m_isDead = true; }               // 객체 자체가 죽었을 때 (객체를 삭제한다)
    void        Set_ObjectDead() { m_isObjectDead = true; }   // 객체가 죽었는지 (객체 삭제X)
    float       Get_ObjID() { return m_iObjectIndex; }
    void        Set_ObjID(float fID) { m_iObjectIndex = fID; }
    void        Set_Render(_bool bRender) { m_bRender = bRender; } // occulusion culling 후 정보 set

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Tick(const _float& fTimeDelta);
    virtual void Tick(const _float& fTimeDelta);
    virtual void Late_Tick(const _float& fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT Render_Bloom() { return S_OK; }
    virtual HRESULT Render_LightDepth() { return S_OK; }
    virtual HRESULT Render_BoneCompute() { return S_OK; }
    virtual HRESULT Render_OcculusionDepth() { return S_OK; }
    virtual void ImpulseResolution(CGameObject* pTargetObject, _float fDistance = 0.5f) {};
    virtual void Attack_Event(CGameObject* pHitObject, _bool isItem = false) {};

    virtual HRESULT Check_OcculusionCulling() { return S_OK; }

public:
    class CComponent* Get_Component(wstring strComponentTag);
    CTransform* Get_TransformCom() { return m_pTransformCom; }

    class CCollider* Get_Collider() {
        return m_pColliderCom;
    }

protected:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

protected:
    map<const wstring, class CComponent*> m_Components;

protected:
    class CCollider* m_pColliderCom = { nullptr };        // 아이템과 충돌처리를 위해 게임오브젝트로 옮김

    _uint m_iCurrentLevel = { 0 };
    _bool m_isDead = { false };
    _bool m_isObjectDead = { false };
    _bool m_isCloned = { false };

    _uint   m_iObjectIndex = { 0 };

    _bool   m_bRender = { true };

    _float m_fCamDistance = { 0.f };

protected:
    vector<_float> m_Casecade;

protected:
    HRESULT Add_Component(_uint iLevelIndex, const wstring strComponentPrototypeTag, const wstring strComponentTag, class CComponent** pComponent, void* pArg = nullptr);

private:
    class CComponent* Find_Component(const wstring StrComponentTag);

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

END

