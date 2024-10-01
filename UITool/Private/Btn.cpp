#include "Btn.h"

#include "GameInstance.h"

CBtn::CBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext}
{
}

CBtn::CBtn(const CBtn& rhs)
	: CUI_Texture{ rhs },
	m_strClickFilePath{rhs.m_strClickFilePath },
	m_StrClickFileName{rhs.m_StrClickFileName },
	m_ClickStartUV{rhs.m_ClickStartUV },
	m_ClickEndUV{rhs.m_ClickEndUV },
	m_vClickColor{rhs.m_vClickColor}
{
}

HRESULT CBtn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBtn::Initialize_Prototype(ifstream& in)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Load_binary(in)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	return S_OK;
}

HRESULT CBtn::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		BTN_DESC* pDesc = static_cast<BTN_DESC*>(pArg);
		m_strClickFilePath = pDesc->strClickFilePath;
		m_StrClickFileName = pDesc->strClickFileName;
		m_ClickStartUV = pDesc->ClickStartUV;
		m_ClickEndUV = pDesc->ClickEndUV;
		m_vClickColor = pDesc->vClickColor;
	}


	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBtn::Priority_Tick(const _float& fTimeDelta)
{
}

void CBtn::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBtn::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
#ifndef _TOOL
	//커서 확인
	m_isClick = m_pGameInstance->Picking_UI(m_pTransformCom);
#endif // _TOOL
}

HRESULT CBtn::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;



	m_pShaderCom->Begin(m_iShaderPass);

	if (m_isClick)
		m_pClickVIBufferCom->Render();
	else
		m_pVIBufferCom->Render();	

	return S_OK;
}

HRESULT CBtn::Chage_ClickUV(_float2 StartUV, _float2 EndUV)
{
	m_ClickStartUV = StartUV;
	m_ClickEndUV = EndUV;

	if (FAILED(m_pClickVIBufferCom->EditUV(m_ClickStartUV, m_ClickEndUV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBtn::Save_binary(const string strDirectory)
{

	string Directory = strDirectory;

	Directory = Directory + m_strName + ".dat";
	ofstream out(Directory, ios::binary);

	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);

	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	string FilePath = m_pGameInstance->WstringToString(m_strTextureFilePath);
	strTexturelength = FilePath.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FilePath.c_str(), strTexturelength);

	string FileName = m_pGameInstance->WstringToString(m_strTextureName);
	strTexturelength = FileName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FileName.c_str(), strTexturelength);

	out.write((char*)&m_fStartUV, sizeof(_float2));

	out.write((char*)&m_fEndUV, sizeof(_float2));

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

	out.write((char*)&m_vAnimScale, sizeof(_float2));
	//개별 저장

	string ClickFilePath = m_pGameInstance->WstringToString(m_strClickFilePath);
	strTexturelength = ClickFilePath.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(ClickFilePath.c_str(), strTexturelength);

	string ClickFileName = m_pGameInstance->WstringToString(m_StrClickFileName);
	strTexturelength = ClickFileName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(ClickFileName.c_str(), strTexturelength);

	out.write((char*)&m_ClickStartUV, sizeof(_float2));

	out.write((char*)&m_ClickEndUV, sizeof(_float2));

	out.write((char*)&m_vClickColor, sizeof(_float4));//20240722생성


	out.close();


	return S_OK;
}

HRESULT CBtn::Save_Groupbinary(ofstream& out)
{

	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);

	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	string FilePath = m_pGameInstance->WstringToString(m_strTextureFilePath);
	strTexturelength = FilePath.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FilePath.c_str(), strTexturelength);

	string FileName = m_pGameInstance->WstringToString(m_strTextureName);
	strTexturelength = FileName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FileName.c_str(), strTexturelength);

	out.write((char*)&m_fStartUV, sizeof(_float2));

	out.write((char*)&m_fEndUV, sizeof(_float2));

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_isColor, sizeof(_bool));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	//_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	out.write((char*)&m_WorldMatrix, sizeof(_float4x4));

	out.write((char*)&m_isAnim, sizeof(_bool));

	_float2 AnimTime = { 0.f , m_fAnimTime.y };
	out.write((char*)&AnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));

	out.write((char*)&m_fControlAlpha, sizeof(_float2));

	out.write((char*)&m_isReverse, sizeof(_bool));

	out.write((char*)&m_isEvent, sizeof(_bool));

	out.write((char*)&m_isScreen, sizeof(_bool));

	out.write((char*)&m_vEndColor, sizeof(_float4));

	out.write((char*)&m_vAnimScale, sizeof(_float2));
	
	//개별 저장

	string ClickFilePath = m_pGameInstance->WstringToString(m_strClickFilePath);
	strTexturelength = ClickFilePath.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(ClickFilePath.c_str(), strTexturelength);

	string ClickFileName = m_pGameInstance->WstringToString(m_StrClickFileName);
	strTexturelength = ClickFileName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(ClickFileName.c_str(), strTexturelength);

	out.write((char*)&m_ClickStartUV, sizeof(_float2));

	out.write((char*)&m_ClickEndUV, sizeof(_float2));

	out.write((char*)&m_vClickColor, sizeof(_float4));//20240722생성

	return S_OK;
}

HRESULT CBtn::Load_binary(ifstream& in)
{

	CBtn::BTN_DESC pDesc{};
	m_iTypeIndex = 1;

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
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	path = charBox;
	m_strTextureFilePath = m_pGameInstance->StringToWstring(path);

	ZeroMemory(charBox, MAX_PATH);
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	path = charBox;
	m_strTextureName = m_pGameInstance->StringToWstring(path);

	in.read((char*)&m_fStartUV, sizeof(_float2));
	in.read((char*)&m_fEndUV, sizeof(_float2));
	in.read((char*)&m_vColor, sizeof(_float4));
	in.read((char*)&m_isColor, sizeof(_bool));
	in.read((char*)&m_iShaderPass, sizeof(_uint));

	_float4x4 World{};
	in.read((char*)&World, sizeof(_float4x4));
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&World));

	in.read((char*)&m_isAnim, sizeof(_bool));
	in.read((char*)&m_fAnimTime, sizeof(_float2));
	in.read((char*)&m_vStartPos, sizeof(_float3));

	in.read((char*)&m_fControlAlpha, sizeof(_float2));
	in.read((char*)&m_isReverse, sizeof(_bool));

	in.read((char*)&m_isEvent, sizeof(_bool));
	in.read((char*)&m_isScreen, sizeof(_bool));
	in.read((char*)&m_vEndColor, sizeof(_float4));
	in.read((char*)&m_vAnimScale, sizeof(_float2));

	//개별
	ZeroMemory(charBox, MAX_PATH);
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	path = charBox;
	m_strClickFilePath = m_pGameInstance->StringToWstring(path);

	ZeroMemory(charBox, MAX_PATH);
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	path = charBox;
	m_StrClickFileName = m_pGameInstance->StringToWstring(path);

	in.read((char*)&m_ClickStartUV, sizeof(_float2));
	in.read((char*)&m_ClickEndUV, sizeof(_float2));

	in.read((char*)&m_vClickColor, sizeof(_float4));

	in.close();

	return S_OK;
}

HRESULT CBtn::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	CVIBuffer_Rect::RECT_DESC Desc;

	Desc.fStartUV = m_ClickStartUV;
	Desc.fEndUV = m_ClickEndUV;
	Desc.vUpPoint = m_fUpPoint;
	Desc.vDownPoint = m_fDownPoint;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_ClickVIBuffer"), reinterpret_cast<CComponent**>(&m_pClickVIBufferCom),&Desc)))
		return E_FAIL;

	//m_pClickVIBufferCom->EditUV(m_ClickStartUV, m_ClickEndUV);

	m_pClickTextureCom = CTexture::Create(m_pDevice, m_pContext, m_strClickFilePath, 1);
	if (nullptr == m_pClickTextureCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CBtn::Bind_ResourceData()
{
	if (m_isParent)
	{
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}

	if (m_isAnim)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartPos", &m_vStartPos, sizeof(_float3))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimTime", &m_fAnimTime, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fControlAlpha", &m_fControlAlpha, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_AnimScale", &m_vAnimScale, sizeof(_float2))))
			return E_FAIL;
	}



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (m_isClick)
	{
		if (FAILED(m_pClickTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
		if (m_isColor)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vClickColor, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &m_vEndColor, sizeof(_float4))))
				return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
		if (m_isColor)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &m_vEndColor, sizeof(_float4))))
				return E_FAIL;
		}
	}

	return S_OK;
}

CBtn* CBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBtn* pInstance = new CBtn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBtn* CBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in)
{
	CBtn* pInstance = new CBtn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(in)))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CBtn::Clone(void* pArg)
{
	CBtn* pInstance = new CBtn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBtn::Free()
{
	__super::Free();
	
	Safe_Release(m_pClickTextureCom);
	Safe_Release(m_pClickVIBufferCom);
}
