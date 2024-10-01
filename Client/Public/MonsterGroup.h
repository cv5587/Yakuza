#pragma once
#include "LandObject.h"

#include "Client_Defines.h"

/* 몬스터를 묶어서 생성하는 용도로 한다. */
BEGIN(Client)
class CMonsterGroup :
    public CLandObject
{
public:
    typedef struct tMapAdventureObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        vector<CLandObject::LANDOBJ_MAPDESC>        vMonsters;
        int                                         iGroupNum;
    }MONSTERGROUPDESC;

public:
    vector<class CMonster*> Get_Monsters() { return m_Monsters; }

private:
    CMonsterGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMonsterGroup(const CMonsterGroup& rhs);
    virtual ~CMonsterGroup() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override; 

private:
    class CFightManager* m_pFightManager = { nullptr };
    vector<class CMonster*> m_Monsters;

    _uint m_iMaxCount = { 3 };
    _int   m_iGroupNum = { 0 };

    _bool m_isFight = { false };

private:
    void Check_Fight();

public:
    static CMonsterGroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();

    string Get_CurrentAnimationName() override;
};
END