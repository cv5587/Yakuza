#include "UIMoney.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"
#include "Player.h"
#include"GameInstance.h"
CUIMoney::CUIMoney()
	:CUIScene{}
{
}

CUIMoney::CUIMoney(const CUIMoney& rhs)
	:CUIScene{rhs}
{
}

HRESULT CUIMoney::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMoney::Tick(const _float& fTimeDelta)
{

	UpdateMoney();
	__super::Tick(fTimeDelta);

	for (auto& iter : m_EventUI)
		iter->Tick(fTimeDelta);
	

	return S_OK;
}

HRESULT CUIMoney::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& iter : m_EventUI)
		iter->Late_Tick(fTimeDelta);

	return S_OK;
}

void CUIMoney::UpdateMoney()
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0);

	_uint iMoney = dynamic_cast<CPlayer*>(pPlayer)->Get_Money();

	vector<CUI_Object*> pMoneyGroup = dynamic_cast<CGroup*>(m_EventUI[MONEY])->Get_PartObjects();

	string Number = to_string(iMoney);
	_int iIndex = 0;

	for (_int i = Number.length() - 1; 0 <= i; --i)
	{
		char digit = Number[i];
		_uint OneNum = digit - '0';

		_int Row = OneNum / 5;
		_int Col = OneNum % 5;

		_float U1 = Col * U;
		_float V1 = Row * V;
		_float U2 = (Col + 1) * U;
		_float V2 = (Row + 1) * V;
		dynamic_cast<CUI_Texture*>((pMoneyGroup)[iIndex])->Change_UV(_float2(U1, V1), _float2(U2, V2));
		++iIndex;

	}

	for (size_t i = iIndex; i < pMoneyGroup.size(); i++)
	{
		dynamic_cast<CUI_Texture*>((pMoneyGroup)[i])->Change_UV(_float2(0, 0), _float2(0, 0));
	}

}

CUIMoney* CUIMoney::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIMoney* pInstance = new CUIMoney();
	if (FAILED(pInstance->Initialize(pDevice , pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIMoney::Free()
{
	__super::Free();
}
