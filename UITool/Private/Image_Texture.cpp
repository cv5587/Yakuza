#include "Image_Texture.h"

#include "GameInstance.h"

CImage_Texture::CImage_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CImage_Texture::CImage_Texture(const CImage_Texture& rhs)
	: CUI_Texture{ rhs }
{
}

HRESULT CImage_Texture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CImage_Texture::Initialize_Prototype(ifstream& in)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Load_binary(in)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CImage_Texture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CImage_Texture::Priority_Tick(const _float& fTimeDelta)
{
}

void CImage_Texture::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CImage_Texture::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	//일단 클라에서 어케 쓸지 정하고 따라서 만들자
}

HRESULT CImage_Texture::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;


	m_pShaderCom->Begin(m_iShaderPass);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CImage_Texture::Save_binary(const string strDirectory)
{
	string Directory = strDirectory;

	Directory = Directory + m_strName+".dat";
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

	out.write((char*)&m_isEvent, sizeof(_bool));

	out.write((char*)&m_isScreen, sizeof(_bool));

	out.write((char*)&m_isAnimLoop, sizeof(_bool));

	out.write((char*)&m_fUpPoint, sizeof(_float4));

	out.write((char*)&m_fDownPoint, sizeof(_float4));

	out.write((char*)&m_vEndColor, sizeof(_float4));

	out.write((char*)&m_vAnimScale, sizeof(_float2));

	out.close();

	return S_OK;
}

HRESULT CImage_Texture::Save_Groupbinary( ofstream& out)
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

	out.write((char*)&m_isAnimLoop, sizeof(_bool));

	out.write((char*)&m_fUpPoint, sizeof(_float4));

	out.write((char*)&m_fDownPoint, sizeof(_float4));

	out.write((char*)&m_vEndColor, sizeof(_float4));

	out.write((char*)&m_vAnimScale, sizeof(_float2));
	return S_OK;
}

HRESULT CImage_Texture::Load_binary(ifstream& in)
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

	in.read((char*)&m_isAnimLoop, sizeof(_bool));

	in.read((char*)&m_fUpPoint, sizeof(_float4));
	if (isnan(m_fUpPoint.x))
		m_fUpPoint = _float4(0.f, 0.f, 0.f, 0.f);

	in.read((char*)&m_fDownPoint, sizeof(_float4));
	if (isnan(m_fDownPoint.x))
		m_fDownPoint = _float4(0.f, 0.f, 0.f, 0.f);

	in.read((char*)&m_vEndColor, sizeof(_float4));
	if (isnan(m_vEndColor.x)|| isnan(m_vEndColor.y)|| isnan(m_vEndColor.z)|| isnan(m_vEndColor.w))
		m_vEndColor = _float4(1.f, 1.f, 1.f, 1.f);

	in.read((char*)&m_vAnimScale, sizeof(_float2));
	if (isnan(m_vAnimScale.x) || isnan(m_vAnimScale.y))
		m_vAnimScale = _float2(1.f, 1.f);

	in.close();


	return S_OK;
}

CImage_Texture* CImage_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImage_Texture* pInstance = new CImage_Texture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CImage_Texture* CImage_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in)
{
	CImage_Texture* pInstance = new CImage_Texture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(in)))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CImage_Texture::Clone(void* pArg)
{
	CImage_Texture* pInstance = new CImage_Texture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CImage_Texture::Free()
{
	__super::Free();
}
