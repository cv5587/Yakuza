#include "AI_CarChase.h"

#include "GameInstance.h"

#include "Highway_Taxi.h"
#include "CarChase_Monster.h"

CAI_CarChase::CAI_CarChase()
    : CBTNode{},
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

CAI_CarChase::CAI_CarChase(const CAI_CarChase& rhs)
    : CBTNode{ rhs },
    m_pGameInstance{ rhs.m_pGameInstance }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_CarChase::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAI_CarChase::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    AI_CARCHASE_DESC* pDesc = static_cast<AI_CARCHASE_DESC*>(pArg);
	memcpy(m_pAnimCom, pDesc->pAnim, sizeof(CAnim*) * 2);
    m_pThis = pDesc->pThis;
	m_pState = pDesc->pState;
	m_pDir = pDesc->pDir;
	m_pCurrentAnimType = pDesc->pCurrentAnimType;
	m_pWeaponType = pDesc->pWeaponType;

    m_pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0));

	m_fDelayAttackDuration = m_pGameInstance->Get_Random(4.f, 8.f);
	m_fDelayAttackReadyDuration = m_fDelayAttackDuration + 2.f;

    return S_OK;
}

void CAI_CarChase::Tick(const _float& fTimeDelta)
{
	if (!m_isAttack)
		m_fAttackDelayTime += fTimeDelta;

	m_fAttackReadyDelayTime += fTimeDelta;

	m_fSitUpTime += fTimeDelta;

    this->Execute();
}

_float CAI_CarChase::DistanceFromPlayer()
{
	_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vThisPos = m_pThis->m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVector3Length(vPlayerPos - vThisPos).m128_f32[0]; // 거리 측정	

	return fDistance;
}

_uint CAI_CarChase::AngleFromPlayer()
{
	//몬스터를 기준으로 플레이어가 뒤에 존재하는지 확인하기.
	_vector vThisLook = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vThisPos = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	_vector vToPlayer = XMVector3Normalize(vPlayerPos - vThisPos);

	_vector vLeftLook = XMVector3Cross(vThisLook, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_vector vRightLook = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vThisLook);

	_float fLeftDot = XMVectorGetX(XMVector3Dot(vLeftLook, vToPlayer));
	_float fRightDot = XMVectorGetX(XMVector3Dot(vRightLook, vToPlayer));

	if (acos(fLeftDot) > XMConvertToRadians(45.f))
		return DIR_FR;
	else if (acos(fLeftDot) > XMConvertToRadians(90.f))
		return DIR_R;
	else if (acos(fRightDot) > XMConvertToRadians(45.f))
		return DIR_FL;
	else if (acos(fRightDot) > XMConvertToRadians(90.f))
		return DIR_L;

	return DIR_END;
}

_bool CAI_CarChase::isBehine()
{
	//몬스터를 기준으로 플레이어가 뒤에 존재하는지 확인하기.
	_vector vThisLook = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vThisPos = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	_vector vToPlayer = XMVector3Normalize(vPlayerPos - vThisPos);

	_float fDot = XMVector3Dot(vThisLook, vToPlayer).m128_f32[0];
	_float fAngle = acos(fDot);

	//플레이어의 시야각을 180도로 두고 계산한다.
	//뒤에 있는지 없는지 체크.
	return  fAngle > (XMConvertToRadians(180.f) * 0.5f);
}

void CAI_CarChase::LookAtPlayer()
{
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	m_pThis->Get_TransformCom()->LookAt_For_LandObject(vPlayerPos);
}

void CAI_CarChase::Play_Hit()
{
	_uint iRandom = m_pGameInstance->Get_Random(0, 2);
	wstring strName = TEXT("");
	if (iRandom == 0)
		strName = TEXT("gunshot_damage_0.wav");
	else if (iRandom == 1)
		strName = TEXT("gunshot_damage_1.wav");
	else if (iRandom == 2)
		strName = TEXT("gunshot_damage_2.wav");

	m_pGameInstance->PlaySound_W(strName, SOUND_ENEMY_HIT, DEFAULT_EFFECT_VOLUME);
}

_bool CAI_CarChase::Find_PlayerCurrentAnimationName(string strAnimName)
{
	string strPlayerAnimName = m_pPlayer->Get_CurrentAnimationName();
	if (strPlayerAnimName.find(strAnimName) == string::npos)
		return false;

	return true;
}

CBTNode::NODE_STATE CAI_CarChase::Check_Down()
{
	if (m_pThis->isObjectDead() || m_pThis->isReactorDead())
	{
		return CBTNode::SUCCESS;
	}

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_CarChase::Dead()
{
	m_iSkill = SKILL_DEAD;

	return CBTNode::SUCCESS;
}

CBTNode::NODE_STATE CAI_CarChase::Check_Hit()
{
	if (m_pThis->isColl() || m_isHit)
		return CBTNode::SUCCESS;

	return CBTNode::FAIL;
}

CBTNode::NODE_STATE CAI_CarChase::Hit()
{
	return CBTNode::NODE_STATE();
}

CBTNode::NODE_STATE CAI_CarChase::Check_Sync()
{

	return CBTNode::NODE_STATE();
}

CBTNode::NODE_STATE CAI_CarChase::Sync()
{
	return CBTNode::NODE_STATE();
}

void CAI_CarChase::Free()
{
	__super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pRootNode);
}

