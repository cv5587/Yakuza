#include "GameInstance.h"
#include "Kiryu_KRC_Hit.h"
#include "Player.h"

CKiryu_KRC_Hit::CKiryu_KRC_Hit()
	:CBehaviorAnimation{}
{
	// 몸통 맞을 때 0 ~ 5
	/*0*/m_AnimationIndices.push_back(13); m_AnimationNames.push_back("c_dam_body_lv01_b");
	/*1*/m_AnimationIndices.push_back(14); m_AnimationNames.push_back("c_dam_body_lv01_f");
	/*2*/m_AnimationIndices.push_back(15); m_AnimationNames.push_back("c_dam_body_lv02_b");
	/*3*/m_AnimationIndices.push_back(16); m_AnimationNames.push_back("c_dam_body_lv02_f");
	/*4*/m_AnimationIndices.push_back(17); m_AnimationNames.push_back("c_dam_body_lv02_l");
	/*5*/m_AnimationIndices.push_back(18); m_AnimationNames.push_back("c_dam_body_lv02_r");

	// 머리 맞을 때 6 ~ 13
	/*6*/m_AnimationIndices.push_back(33); m_AnimationNames.push_back("c_dam_head_lv01_b");
	/*7*/m_AnimationIndices.push_back(34); m_AnimationNames.push_back("c_dam_head_lv01_f");
	/*8*/m_AnimationIndices.push_back(35); m_AnimationNames.push_back("c_dam_head_lv01_l");
	/*9*/m_AnimationIndices.push_back(36); m_AnimationNames.push_back("c_dam_head_lv01_r");
	/*10*/m_AnimationIndices.push_back(37); m_AnimationNames.push_back("c_dam_head_lv02_b");
	/*11*/m_AnimationIndices.push_back(38); m_AnimationNames.push_back("c_dam_head_lv02_f");
	/*12*/m_AnimationIndices.push_back(39); m_AnimationNames.push_back("c_dam_head_lv02_l");
	/*13*/m_AnimationIndices.push_back(40); m_AnimationNames.push_back("c_dam_head_lv02_r");

	// 어퍼컷맞을때
	/*14*/m_AnimationIndices.push_back(57); m_AnimationNames.push_back("c_dam_push_head_b");
	/*15*/m_AnimationIndices.push_back(58); m_AnimationNames.push_back("c_dam_push_head_f");

	// 다리맞을때 16 ~ 21
	/*16*/m_AnimationIndices.push_back(43); m_AnimationNames.push_back("c_dam_leg_lv01_b");
	/*17*/m_AnimationIndices.push_back(44); m_AnimationNames.push_back("c_dam_leg_lv01_f");
	/*18*/m_AnimationIndices.push_back(45); m_AnimationNames.push_back("c_dam_leg_lv01_l");
	/*19*/m_AnimationIndices.push_back(46); m_AnimationNames.push_back("c_dam_leg_lv01_r");
	/*20*/m_AnimationIndices.push_back(47); m_AnimationNames.push_back("c_dam_leg_lv02_l");
	/*21*/m_AnimationIndices.push_back(48); m_AnimationNames.push_back("c_dam_leg_lv02_r");

	// 넘어지는거 22 ~ 25
	/*22*/m_AnimationIndices.push_back(86); m_AnimationNames.push_back("c_dwn_body_b");
	/*23*/m_AnimationIndices.push_back(88); m_AnimationNames.push_back("c_dwn_body_f");
	/*24*/m_AnimationIndices.push_back(90); m_AnimationNames.push_back("c_dwn_body_l");
	/*25*/m_AnimationIndices.push_back(91); m_AnimationNames.push_back("c_dwn_body_r");

	// 가드깨지면서 넘어지는거 26 ~ 29
	/*26*/m_AnimationIndices.push_back(92); m_AnimationNames.push_back("c_dwn_break_guard_b");
	/*27*/m_AnimationIndices.push_back(93); m_AnimationNames.push_back("c_dwn_break_guard_f");
	/*28*/m_AnimationIndices.push_back(94); m_AnimationNames.push_back("c_dwn_break_guard_l");
	/*29*/m_AnimationIndices.push_back(95); m_AnimationNames.push_back("c_dwn_break_guard_r");

	// 엎어지는거 30 ~ 33
	/*30*/m_AnimationIndices.push_back(96); m_AnimationNames.push_back("c_dwn_direct_b");
	/*31*/m_AnimationIndices.push_back(99); m_AnimationNames.push_back("c_dwn_direct_f");
	/*32*/m_AnimationIndices.push_back(102); m_AnimationNames.push_back("c_dwn_direct_l");
	/*33*/m_AnimationIndices.push_back(103); m_AnimationNames.push_back("c_dwn_direct_r");

	/*
		// 쓸지 안쓸지 모르겠음
		[6]	c_dam_break_attack[c_dam_break_attack]
		[7]	c_dam_break_guard[c_dam_break_guard]
		[14]	c_dam_fire_b[c_dam_fire_b]
		[15]	c_dam_fire_f[c_dam_fire_f]
	*/
}

void CKiryu_KRC_Hit::Tick(const _float& fTimeDelta)
{
}

void CKiryu_KRC_Hit::Change_Animation()
{
	m_pPlayer->Change_Animation(m_AnimationIndices[m_iCurrentIndex]);
}

_bool CKiryu_KRC_Hit::Get_AnimationEnd()
{
	CModel* pModelCom = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (pModelCom->Get_AnimFinished())
	{
		return true;
	}

	return false;
}

void CKiryu_KRC_Hit::Reset()
{
	m_iCurrentIndex = 0;
}

void CKiryu_KRC_Hit::Setting_Value(void* pValue)
{
	KRC_Hit_DESC* pDesc = static_cast<KRC_Hit_DESC*>(pValue);

	string strAnimName = m_pGameInstance->Extract_String(pDesc->strAnimationName, '[', ']');

	if ("p_krh_cmb_01" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}

	}
	else if ("p_krh_cmb_02" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("p_krh_cmb_03" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("p_krh_cmb_04" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("p_krh_cmb_05" == strAnimName)
	{
		/*
		[30]	c_dam_push_head_b[c_dam_push_head_b]	14
		[31] c_dam_push_head_f[c_dam_push_head_f]		15
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 14;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 14;
			break;
		}
		}
	}
	else if ("e_knk_atk_chop" == strAnimName)
	{
		// [30]	c_dam_push_head_b[c_dam_push_head_b]	14;
		m_iCurrentIndex = 14;
	}
	else if ("e_knk_atk_kick" == strAnimName)
	{
		/*
		* 	m_AnimationIndices.push_back(2);		//[2] c_dam_body_lv02_b[c_dam_body_lv02_b]	2
			m_AnimationIndices.push_back(3);		//[3] c_dam_body_lv02_f[c_dam_body_lv02_f]	3
			m_AnimationIndices.push_back(4);		//[4] c_dam_body_lv02_l[c_dam_body_lv02_l]	4
			m_AnimationIndices.push_back(5);		//[5] c_dam_body_lv02_r[c_dam_body_lv02_r]	5
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 3;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 2;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 5;
			break;
		}
		}
	}
	else if ("e_wpa_cmb_01" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_wpa_cmb_02" == strAnimName)
	{
		/*
		[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
		[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
		[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
		[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}
	else if ("e_wpa_atk_heavy" == strAnimName)
	{
		// [30]	c_dam_push_head_b[c_dam_push_head_b]	14;
		m_iCurrentIndex = 14;
	}
	else if ("e_kta_atk_rariatto" == strAnimName)
	{
		/*
		[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
		[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
		[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
		[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}
	else if ("e_kta_atk_gurad_run" == strAnimName)
	{
		/*22 c_dwn_body_b*/
		/*23 c_dwn_body_f*/
		/*24 c_dwn_body_l*/
		/*25 c_dwn_body_r*/

		// 맞으면 넘어지는 스킬
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 23;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 22;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 24;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 25;
			break;
		}
		}
	}
	else if ("e_kta_cmb_a_01" == strAnimName)
	{
		/*
		* 	[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
			[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
			[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
			[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/

		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}
	else if ("e_kta_cmb_a_02" == strAnimName)
	{
		/*
			[4] c_dam_body_lv02_l[c_dam_body_lv02_l]	4
			[5] c_dam_body_lv02_r[c_dam_body_lv02_r]	5
		*/

		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 5;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 5;
			break;
		}
		}
	}
	else if ("e_kta_cmb_a_03" == strAnimName)
	{
		/*
			[4] c_dam_body_lv02_l[c_dam_body_lv02_l]	4
			[5] c_dam_body_lv02_r[c_dam_body_lv02_r]	5
		*/

		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 5;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 5;
			break;
		}
		}
	}
	else if ("e_kuz_atk_jab" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_a_01" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_a_02" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_a_03" == strAnimName)
	{
		/*
		[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
		[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
		[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
		[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_b_01" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_b_02" == strAnimName)
	{
		/*
		[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
		[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
		[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
		[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_b_03" == strAnimName)
	{
		/*
		[30]	c_dam_push_head_b[c_dam_push_head_b]	14
		[31] c_dam_push_head_f[c_dam_push_head_f]		15
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 14;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 14;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_headbutt_01" == strAnimName)
	{
		/*
		[30]	c_dam_push_head_b[c_dam_push_head_b]	14
		[31] c_dam_push_head_f[c_dam_push_head_f]		15
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 14;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 14;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_headbutt_02" == strAnimName)
	{
		/*
[30]	c_dam_push_head_b[c_dam_push_head_b]	14
[31] c_dam_push_head_f[c_dam_push_head_f]		15
*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 14;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 15;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 14;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_renda_01" == strAnimName)
	{
		/*
			[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
			[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
			[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
			[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_renda_02_l" == strAnimName)
	{
		/*
			[16]	c_dam_head_lv01_b[c_dam_head_lv01_b]	6
			[17]	c_dam_head_lv01_f[c_dam_head_lv01_f]	7
			[18]	c_dam_head_lv01_l[c_dam_head_lv01_l]	8
			[19]	c_dam_head_lv01_r[c_dam_head_lv01_r]	9
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 7;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 6;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 8;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 9;
			break;
		}
		}
	}
	else if ("e_kuz_cmb_renda_02_l_fin" == strAnimName)
	{
		/*
			//[4] c_dam_body_lv02_l[c_dam_body_lv02_l]	4
			//[5] c_dam_body_lv02_r[c_dam_body_lv02_r]	5
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 4;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 5;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 5;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 4;
			break;
		}
		}
	}
	else if ("e_kuz_atk_heavy" == strAnimName)
	{
		// 하단(발로 밟고) 상단 상단(상체를 돌리면서 팔꿈치 휘두름) 때림
	}
	else if ("e_kuz_atk_hiji_2ren" == strAnimName)
	{
		/*
		[20]	c_dam_head_lv02_b[c_dam_head_lv02_b]	10
		[21]	c_dam_head_lv02_f[c_dam_head_lv02_f]	11
		[22]	c_dam_head_lv02_l[c_dam_head_lv02_l]	12
		[23]	c_dam_head_lv02_r[c_dam_head_lv02_r]	13
		*/
		switch (pDesc->iDirection)
		{
		case CPlayer::F:
		{
			m_iCurrentIndex = 11;
			break;
		}
		case CPlayer::B:
		{
			m_iCurrentIndex = 10;
			break;
		}
		case CPlayer::L:
		{
			m_iCurrentIndex = 12;
			break;
		}
		case CPlayer::R:
		{
			m_iCurrentIndex = 13;
			break;
		}
		}
	}

}

string CKiryu_KRC_Hit::Get_AnimationName()
{
	return m_AnimationNames[m_iCurrentIndex];
}

CBehaviorAnimation* CKiryu_KRC_Hit::Create(CPlayer* pPlayer)
{
	CKiryu_KRC_Hit* pInstnace = new CKiryu_KRC_Hit();

	if (nullptr == pInstnace)
	{
		Safe_Release(pInstnace);
		MSG_BOX("Faild To Created : Kiryu_KRC_Hit");
		return pInstnace;
	}

	pInstnace->m_pPlayer = pPlayer;

	return pInstnace;
}

void CKiryu_KRC_Hit::Free()
{
	__super::Free();
}