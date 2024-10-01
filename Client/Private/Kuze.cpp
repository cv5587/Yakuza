#include "Kuze.h"

#include "GameInstance.h"
#include "Collision_Manager.h"
#include "AI_Kuze.h"
#include "Mesh.h"

#include "SocketCollider.h"
#include "Player.h"
#include <Camera.h>
#include "SocketEffect.h"

CKuze::CKuze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CKuze::CKuze(const CKuze& rhs)
	: CMonster{ rhs }
{
}

HRESULT CKuze::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKuze::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_wstrModelName = TEXT("Jimu");
	m_wstrScndModelName = TEXT("Kuze_Fight");

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	m_iMonsterType = CMonster::KUZE;

	m_Info.fMaxHP = 300.f;
	m_Info.fHp = m_Info.fMaxHP;

	m_iHandAnimIndex = HAND_MIN;
	On_Separation_Hand(0);			// 양손 분리 켜둠

	m_iFaceAnimIndex = 0;
	On_Separation_Face();

	return S_OK;
}

void CKuze::Priority_Tick(const _float& fTimeDelta)
{
}

void CKuze::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iHandAnimIndex, m_SeparationAnimComs[HAND_ANIM], false, (_int)HAND_ANIM);
	m_pModelCom->Play_Animation_Separation(fTimeDelta, m_iFaceAnimIndex, m_SeparationAnimComs[FACE_ANIM], false, (_int)FACE_ANIM);

	if (m_pGameInstance->GetKeyState(DIK_RCONTROL) == TAP)
	{
		dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Player"), 0))->Play_Kuze_QTE(this);
	}

	Trail_Event();
	Sound_Event();
}

void CKuze::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CKuze::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	//스웨이를 사용하고 있을 경우 충돌 x
	if (m_pTree->isSway())
		return;

	//CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Camera"), CAMERA_PLAYER));
	//pCamera->Set_Shaking(true, vDir);

	//충돌이 일어났을때?
	m_isColl = true;
	m_fHitDamage = fDamage;

	//데미지 처리하기
	if (!m_isObjectDead)
	{
		m_Info.fHp -= fDamage;

		//무지성이라 변경해야함.
		if (m_iPage == ONE && m_Info.fHp <= 150)
		{
			m_iPage = TWO;
		}

		if (m_Info.fHp <= 0.f)
			m_isObjectDead = true;
	}
}

HRESULT CKuze::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Kuze_Fight"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//Prototype_Component_Anim_KuzeFace
	CAnim* pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_KuzeFace"),
		TEXT("Com_Anim_Face"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);

	//Prototype_Component_Anim_Hand
	pAnimCom = { nullptr };
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_Hand"),
		TEXT("Com_Anim_Hand"), reinterpret_cast<CComponent**>(&pAnimCom))))
		return E_FAIL;
	m_SeparationAnimComs.push_back(pAnimCom);


	//행동트리 저장
	CAI_Kuze::AI_MONSTER_DESC AIDesc{};
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_Kuze*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_Kuze"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

void CKuze::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

  	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		//e_kuz_stand_blend[e_kuz_stand_blend]
		m_strAnimName = "e_kuz_stand_blend";
		m_isAnimLoop = true;
		m_isDown = false;

		break;
	}
	case MONSTER_SHIFT_F:
	{
		//e_kuz_shift_f[e_kuz_shift_f]
		m_strAnimName = "e_kuz_shift_f";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_L:
	{
		//e_kuz_shift_l[e_kuz_shift_l]
		m_strAnimName = "e_kuz_shift_l";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_R:
	{
		//e_kuz_shift_r[e_kuz_shift_r]
		m_strAnimName = "e_kuz_shift_r";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SHIFT_B:
	{
		//e_kuz_shift_b[e_kuz_shift_b]
		m_strAnimName = "e_kuz_shift_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_SWAY_B:
	{
		//e_kuz_sway_b[e_kuz_sway_b]
		m_strAnimName = "e_kuz_sway_b";
		break;
	}
	case MONSTER_SWAY_F:
	{
		//e_kuz_sway_f[e_kuz_sway_f]
		m_strAnimName = "e_kuz_sway_f";
		break;
	}
	case MONSTER_SWAY_L:
	{
		//e_kuz_sway_l[e_kuz_sway_l]
		m_strAnimName = "e_kuz_sway_l";
		break;
	}
	case MONSTER_SWAY_R:
	{
		//e_kuz_sway_r[e_kuz_sway_r]
		m_strAnimName = "e_kuz_sway_r";
		break;
	}
	case MONSTER_ATK_DOWN:
	{
		//e_kuz_atk_down[e_kuz_atk_down]
		m_strAnimName = "e_kuz_atk_down";
		Shaking(0.3f, 0.2f, 0.3f);
		break;
	}
	case MONSTER_JAB:
	{
		//e_kuz_atk_jab[e_kuz_atk_jab]
		m_strAnimName = "e_kuz_atk_jab";
		break;
	}
	case MONSTER_CMB_A_1:
	{
		//e_kuz_cmb_a_01[e_kuz_cmb_a_01]
		m_strAnimName = "e_kuz_cmb_a_01";
		break;
	}
	case MONSTER_CMB_A_2:
	{
		//e_kuz_cmb_a_02[e_kuz_cmb_a_02]
		m_strAnimName = "e_kuz_cmb_a_02";
		Shaking(0.7f, 0.2f, 0.3f);
		break;
	}
	case MONSTER_CMB_A_3:
	{
		//e_kuz_cmb_a_03[e_kuz_cmb_a_03]
		m_strAnimName = "e_kuz_cmb_a_03";
		Shaking(0.3f, 0.2f, 0.3f);
		break;
	}
	case MONSTER_CMB_B_1:
	{
		//e_kuz_cmb_b_01[e_kuz_cmb_b_01]
		m_strAnimName = "e_kuz_cmb_b_01";
		break;
	}
	case MONSTER_CMB_B_2:
	{
		//e_kuz_cmb_b_02[e_kuz_cmb_b_02]
		m_strAnimName = "e_kuz_cmb_b_02";
		Shaking(0.3f, 0.2f, 0.3f);
		break;
	}
	case MONSTER_CMB_B_3:
	{
		//e_kuz_cmb_b_03[e_kuz_cmb_b_03]
		m_strAnimName = "e_kuz_cmb_b_03";
		Shaking(0.3f, 0.2f, 0.3f);
		break;
	}
	case MONSTER_CMB_HEADBUTT_1:
	{
		//e_kuz_cmb_headbutt_01[e_kuz_cmb_headbutt_01]
		m_strAnimName = "e_kuz_cmb_headbutt_01";
		Shaking(0.3f, 0.2f, 0.3f);
		break;
	}
	case MONSTER_CMB_HEADBUTT_2:
	{
		//e_kuz_cmb_headbutt_02[e_kuz_cmb_headbutt_02]
		m_strAnimName = "e_kuz_cmb_headbutt_02";

		// 첫 쉐이킹
		if (m_iShakedCount < 3)
		{
			m_iShakedCount++;
			Shaking(0.2f * (m_iShakedCount + 1.f), 0.2f, 0.2f);
		}
		else
		{
			Shaking(0.8f, 0.2f, 0.4f);			// 마지막타에만 강하게
		}

		break;
	}
	case MONSTER_CMB_RENDA_1:
	{
		//e_kuz_cmb_renda_01[e_kuz_cmb_renda_01]
		m_strAnimName = "e_kuz_cmb_renda_01";
		break;
	}
	case MONSTER_CMB_RENDA_2:
	{
		//e_kuz_cmb_renda_02_l[e_kuz_cmb_renda_02_l]
		m_strAnimName = "e_kuz_cmb_renda_02_l";
		break;
	}
	case MONSTER_CMB_RENDA_3:
	{
		//e_kuz_cmb_renda_02_l_fin[e_kuz_cmb_renda_02_l_fin]
		m_strAnimName = "e_kuz_cmb_renda_02_l_fin";
		Shaking(0.3f, 0.2f, 0.3f);
		break;
	}
	case MONSTER_HEAVY_ATTACK:
	{
		//e_kuz_atk_heavy[e_kuz_atk_heavy]
		m_strAnimName = "e_kuz_atk_heavy";
		// 첫 쉐이킹
		if (m_iShakedCount == 0)			// 첫타, 발 밟기
		{
			m_iShakedCount++;
			Shaking(0.3f, 0.2f, 0.2f);			
		}
		else if (m_iShakedCount == 2)		// 마지막타, 팔꿈치로 후려치는 거에는 쉐이킹을 강하게
		{
			Shaking(0.6f, 0.2f, 0.4f);			
		}
		break;
	}
	case MONSTER_HIJI_2REN:
	{
		//e_kuz_atk_hiji_2ren[e_kuz_atk_hiji_2ren]
		m_strAnimName = "e_kuz_atk_hiji_2ren";
		Shaking(0.3f, 0.2f, 0.2f);
		break;
	}
	case MONSTER_STEP_B:
	{
		m_strAnimName = "e_kuz_step_b";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_STEP_L:
	{
		m_strAnimName = "e_kuz_step_l";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_STEP_R:
	{
		m_strAnimName = "e_kuz_step_r";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_STEP_F:
	{
		m_strAnimName = "e_kuz_step_f";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_RUN:
	{
		m_strAnimName = "e_kuz_run";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_GURAD_START:
	{
		m_strAnimName = "e_kuz_guard_st";
		break;
	}
	case MONSTER_GURAD_LOOP:
	{
		m_strAnimName = "e_kuz_guard_lp";
		m_isAnimLoop = true;
		break;
	}
	case MONSTER_GURAD_END:
	{
		m_strAnimName = "e_kuz_guard_en";
		break;
	}
	case MONSTER_GURAD_FLOAT:
	{
		m_strAnimName = "e_kuz_guard";
		break;
	}
	default:
		break;
	}

	m_pData->Set_CurrentAnimation(m_strAnimName);

	if (FAILED(Setup_Animation()))
		return;
}

void CKuze::Trail_Event()
{
	auto& pCurEvents = m_pData->Get_Current_Trail_Events();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pAnimCom[m_iCurrentAnimType]->Get_AnimPosition());
		_double Duration = *(m_pAnimCom[m_iCurrentAnimType]->Get_AnimDuration());

		if (CurPos >= pEvent.fAinmPosition && CurPos < Duration)
		{
			if (pEvent.iType == 0)		// 트레일 켜주기
			{
				// 현재 본 이름을 가진 모든 이펙트를 온해주기
				auto lower_bound_iter = m_pEffects.lower_bound(pEvent.strBonelName);

				if (!(lower_bound_iter != m_pEffects.end() && (*lower_bound_iter).first != pEvent.strBonelName))
				{
					auto upper_bound_iter = m_pEffects.upper_bound(pEvent.strBonelName);

					if (lower_bound_iter == upper_bound_iter && lower_bound_iter != m_pEffects.end())
					{
						lower_bound_iter->second->On();
					}
					else
					{
						for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
						{
							lower_bound_iter->second->On();
						}
					}

				}

			}
			else
			{
				// 현재 본 이름을 가진 모든 이펙트를 온해주기
				auto lower_bound_iter = m_pEffects.lower_bound(pEvent.strBonelName);

				if (!(lower_bound_iter != m_pEffects.end() && (*lower_bound_iter).first != pEvent.strBonelName))
				{
					auto upper_bound_iter = m_pEffects.upper_bound(pEvent.strBonelName);

					if (lower_bound_iter == upper_bound_iter && lower_bound_iter != m_pEffects.end())
					{
						lower_bound_iter->second->Off();
					}
					else
					{
						for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
						{
							lower_bound_iter->second->Off();
						}
					}
				}
			}

		}
	}
}

CKuze* CKuze::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKuze* pInstance = new CKuze(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CKuze::Clone(void* pArg)
{
	CKuze* pInstance = new CKuze(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CKuze::Free()
{
	__super::Free();
}
