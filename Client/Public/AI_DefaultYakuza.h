#pragma once
#include "AI_Monster.h"

/*
* 도지마조에서 쓰일 몬스터
[공격 패턴]
1. 콤보
e_sae_cmb_01 -> e_sae_cmb_02 -> e_sae_cmb_03

2. Kick
e_sae_atk_kick_f

3. run atk
e_sae_atk_run

4. heavy_f
e_sae_atk_heavy_f

5. heavy_renda
e_sae_atk_heavy_renda
*/

BEGIN(Client)
class CAI_DefaultYakuza :
    public CAI_Monster
{
private:
    CAI_DefaultYakuza();
    CAI_DefaultYakuza(const CAI_DefaultYakuza& rhs);
    virtual ~CAI_DefaultYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _float& fTimeDelta);
    virtual NODE_STATE Execute();

private:
    virtual void Ready_Tree();

private:
    virtual CBTNode::NODE_STATE HitAndGuard() override;

    CBTNode::NODE_STATE Attack();
    CBTNode::NODE_STATE ATK_CMB();
    CBTNode::NODE_STATE ATK_Kick();
    CBTNode::NODE_STATE ATK_Run_Attck();
    CBTNode::NODE_STATE ATK_Heavy();
    CBTNode::NODE_STATE ATK_Heavy_Renda();

public:
    static CAI_DefaultYakuza* Create();
    virtual CBTNode* Clone(void* pArg);
    virtual void Free();
};
END
