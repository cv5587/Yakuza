#pragma once
#include "AI_Monster.h"

/*
맨손 공격은 DEFAULT YAKUZA와 동일함

나이프 공격은 총 3개

나이프
e_wpb_knife_atk[e_wpb_knife_atk]
- 일반 나이프 공격

e_wpb_knife_atk_sting_3ren[e_wpb_knife_atk_sting_3ren]
- 3ren 나이프 공격

e_wpb_knife_cmb_01[e_wpb_knife_cmb_01]
e_wpb_knife_cmb_02[e_wpb_knife_cmb_02]
콤보 2단
*/

BEGIN(Client)
class CAI_Yoneda :
    public CAI_Monster
{
public:
    typedef struct tAIYonedaDesc : public CAI_Monster::AI_MONSTER_DESC {
        const _uint* pWeaponType;
    } AI_YONEDA_DESC;

private:
    CAI_Yoneda();
    CAI_Yoneda(const CAI_Yoneda& rhs);
    virtual ~CAI_Yoneda() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    const _uint* m_pWeaponType = { nullptr };

private:
    void Ready_Tree();

private:
    CBTNode::NODE_STATE Attack();
    CBTNode::NODE_STATE ATK_CMB();
    CBTNode::NODE_STATE ATK_Kick();
    CBTNode::NODE_STATE ATK_Run_Attck();
    CBTNode::NODE_STATE ATK_Heavy();
    CBTNode::NODE_STATE ATK_Heavy_Renda();

    CBTNode::NODE_STATE ATK_Normal();
    CBTNode::NODE_STATE ATK_3Ren();
    CBTNode::NODE_STATE ATK_Knife_CMB();

public:
    static CAI_Yoneda* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free() override;
};
END
