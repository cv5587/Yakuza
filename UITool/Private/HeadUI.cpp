#include "HeadUI.h"
#include "GameInstance.h"
CHeadUI::CHeadUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CHeadUI::CHeadUI(const CHeadUI& rhs)
	: CUI_Texture{ rhs }
{
}

HRESULT CHeadUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHeadUI::Initialize_Prototype(ifstream& in)
{

	if (FAILED(Load_binary(in)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHeadUI::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHeadUI::Priority_Tick(const _float& fTimeDelta)
{
}

void CHeadUI::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CHeadUI::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CHeadUI::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;


	m_pShaderCom->Begin(m_iShaderPass);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CHeadUI::Add_Components(void* pArg)
{
	CVIBuffer_Rect::RECT_DESC Desc;

	Desc.fStartUV = m_fStartUV;
	Desc.fEndUV = m_fEndUV;
	Desc.vUpPoint = m_fUpPoint;
	Desc.vDownPoint = m_fDownPoint;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, m_strTextureFilePath, 1);
	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CHeadUI::Bind_ShaderResources()
{
	if (m_isParent)
	{
		_float4x4 ResultWorld;
		XMStoreFloat4x4(&ResultWorld, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
		//XMStoreFloat4x4(&ResultWorld, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(m_pParentMatrix));

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ResultWorld)))
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_AnimScale", &m_vAnimScale, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimTime", &m_fAnimTime, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fControlAlpha", &m_fControlAlpha, sizeof(_float2))))
			return E_FAIL;

	}

	if (m_isColor)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &m_vEndColor, sizeof(_float4))))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHeadUI::Save_binary(const string strDirectory)
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
	out.write((char*)&AnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));

	out.write((char*)&m_fControlAlpha, sizeof(_float2));

	out.write((char*)&m_isReverse, sizeof(_bool));

	out.close();

	return S_OK;
}

HRESULT CHeadUI::Save_Groupbinary(ofstream& out)
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

	_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	out.write((char*)&WorldMatrix, sizeof(_float4x4));

	out.write((char*)&m_isAnim, sizeof(_bool));

	_float2 AnimTime = { 0.f , m_fAnimTime.y };
	out.write((char*)&AnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));

	out.write((char*)&m_fControlAlpha, sizeof(_float2));

	out.write((char*)&m_isReverse, sizeof(_bool));
	return S_OK;
}

HRESULT CHeadUI::Load_binary(ifstream& in)
{
	m_iTypeIndex = 0;

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

	in.read((char*)&m_WorldMatrix, sizeof(_float4x4));

	in.read((char*)&m_isAnim, sizeof(_bool));
	in.read((char*)&m_fAnimTime, sizeof(_float2));
	in.read((char*)&m_vStartPos, sizeof(_float3));

	in.read((char*)&m_fControlAlpha, sizeof(_float2));
	in.read((char*)&m_isReverse, sizeof(_bool));
	in.read((char*)&m_isEvent, sizeof(_bool));
	in.read((char*)&m_isScreen, sizeof(_bool));
	in.read((char*)&m_isAnimLoop, sizeof(_bool));

	in.read((char*)&m_fUpPoint, sizeof(_float4));
	if (isnan(m_fUpPoint.x))
		m_fUpPoint = _float4(0.f, 0.f, 0.f, 0.f);

	in.read((char*)&m_fDownPoint, sizeof(_float4));
	if (isnan(m_fDownPoint.x))
		m_fDownPoint = _float4(0.f, 0.f, 0.f, 0.f);

	in.read((char*)&m_vEndColor, sizeof(_float4));
	if (isnan(m_vEndColor.x) || isnan(m_vEndColor.y) || isnan(m_vEndColor.z) || isnan(m_vEndColor.w))
		m_vEndColor = _float4(1.f, 1.f, 1.f, 1.f);

	in.read((char*)&m_vAnimScale, sizeof(_float2));
	if (isnan(m_vAnimScale.x) || isnan(m_vAnimScale.y))
		m_vAnimScale = _float2(1.f, 1.f);

	in.close();


	return S_OK;
}

CHeadUI* CHeadUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHeadUI* pInstance = new CHeadUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CHeadUI* CHeadUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in)
{
	CHeadUI* pInstance = new CHeadUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(in)))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CHeadUI::Clone(void* pArg)
{
	CGameObject* pInstance = new CHeadUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHeadUI::Free()
{
	__super::Free();
}
