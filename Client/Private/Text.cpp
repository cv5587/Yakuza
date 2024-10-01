#include "Text.h"

#include "GameInstance.h"

CText::CText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CText::CText(const CText& rhs)
	: CUI_Texture{ rhs },
	m_strText{ rhs.m_strText },
	m_iAlign{rhs.m_iAlign},
	m_Font{rhs.m_Font}
{
}

HRESULT CText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CText::Initialize_Prototype(ifstream& in)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Load_binary(in)))
		return E_FAIL;

	return S_OK;
}

HRESULT CText::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		TEXT_DESC* pDesc = static_cast<TEXT_DESC*>(pArg);
		m_strText = pDesc->strText;
		m_iAlign = pDesc->iAlign;
		m_Font = pDesc->Font;
	}



	return S_OK;
}

void CText::Priority_Tick(const _float& fTimeDelta)
{
}

void CText::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CText::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CText::Render()
{
	_float4 vPos;
	if (m_isParent)
	{
		XMStoreFloat4(&vPos, (XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(m_pParentMatrix)).r[3]);
	}
	else
	{
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

	_float2 vFontPos = _float2(vPos.x + g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f);
	//0중간 , 1 우측 ,2 좌측

	_float4 vColor;
	if (m_isAnim)
	{
		_float factor = m_fAnimTime.x / m_fAnimTime.y;
		_vector lerpColor = XMVectorLerp(XMLoadFloat4(&m_vColor), XMLoadFloat4(&m_vEndColor), factor);
		_float AnimAlpha = m_pGameInstance->Lerp(m_fControlAlpha.x, m_fControlAlpha.y, factor);


		_vector LerpPosition = XMVectorLerp(XMVectorSet(m_vStartPos.x + vFontPos.x, -m_vStartPos.y + vFontPos.y, 0.f, 1.f), XMVectorSet(vFontPos.x, vFontPos.y, 0.f, 0.f), factor);

		vFontPos.x = XMVectorGetX(LerpPosition);
		vFontPos.y = XMVectorGetY(LerpPosition);
		XMStoreFloat4(&vColor, XMVectorScale(lerpColor, AnimAlpha));

	}
	else
	{
		vColor = m_vColor;
	}
	m_pGameInstance->AlignRender_Font(m_Font, m_strText, vFontPos, XMLoadFloat4(&vColor), m_iAlign);

	return S_OK;
}

HRESULT CText::Save_binary(const string strDirectory)
{
	string Directory = strDirectory;

	Directory = Directory + m_strName + ".dat";
	ofstream out(Directory, ios::binary);
	//공용저장
	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);

	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_isColor, sizeof(_bool));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	out.write((char*)&WorldMatrix, sizeof(_float4x4));


	out.write((char*)&m_isAnim, sizeof(_bool));

	_float2 AnimTime = { 0.f , m_fAnimTime.y };
	out.write((char*)&m_fAnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));

	out.write((char*)&m_fControlAlpha, sizeof(_float2));

	out.write((char*)&m_isReverse, sizeof(_bool));

	out.write((char*)&m_isEvent, sizeof(_bool));

	out.write((char*)&m_isScreen, sizeof(_bool));

	out.write((char*)&m_vEndColor, sizeof(_float4));
	//개별적인 저장

	string Text = m_pGameInstance->WstringToString(m_strText);
	strTexturelength = Text.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(Text.c_str(), strTexturelength);

	out.write((char*)&m_iAlign, sizeof(_uint));

	string Font = m_pGameInstance->WstringToString(m_Font);
	strTexturelength = Font.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(Font.c_str(), strTexturelength);


	out.close();

	return S_OK;
}

HRESULT CText::Save_Groupbinary(ofstream& out)
{
	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);


	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_isColor, sizeof(_bool));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	out.write((char*)&WorldMatrix, sizeof(_float4x4));

	out.write((char*)&m_isAnim, sizeof(_bool));

	_float2 AnimTime = { 0.f , m_fAnimTime.y };
	out.write((char*)&AnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));

	out.write((char*)&m_fControlAlpha, sizeof(_float2));

	out.write((char*)&m_isReverse, sizeof(_bool));

	out.write((char*)&m_isEvent, sizeof(_bool));

	out.write((char*)&m_isScreen, sizeof(_bool));

	out.write((char*)&m_vEndColor, sizeof(_float4));
	//개별적인 저장

	string Text = m_pGameInstance->WstringToString(m_strText);
	strTexturelength = Text.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(Text.c_str(), strTexturelength);
	out.write((char*)&m_iAlign, sizeof(_uint));

	string Font = m_pGameInstance->WstringToString(m_Font);
	strTexturelength = Font.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(Font.c_str(), strTexturelength);

	return S_OK;
}

HRESULT CText::Load_binary(ifstream& in)
{
	m_iTypeIndex = 3;

	_int strTexturelength;
	char charBox[MAX_PATH] = {};
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	m_strName = charBox;

	in.read((char*)&m_isParent, sizeof(_bool));

	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	m_strParentName = charBox;

	string path;

	in.read((char*)&m_vColor, sizeof(_float4));
	in.read((char*)&m_isColor, sizeof(_bool));
	in.read((char*)&m_iShaderPass, sizeof(_uint));

	in.read((char*)&m_WorldMatrix, sizeof(_float4x4));

	in.read((char*)&m_isAnim, sizeof(_bool));
	in.read((char*)&m_fAnimTime, sizeof(_float2));
	in.read((char*)&m_vStartPos, sizeof(_float3));

	in.read((char*)&m_fControlAlpha, sizeof(_float2));
	in.read((char*)&m_isReverse, sizeof(_bool));
	in.read((char*)&m_isEvent, sizeof(_bool));
	in.read((char*)&m_isScreen, sizeof(_bool));
	in.read((char*)&m_vEndColor, sizeof(_float4));


	//개별
	ZeroMemory(charBox, MAX_PATH);
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	path = charBox;
	m_strText = m_pGameInstance->StringToWstring(path);

	in.read((char*)&m_iAlign, sizeof(_uint));

	ZeroMemory(charBox, MAX_PATH);
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	path = charBox;
	m_Font = m_pGameInstance->StringToWstring(path);


	in.close();

	return S_OK;
}

CText* CText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CText* pInstance = new CText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CText* CText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in)
{
	CText* pInstance = new CText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(in)))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CText::Clone(void* pArg)
{
	CText* pInstance = new CText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CText::Free()
{
	__super::Free();
}
