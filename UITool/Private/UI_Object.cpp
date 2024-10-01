#include "UI_Object.h"

CUI_Object::CUI_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CUI_Object::CUI_Object(const CUI_Object& rhs)
	: CGameObject { rhs },
	m_strName{rhs.m_strName},
	m_iTypeIndex{rhs.m_iTypeIndex},
	m_isEvent{rhs.m_isEvent},
	m_isScreen{rhs.m_isScreen }
{
}

HRESULT CUI_Object::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Object::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		UI_OBJECT_DESC* pDesc = static_cast<UI_OBJECT_DESC*>(pArg);
		m_strName = pDesc->strName;
		m_iTypeIndex = pDesc->iTypeIndex;
		m_isEvent = pDesc->isEvent;
		m_isScreen = pDesc->isScreen;
	}

	return S_OK;
}

void CUI_Object::Priority_Tick(const _float& fTimeDelta)
{
}

void CUI_Object::Tick(const _float& fTimeDelta)
{
}

void CUI_Object::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CUI_Object::Render()
{
	return S_OK;
}


HRESULT CUI_Object::Save_binary(const string strDirectory)
{
	return E_FAIL;
}

HRESULT CUI_Object::Save_Groupbinary( ofstream& out)
{
	return S_OK;
}

HRESULT CUI_Object::Load_binary(ifstream& in)
{
	return E_FAIL;
}

HRESULT CUI_Object::Show_UI()
{
	return E_FAIL;
}

HRESULT CUI_Object::Close_UI()
{
	return E_FAIL;
}


void CUI_Object::Free()
{
	__super::Free();
}
