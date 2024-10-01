#include "Group.h"

#include "GameInstance.h"

#ifdef _TOOL
#include "Object_Manager.h"
#endif // _TOOL

#include "Image_Texture.h"
#include "Text.h"
#include "Btn.h"
#include "UI_Effect.h"	

CGroup::CGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Object{ pDevice, pContext}
{
}

CGroup::CGroup(const CGroup& rhs)
	: CUI_Object{ rhs },
	m_GroupWorld{rhs.m_GroupWorld}
{
	for (auto& iter : rhs.m_PartObjects)
	{
		CUI_Object* pUI = dynamic_cast<CUI_Object*>(iter->Clone(nullptr));
		m_PartObjects.emplace_back(pUI);
	}

}

void CGroup::Set_isPlay(_bool isPlay)
{
	for (auto& iter : m_PartObjects)
		iter->Set_isPlay(isPlay);
}

void CGroup::Remove_PartObject(_uint iIndex)
{
	auto iter = m_PartObjects.begin();

	Safe_Release(m_PartObjects[iIndex]);

	m_PartObjects.erase(iter + iIndex);

}

HRESULT CGroup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGroup::Initialize_Prototype(ifstream& in)
{
	//if (FAILED(__super::Initialize(nullptr)))
	//	return E_FAIL;

	if (FAILED(Load_Groupbinary(in)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGroup::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_GroupWorld));
	for (auto& iter : m_PartObjects)
	{
		dynamic_cast<CUI_Texture*>(iter)->Set_pParentWorld(m_pTransformCom->Get_WorldFloat4x4());
	}

	return S_OK;
}

void CGroup::Priority_Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Priority_Tick(fTimeDelta);
}

void CGroup::Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Tick(fTimeDelta);
}

void CGroup::Late_Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Late_Tick(fTimeDelta);
}

HRESULT CGroup::Render()
{
//	for (auto& pObject : m_PartObjects)
	//	pObject->Render();

	return S_OK;
}

void CGroup::Show_Choice(_int Index)
{

	for (size_t i = 0; i < m_PartObjects.size(); i++)
	{
		if (Index == i)
			m_PartObjects[i]->Set_isPlay(true);
		else
			m_PartObjects[i]->Set_isPlay(false);
	}
}

void CGroup::Show_Off_All()
{
	for (size_t i = 0; i < m_PartObjects.size(); i++)
	{
		m_PartObjects[i]->Set_isPlay(false);
	}
}

void CGroup::Show_On_All()
{
	for (size_t i = 0; i < m_PartObjects.size(); i++)
	{
		m_PartObjects[i]->Set_isPlay(true);
	}
}

void CGroup::Show_On(_int Index)
{
	if (m_PartObjects.size() <= Index) return;

	m_PartObjects[Index]->Set_isPlay(true);
}

void CGroup::Show_Off(_int Index)
{
	if (m_PartObjects.size() <= Index) return;

	m_PartObjects[Index]->Set_isPlay(false);
}


HRESULT CGroup::Save_binary(const string strDirectory)
{

	return S_OK;
}

HRESULT CGroup::Save_Groupbinary(ofstream& out)
{
	_int size = m_PartObjects.size();
	out.write((char*)&size, sizeof(_int));

	_float4x4 GroupWorld = *m_pTransformCom->Get_WorldFloat4x4();
	out.write((char*)&GroupWorld, sizeof(_float4x4));

	for (auto& pObject : m_PartObjects)
	{
		pObject->Save_Groupbinary(out);
	}

	return S_OK;
}

HRESULT CGroup::Load_Groupbinary(ifstream& in)
{
	_int strTexturelength;
	char charBox[MAX_PATH] = {};
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	m_strName = charBox;

	_int size;
	in.read((char*)&size, sizeof(_int));

	in.read((char*)&m_GroupWorld, sizeof(_float4x4));

	in.read((char*)&m_isEvent, sizeof(_bool));

	for (_int i = 0; i < size; i++)
	{
		_uint Type;
		in.read((char*)&Type, sizeof(_uint));

		switch (Type)
		{
		case 0:
		{
			CImage_Texture::tUITextureDesc pDesc{};
			pDesc.iTypeIndex = Type;

			_int strTexturelength;
			char charBox[MAX_PATH] = {};
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strName = charBox;

			in.read((char*)&pDesc.isParent, sizeof(_bool));

			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strParentName = charBox;

			string path;
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

			in.read((char*)&pDesc.fStartUV, sizeof(_float2));
			in.read((char*)&pDesc.fEndUV, sizeof(_float2));
			in.read((char*)&pDesc.vColor, sizeof(_float4));
			in.read((char*)&pDesc.isColor, sizeof(_bool));
			in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
			in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

			in.read((char*)&pDesc.bAnim, sizeof(_bool));
			in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
			in.read((char*)&pDesc.vStartPos, sizeof(_float3));
			in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
			in.read((char*)&pDesc.isReverse, sizeof(_bool));

			in.read((char*)&pDesc.isEvent, sizeof(_bool));
			in.read((char*)&pDesc.isScreen, sizeof(_bool));
			in.read((char*)&pDesc.isAnimLoop, sizeof(_bool));


			in.read((char*)&pDesc.vUpPoint, sizeof(_float4));
			if (isnan(pDesc.vUpPoint.x) || isnan(pDesc.vUpPoint.y) || isnan(pDesc.vUpPoint.z) || isnan(pDesc.vUpPoint.w))
				pDesc.vUpPoint = _float4(0.f, 0.f, 0.f, 0.f);

			in.read((char*)&pDesc.vDownPoint, sizeof(_float4));
			if (isnan(pDesc.vDownPoint.x) || isnan(pDesc.vDownPoint.y) || isnan(pDesc.vDownPoint.z) || isnan(pDesc.vDownPoint.w))
				pDesc.vDownPoint = _float4(0.f, 0.f, 0.f, 0.f);

			in.read((char*)&pDesc.vEndColor, sizeof(_float4));
			if (isnan(pDesc.vEndColor.x) || isnan(pDesc.vEndColor.y) || isnan(pDesc.vEndColor.z) || isnan(pDesc.vEndColor.w))
				pDesc.vEndColor = _float4(1.f, 1.f, 1.f, 1.f);

			in.read((char*)&pDesc.vAnimScale, sizeof(_float2));
			if (isnan(pDesc.vAnimScale.x) || isnan(pDesc.vAnimScale.y))
				pDesc.vAnimScale = _float2(1.f, 1.f);

			pDesc.isLoad = true;
			pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

			CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &pDesc));
			if (nullptr == pImage)
				return E_FAIL;

			m_PartObjects.emplace_back(pImage);
		}
		break;

		case 1:
		{
			CBtn::BTN_DESC pDesc{};
			pDesc.iTypeIndex = Type;

			_int strTexturelength;
			char charBox[MAX_PATH] = {};
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strName = charBox;

			in.read((char*)&pDesc.isParent, sizeof(_bool));

			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strParentName = charBox;

			string path;
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

			in.read((char*)&pDesc.fStartUV, sizeof(_float2));
			in.read((char*)&pDesc.fEndUV, sizeof(_float2));
			in.read((char*)&pDesc.vColor, sizeof(_float4));
			in.read((char*)&pDesc.isColor, sizeof(_bool));
			in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
			in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

			in.read((char*)&pDesc.bAnim, sizeof(_bool));
			in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
			in.read((char*)&pDesc.vStartPos, sizeof(_float3));
			in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
			in.read((char*)&pDesc.isReverse, sizeof(_bool));

			in.read((char*)&pDesc.isEvent, sizeof(_bool));
			in.read((char*)&pDesc.isScreen, sizeof(_bool));

			in.read((char*)&pDesc.vEndColor, sizeof(_float4));
			in.read((char*)&pDesc.vAnimScale, sizeof(_float2));

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strClickFilePath = m_pGameInstance->StringToWstring(path);

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strClickFileName = m_pGameInstance->StringToWstring(path);

			in.read((char*)&pDesc.ClickStartUV, sizeof(_float2));
			in.read((char*)&pDesc.ClickEndUV, sizeof(_float2));

			pDesc.isLoad = true;

			//pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
			CBtn* pBtn = dynamic_cast<CBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn"), &pDesc));
			if (nullptr == pBtn)
				return E_FAIL;

			m_PartObjects.emplace_back(pBtn);
		}
		break;

		case 2:
		{
			CText::TEXT_DESC pDesc{};
			pDesc.iTypeIndex = Type;

			_int strTexturelength;
			char charBox[MAX_PATH] = {};
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strName = charBox;

			in.read((char*)&pDesc.isParent, sizeof(_bool));

			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strParentName = charBox;

			string path;

			in.read((char*)&pDesc.vColor, sizeof(_float4));
			in.read((char*)&pDesc.isColor, sizeof(_bool));
			in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
			in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

			in.read((char*)&pDesc.bAnim, sizeof(_bool));
			in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
			in.read((char*)&pDesc.vStartPos, sizeof(_float3));
			in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
			in.read((char*)&pDesc.isReverse, sizeof(_bool));

			in.read((char*)&pDesc.isEvent, sizeof(_bool));
			in.read((char*)&pDesc.isScreen, sizeof(_bool));
			in.read((char*)&pDesc.vEndColor, sizeof(_float4));

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strText = m_pGameInstance->StringToWstring(path);
			in.read((char*)&pDesc.iAlign, sizeof(_uint));

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.Font = m_pGameInstance->StringToWstring(path);

			pDesc.isLoad = true;
			pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
			CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &pDesc));
			if (nullptr == pText)
				return E_FAIL;

			m_PartObjects.emplace_back(pText);
		}
		break;

		case 4:
		{
			CUI_Effect::UI_EFFECT_DESC pDesc{};
			pDesc.iTypeIndex = Type;

			_int strTexturelength;
			char charBox[MAX_PATH] = {};
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strName = charBox;

			in.read((char*)&pDesc.isParent, sizeof(_bool));

			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strParentName = charBox;


			string path;
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

			in.read((char*)&pDesc.fStartUV, sizeof(_float2));
			in.read((char*)&pDesc.fEndUV, sizeof(_float2));
			in.read((char*)&pDesc.vColor, sizeof(_float4));
			in.read((char*)&pDesc.isColor, sizeof(_bool));
			in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
			in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

			in.read((char*)&pDesc.bAnim, sizeof(_bool));
			in.read((char*)&pDesc.isAnimLoop, sizeof(_bool));
			in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
			in.read((char*)&pDesc.vStartPos, sizeof(_float3));

			in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
			in.read((char*)&pDesc.isReverse, sizeof(_bool));

			in.read((char*)&pDesc.isEvent, sizeof(_bool));
			in.read((char*)&pDesc.isScreen, sizeof(_bool));

			in.read((char*)&pDesc.vAnimScale, sizeof(_float2));
			//°³º°
			in.read((char*)&pDesc.vLifeTime, sizeof(_float3));
			in.read((char*)&pDesc.fSpeed, sizeof(_float));
			in.read((char*)&pDesc.isUVAnim, sizeof(_bool));
			pDesc.isLoad = true;
		//	pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
			CUI_Effect* pUIEffect = dynamic_cast<CUI_Effect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIEffect"), &pDesc));
			if (nullptr == pUIEffect)
				return E_FAIL;

			m_PartObjects.emplace_back(pUIEffect);
		}
		break;

		default:
			break;
		}

	}

	return S_OK;
}

HRESULT CGroup::Show_UI()
{
	for (auto& UIObject : m_PartObjects)
	{
		UIObject->Show_UI();
	}
	return S_OK;
}

HRESULT CGroup::Close_UI()
{
	for (auto& UIObject : m_PartObjects)
	{
		UIObject->Close_UI();
	}
	return S_OK;
}

_bool CGroup::Check_AnimFin()
{
	for (auto& UIObject : m_PartObjects)
	{
		if (!UIObject->Check_AnimFin())
			return false;
	}
	return true;
}

_bool CGroup::Click_Intersect(_int Index)
{
	return m_PartObjects[Index]->Click_Intersect();
}



CGroup* CGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGroup* pInstance = new CGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGroup* CGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in)
{
	CGroup* pInstance = new CGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(in)))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CGroup::Clone(void* pArg)
{
	CGroup* pInstance = new CGroup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGroup::Free()
{
	__super::Free();

	for (auto& pObject : m_PartObjects)
		Safe_Release(pObject);
}
