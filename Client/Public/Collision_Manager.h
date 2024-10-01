#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Effect.h"
BEGIN(Engine)
class CCollider;
class CGameObject;
class CGameInstance;
END

BEGIN(Client)
class CCollision_Manager :
    public CBase
{
    DECLARE_SINGLETON(CCollision_Manager)

public:
    // 소켓콜라이더만 적용 (아이템은 소켓콜라이더를 사용하지 않음)
    enum COLLIDER_TYPE
    {
        PLAYER, ENEMY, TYPE_END
    };

private:
    CCollision_Manager();
    virtual ~CCollision_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_ImpulseResolution(CGameObject* pObejct);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수
    HRESULT Add_AttackCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수
    HRESULT Add_HitCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수
    HRESULT Add_ItemCollider(CCollider* pCollider);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수

    HRESULT Add_MapCollider(CCollider* pCollider);
    void    Clear_MapCollider();

    void Tick();

public:
    void All_Clear();

public:
    /*
    * 실제 충돌처리를 비교할 함수

    * 인자로 넣는 객체와 m_HitColliders 벡터 원소들의 충돌을 비교한다.
    */
    void Enemy_Hit_Collision();
    void Player_Hit_Collision();
    void Item_Attack_Collision();           // 아이템에 몬스터가 맞았는지?
    void ItemCollision();

    // 카메라 - 맵 충돌 관련
    _bool Map_Collision_Move(CCollider* pCollider, CTransform* pTransform); // 충돌 후 position 다시 나두기
    _bool Check_Map_Collision(CCollider* pCollider, XMVECTOR& pCollisionPos, CTransform* pTransform); // 충돌 확인용 - transform으로 확인
    _bool Check_Map_Collision_Using_Position(CCollider* pCollider, XMVECTOR vPosition, XMVECTOR& pCollisionPos); //충돌 확인용 - position(vector)로 확인

    CGameObject* Get_Near_Object(CGameObject* pObject, vector<CGameObject*>& pObjects, _float fDistance = 5.f);
    CCollider* Get_Near_Collider(CGameObject* pObject, vector<CCollider*>& pObjects, _float fDistance = 5.f);

private:
    // 캐릭터가 겹치지않고 밀어내는 함수
    // AABB끼리만 구현되어있음
    void ImpulseResolution();           // 따로 모아둔 벡터에서 서로 밀어내는 작업을 한다 (Tick에서 돌림)

    void ResolveCollision(BoundingSphere* sphere, BoundingBox* box, CTransform* pTransform);

    XMVECTOR Find_Collision_Position(BoundingSphere* sphere, BoundingBox* box, CTransform* pTransform);


    // Radius 가져오기 위해서 sphere 가져옴
    // transform의 position으로 충돌 확인
    bool Check_PositionAABB_Collision(BoundingSphere* sphere, BoundingBox* box, XMVECTOR vPosition, XMVECTOR& pCollisionPos);

private:
    vector<CGameObject*> m_ImpulseResolutionObjects;

    /*
    * AttackColliders는 Attack타입의 소켓 콜라이더들을 모아둘 벡터
    * m_AttackColliders[PLAYER] 라면 플레이어의 Attack타입의 콜라이더 중, 현재 켜져있는 애들만 모아둠
    * m_AttackColliders[ENEMY] 라면 적들의 Attack 타입의 콜라이더 중 현재 틱에서 켜져있는 애들만 모아둠
    * 
    * HitColliders는 Hit타입의 소켓 콜라이더들을 모아둘 벡터
    * m_HitColliders[PLAYER] 라면 플레이어의 Hit콜라이더를 모아둘 것이며, m_AttackColliders[ENEMY]와 충돌처리 한다.
    * m_HitColliders[ENEMY] 라면 플레이어의 Hit콜라이더를 모아둘 것이며, m_AttackColliders[PLAYER]와 충돌처리 한다.
    */
    vector<class CSocketCollider*> m_AttackColliders[TYPE_END];
    vector<class CSocketCollider*> m_HitColliders[TYPE_END];


    vector<CCollider*> m_MapColliders;
    vector<CCollider*> m_ItemColliders;

    CGameInstance* m_pGameInstance = { nullptr };
    class CEffectManager* m_pEffectManager = { nullptr };


    float           m_fIntersectDistance = { 0 };

private:
    void Impulse_Clear();
    void Battle_Clear();
    void Item_Clear();

public:
    virtual void Free();
};
END
