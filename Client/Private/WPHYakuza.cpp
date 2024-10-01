#include "WPHYakuza.h"

#include "GameInstance.h"
#include "Mesh.h"

#include "SocketCollider.h"
#include "Collision_Manager.h"

#include "AI_WPHYakuza.h"

#include "Weapon_Sofa.h"

CWPHYakuza::CWPHYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster { pDevice, pContext }
{
}

CWPHYakuza::CWPHYakuza(const CWPHYakuza& rhs)
    : CMonster { rhs }
{
}

HRESULT CWPHYakuza::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWPHYakuza::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_wstrModelName = TEXT("Jimu");

    if (FAILED(Add_CharacterData()))
        return E_FAIL;

	m_iMonsterType = CMonster::WPH;

    if (FAILED(Add_Objects()))
        return E_FAIL;

	m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);

    return S_OK;
}

void CWPHYakuza::Priority_Tick(const _float& fTimeDelta)
{
	m_pSofa->Priority_Tick(fTimeDelta);
}

void CWPHYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pSofa->Tick(fTimeDelta);
}

void CWPHYakuza::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	m_pSofa->Late_Tick(fTimeDelta);
}

HRESULT CWPHYakuza::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//행동트리 저장
	CAI_WPHYakuza::AI_MONSTER_DESC AIDesc{};
	AIDesc.pState = &m_iState;
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_WPHYakuza*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_WPHYakuza"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

HRESULT CWPHYakuza::Add_Objects()
{
	CSocketObject::SOCKETOBJECT_DESC Desc{};
	Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("buki_r_n");
	Desc.fRotatePecSec = XMConvertToRadians(90.f);
	Desc.fSpeedPecSec = 1.f;
	m_pSofa = dynamic_cast<CSofa*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Sofa"), &Desc));
	return S_OK;
}

void CWPHYakuza::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		m_strAnimName = "e_wph_stand_btl";
		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_HEAVY_ATTACK:
	{
		m_strAnimName = "e_wph_atk_heavy";
		break;
	}
	default:
		break;
	}

	if (FAILED(Setup_Animation()))
		return;
}

CWPHYakuza* CWPHYakuza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWPHYakuza* pInstance = new CWPHYakuza(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
        Safe_Release(pInstance);

    return pInstance;
}

CGameObject* CWPHYakuza::Clone(void* pArg)
{
    CWPHYakuza* pInstance = new CWPHYakuza(*this);

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CWPHYakuza::Free()
{
    __super::Free();

	Safe_Release(m_pSofa);
}
