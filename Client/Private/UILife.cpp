#include "UILife.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
#include"InventoryManager.h"
#include "Player.h"
#include "LandObject.h"
CUILife::CUILife()
	:CUIScene{}
{
}

CUILife::CUILife(const CUILife& rhs)
	:CUIScene{rhs}
{
}

HRESULT CUILife::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice , pContext , pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILife::Tick(const _float& fTimeDelta)
{
	Update_PlayerInfo();

	__super::Tick(fTimeDelta);
	if (!m_EventUI.empty())
		m_EventUI[m_iBts]->Tick(fTimeDelta);

	m_EventUI[GAUAGE]->Tick(fTimeDelta);

	m_EventUI[NUM]->Tick(fTimeDelta);
	m_EventUI[LIGHT]->Tick(fTimeDelta);
	return S_OK;
}

HRESULT CUILife::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (!m_EventUI.empty())
		m_EventUI[m_iBts]->Late_Tick(fTimeDelta);

	m_EventUI[GAUAGE]->Late_Tick(fTimeDelta);
	m_EventUI[NUM]->Late_Tick(fTimeDelta);
	m_EventUI[LIGHT]->Late_Tick(fTimeDelta);
	return S_OK;
}

void CUILife::Update_PlayerInfo()
{
	_uint Level = m_pGameInstance->Get_CurrentLevel();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(Level ,TEXT("Layer_Player"),0));
	if (nullptr == pPlayer)
		return;


	m_iHitLevel = pPlayer->Get_CurrentHitLevel()+1;

	//advbenture , krs , krh, krc 기본 , 불한당, 러쉬 , 파괴자.
	//m_iCurrentHitLevel 0 ,1,2
	//m_fHitGauge 50기준 렙업
	 vector<CUI_Object*>* pSkillGauage = dynamic_cast<CGroup*>(m_EventUI[GAUAGE])->Get_pPartObjects();

	CLandObject::LAND_OBJ_INFO Info =	pPlayer->Get_Info();

	_float Upoint = m_pGameInstance->Lerp(-1.f, 0.f, Info.fHp / Info.fMaxHP);

	dynamic_cast<CUI_Texture*>((*pSkillGauage)[HP])->Change_Point(_float4(0.f, 0.f, Upoint, 0.f), _float4(0.f, 0.f, Upoint, 0.f));

	_float Hitgauage =pPlayer->Get_HitGauage();

	 _float Curgauage = fmod(Hitgauage, 50.f);


	 for (size_t i = SKILL0; i < GAUAGE_END; i++)
	 {
		 if(i<m_iHitLevel)
			 dynamic_cast<CUI_Texture*>((*pSkillGauage)[i])->Change_Point(_float4(0.f,0.f,0.f,0.f), _float4(0.f, 0.f,  m_fEndHit[i-1], 0.f));
		 else if(m_iHitLevel<i)
			 dynamic_cast<CUI_Texture*>((*pSkillGauage)[i])->Change_Point(_float4(0.f, 0.f, -1.f, 0.f), _float4(0.f, 0.f, -1.f, 0.f));
		 else if (i == m_iHitLevel)
		 {
			 _float UGauagepoint = m_pGameInstance->Lerp(-1.f, 0.f, Curgauage / 50.f);
			 _float DGauagepoint = UGauagepoint;
			 if (DGauagepoint >= m_fEndHit[i])
			 {
				 DGauagepoint = m_fEndHit[i];
			 }

				 dynamic_cast<CUI_Texture*>((*pSkillGauage)[i])->Change_Point(_float4(0.f, 0.f, UGauagepoint, 0.f), _float4(0.f, 0.f, DGauagepoint, 0.f));

		 }

	 }
	 _uint iGauageLevel = 0;
	 if (m_iHitLevel > 3)
		 iGauageLevel = m_iHitLevel - 1;
	 else
		 iGauageLevel = m_iHitLevel;


	 dynamic_cast<CGroup*>(m_EventUI[NUM])->Show_Choice(iGauageLevel -1);
	 dynamic_cast<CGroup*>(m_EventUI[LIGHT])->Show_Choice(m_iBts);





	   
}

void CUILife::Change_Style(_uint iStyle)
{
	_int Bts = iStyle-1;

	if (-1 == Bts)	
		Bts = m_iBts;	

	m_iBts = Bts;	

	m_EventUI[m_iBts]->Show_UI();

	for (size_t i = 0; i < 3; i++)
	{
		m_EventUI[i]->Set_isPlay(true);
	}

}

CUILife* CUILife::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,void* pArg)
{
	CUILife* pInstance = new CUILife();
	if (FAILED(pInstance->Initialize(pDevice, pContext ,pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUILife::Free()
{
	__super::Free();
}
