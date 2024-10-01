#include "RoadStanding.h"

#include "GameInstance.h"

#include "AI_RoadStanding.h"

CRoadStanding::CRoadStanding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAdventure{pDevice, pContext }
{
}

CRoadStanding::CRoadStanding(const CRoadStanding& rhs)
	:CAdventure{ rhs }
{
}

HRESULT CRoadStanding::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoadStanding::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CRoadStanding::Priority_Tick(const _float& fTimeDelta)
{
}

void CRoadStanding::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation();

	if(m_iGender == GENDER_M)
		m_pModelCom->Play_Animation(fTimeDelta * m_fOffset, m_pAnimCom, m_isAnimLoop);
	else
	{
		CModel::ANIMATION_DESC Desc{ m_iAnim, m_isAnimLoop };
		m_pModelCom->Set_AnimLoop(m_iAnim, m_isAnimLoop);
		m_pModelCom->Set_AnimationIndex(m_iAnim, m_fChangeInterval);

		m_pModelCom->Play_Animation(fTimeDelta * m_fOffset, Desc);
	}

	Synchronize_Root(fTimeDelta);
}

void CRoadStanding::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoadStanding::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CRoadStanding::Change_Animation()
{
	__super::Change_Animation();
}

void CRoadStanding::Free()
{
	__super::Free();
	Safe_Release(m_pTree);
}
