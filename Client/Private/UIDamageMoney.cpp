#include "UIDamageMoney.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include "Text.h"
#include "Player.h"
#include"GameInstance.h"
CUIDamageMoney::CUIDamageMoney()
    :CUIScene{}
{
}

CUIDamageMoney::CUIDamageMoney(const CUIDamageMoney& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUIDamageMoney::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;
    return S_OK;
}

HRESULT CUIDamageMoney::Tick(const _float& fTimeDelta)
{
	if (!m_PlayEffect.empty())
	{

		for (auto& iter : m_PlayEffect)
		{
			iter->TimeDelta += fTimeDelta;

			if (iter->TimeDelta > 2.f)
				iter->pUI->Tick(fTimeDelta);
		}
	}

    return S_OK;
}

HRESULT CUIDamageMoney::Late_Tick(const _float& fTimeDelta)
{
	if(!m_PlayEffect.empty())
	{

		for (auto& iter : m_PlayEffect)
		{
			if (iter->pUI->Check_AnimFin())
			{
				Safe_Release(iter->pUI);

				m_PlayEffect.erase(m_PlayEffect.begin());

				CGameObject* pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(),TEXT("Layer_Player"),0);

				dynamic_cast<CPlayer*>(pPlayer)->Set_Money(iter->iMoney * iter->iTime);

			}
			else
				iter->pUI->Late_Tick(fTimeDelta);
		}
	}

	

    return S_OK;
}

void CUIDamageMoney::Add_MoneyEffect(_fmatrix World, _uint iMoney, _uint iTime)
{
	if (999999 < iMoney)
		return;
	

	//돈 변환구간
    CUI_Object* pMoneyEffect = dynamic_cast<CUI_Object*>(m_EventUI[0]->Clone(nullptr));

	pMoneyEffect->Show_UI();

	_matrix FixWorld = XMMatrixIdentity();
	_vector Position = World.r[3];
	
	_vector t{ XMVectorGetX(Position), XMVectorGetY(Position), 0.f, 1.f };

	FixWorld.r[3] = t;

	dynamic_cast<CGroup*>(pMoneyEffect)->Get_TransformCom()->Set_WorldMatrix(FixWorld);

    vector<CUI_Object*> MoneyGroup = dynamic_cast<CGroup*>(pMoneyEffect)->Get_PartObjects();

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
		dynamic_cast<CUI_Texture*>((MoneyGroup)[iIndex])->Change_UV(_float2(U1, V1), _float2(U2, V2));
		++iIndex;

	}
	//7은 돈 표현 숫자 최대 갯수 
	for (size_t i = iIndex; i < 6; i++)
	{
		dynamic_cast<CUI_Texture*>((MoneyGroup)[i])->Change_UV(_float2(0, 0), _float2(0, 0));
	}

	//배율 변환 구간

	_int Row = iTime / 5;
	_int Col = iTime % 5;

	_float U1 = Col * U;
	_float V1 = Row * V;
	_float U2 = (Col + 1) * U;
	_float V2 = (Row + 1) * V;
	dynamic_cast<CUI_Texture*>((MoneyGroup)[6])->Change_UV(_float2(U1, V1), _float2(U2, V2));




	EFFECTTIME* pEffect = new EFFECTTIME;

	pEffect->TimeDelta = 0.f;
	pEffect->pUI = pMoneyEffect;
	pEffect->iMoney = iMoney;
	pEffect->iTime = iTime;

	m_PlayEffect.push_back(pEffect);	
}


CUIDamageMoney* CUIDamageMoney::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUIDamageMoney* pInstance = new CUIDamageMoney();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUIDamageMoney::Free()
{
	for (auto& iter : m_PlayEffect)
	{
		Safe_Release(iter->pUI);
	}
	m_PlayEffect.clear();

    __super::Free();
}
