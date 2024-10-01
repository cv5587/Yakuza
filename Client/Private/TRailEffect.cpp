#include "TRailEffect.h"
#include "GameInstance.h"

CTRailEffect::CTRailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect{ pDevice , pContext }
{
}

CTRailEffect::CTRailEffect(const CTRailEffect& rhs)
	:CEffect{ rhs },
	m_TrailDesc{ rhs.m_TrailDesc }
{
}

HRESULT CTRailEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTRailEffect::Initialize_Prototype(string strFilePath)
{
	if (FAILED(Load_Data(strFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTRailEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

		if (nullptr == pDesc->pWorldMatrix)
		{
			TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);

			m_TrailDesc = pDesc->Trail_Desc;
		}
		else
		{
			m_pWorldMatrix = pDesc->pWorldMatrix;
		}
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Speed(3.f);

	return S_OK;
}

void CTRailEffect::Priority_Tick(const _float& fTimeDelta)
{
}

void CTRailEffect::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float4x4 TrailMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	m_pVIBufferCom->Add_Trail(fTimeDelta, XMLoadFloat4x4(&TrailMatrix));
}

void CTRailEffect::Late_Tick(const _float& fTimeDelta)
{
	//Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
}

HRESULT CTRailEffect::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void* CTRailEffect::Get_Instance()
{
	return &m_TrailDesc;
}

void CTRailEffect::Reset_Buffer()
{
	m_pVIBufferCom->Reset_Trail();
}

HRESULT CTRailEffect::Save_Data(const string strDirectory)
{
	string Directory = strDirectory;
	string ParticleTag = m_pGameInstance->WstringToString(m_ParticleTag);
	string TextureTag = m_pGameInstance->WstringToString(m_TextureTag);

	string headTag = "Prototype_GameObject_Particle_Trail_";
	Directory += "/" + headTag + ParticleTag + ".dat";

	ofstream out(Directory, ios::binary);

	out.sync_with_stdio(false);

	out.write((char*)&m_eType, sizeof(_uint));

	_int strlength = ParticleTag.length();
	out.write((char*)&strlength, sizeof(_int));
	out.write(ParticleTag.c_str(), strlength);

	_int strTexturelength = TextureTag.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(TextureTag.c_str(), strTexturelength);

	out.write((char*)&m_iShaderPass, sizeof(_int));

	out.write((char*)&m_TrailDesc.iMaxTrail, sizeof(_uint));
	out.write((char*)&m_TrailDesc.vInitPosA, sizeof(_float3));
	out.write((char*)&m_TrailDesc.vInitPosB, sizeof(_float3));

	out.flush();

	out.close();

	return S_OK;
}

HRESULT CTRailEffect::Load_Data(const string strDirectory)
{

	string Directory = strDirectory;

	ifstream in(Directory, ios::binary);

	in.sync_with_stdio(false);

	if (!in.is_open()) {
		MSG_BOX("파일 개방 실패");
		return E_FAIL;
	}

	string strFileName = m_pGameInstance->Get_FileName(Directory);

	in.read((char*)&m_eType, sizeof(_uint));

	_int strlength;
	char charparticleTag[MAX_PATH] = {};

	in.read((char*)&strlength, sizeof(_int));

	in.read(charparticleTag, strlength);
	string tag = charparticleTag;
	m_ParticleTag = m_pGameInstance->StringToWstring(tag);

	_int strTexturelength;
	char charTextureTag[MAX_PATH] = {};

	in.read((char*)&strTexturelength, sizeof(_int));

	in.read(charTextureTag, strTexturelength);
	string textag = charTextureTag;
	m_TextureTag = m_pGameInstance->StringToWstring(textag);

	in.read((char*)&m_iShaderPass, sizeof(_int));

	in.read((char*)&m_TrailDesc.iMaxTrail, sizeof(_uint));
	in.read((char*)&m_TrailDesc.vInitPosA, sizeof(_float3));
	in.read((char*)&m_TrailDesc.vInitPosB, sizeof(_float3));

	in.close();

	return S_OK;
}

HRESULT CTRailEffect::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &m_TrailDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Trail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_TextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CTRailEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}


CTRailEffect* CTRailEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTRailEffect* pInstance = new CTRailEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTRailEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTRailEffect* CTRailEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath)
{
	CTRailEffect* pInstance = new CTRailEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
	{
		MSG_BOX("Failed To Created : CTRailEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTRailEffect::Clone(void* pArg)
{
	CTRailEffect* pInstance = new CTRailEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTRailEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTRailEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);
}
