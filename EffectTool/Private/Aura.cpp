#include "Aura.h"
#include "GameInstance.h"

CAura::CAura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect{ pDevice , pContext }
{
}

CAura::CAura(const CAura& rhs)
	:CEffect{ rhs },
	m_BufferInstance{ rhs.m_BufferInstance },
	m_fUVCount{ rhs.m_fUVCount },
	m_fToneUVCount{ rhs.m_fToneUVCount },
	m_ToneTextureTag{rhs.m_ToneTextureTag },
	m_fFlowPow{rhs.m_fFlowPow },
	m_fFlowSpeed{rhs.m_fFlowSpeed }
{
}

HRESULT CAura::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAura::Initialize_Prototype(string strFilePath)
{
	if (FAILED(Load_Data(strFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAura::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);


		if (nullptr == pDesc->pWorldMatrix)
		{
			AURA_DESC* pDesc = static_cast<AURA_DESC*>(pArg);
			m_BufferInstance = pDesc->BufferInstance;
			m_fUVCount = pDesc->fUVCount;
			m_fToneUVCount = pDesc->fToneUVCount;
			m_ToneTextureTag = pDesc->ToneTextureTag;
			m_fFlowPow = pDesc->fFlowPow;
			m_fFlowSpeed = pDesc->fFlowSpeed;
		}
		else
		{
			m_pWorldMatrix = pDesc->pWorldMatrix;
			m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pWorldMatrix));
		}
	}

#ifdef _TOOL
	m_BufferInstance.WorldMatrix = m_pWorldMatrix;
#else
	m_BufferInstance.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
#endif // _TOOL


	//m_fUVCount = _float2(64.f, 1.f);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CAura::Priority_Tick(const _float& fTimeDelta)
{
}

void CAura::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fCurTime += fTimeDelta;
	if (!m_BufferInstance.isLoop)
	{
		_float fTotalTime = m_fStartTime + m_BufferInstance.vLifeTime.y;
		if (m_fCurTime > fTotalTime)
			m_isDead = true;
	}

	if (m_iAction & iAction[ACTION_SPREAD])
	{
		m_pVIBufferCom->Spread(fTimeDelta);//스타트
	}
	if (m_iAction & iAction[ACTION_SIZEUP])
	{
		m_pVIBufferCom->SizeUp_Time(fTimeDelta);
	}
	if (m_iAction & iAction[ACTION_SIZEDOWN])
	{
		m_pVIBufferCom->SizeDown_Time(fTimeDelta);
	}


}

void CAura::Late_Tick(const _float& fTimeDelta)
{
	if (m_BufferInstance.isLoop)
	{
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
	}
	else
	{
		if (m_fCurTime >= m_fStartTime && !m_isDead)
		{
			m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
		}
	}
}

HRESULT CAura::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iShaderPass);

	m_pVIBufferCom->Render();

	return S_OK;
}

void* CAura::Get_Instance()
{
	return &m_BufferInstance;
}

void CAura::Reset_Buffer()
{
	m_pVIBufferCom->Reset();
}

HRESULT CAura::Save_Data(const string strDirectory)
{

	string Directory = strDirectory;
	string ParticleTag = m_pGameInstance->WstringToString(m_ParticleTag);
	string TextureTag = m_pGameInstance->WstringToString(m_TextureTag);
	string ToneTextureTag = m_pGameInstance->WstringToString(m_ToneTextureTag);

	string headTag = "Prototype_GameObject_Particle_Aura_";
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

	_int strToneTexturelength = ToneTextureTag.length();
	out.write((char*)&strToneTexturelength, sizeof(_int));
	out.write(ToneTextureTag.c_str(), strToneTexturelength);


	out.write((char*)&m_iShaderPass, sizeof(_int));

	out.write((char*)&m_fStartTime, sizeof(_float));

	out.write((char*)&m_vStartPos, sizeof(_float4));

	out.write((char*)&m_fLifeAlpha, sizeof(_float2));

	out.write((char*)&m_iAction, sizeof(_uint));

	out.write((char*)&m_fUVCount, sizeof(_float2));

	out.write((char*)&m_fToneUVCount, sizeof(_float2));

	out.write((char*)&m_fFlowPow, sizeof(_float));

	out.write((char*)&m_fFlowSpeed, sizeof(_float));

	out.write((char*)&m_BufferInstance.iNumInstance, sizeof(_uint));
	out.write((char*)&m_BufferInstance.vOffsetPos, sizeof(_float3));
	out.write((char*)&m_BufferInstance.vPivotPos, sizeof(_float3));
	out.write((char*)&m_BufferInstance.bRadius, sizeof(_bool));
	out.write((char*)&m_BufferInstance.vRange, sizeof(_float3));
	out.write((char*)&m_BufferInstance.fRadius, sizeof(_float));
	out.write((char*)&m_BufferInstance.vSize, sizeof(_float2));
	out.write((char*)&m_BufferInstance.vRectSize, sizeof(_float2));
	out.write((char*)&m_BufferInstance.vSpeed, sizeof(_float2));
	out.write((char*)&m_BufferInstance.vLifeTime, sizeof(_float2));
	out.write((char*)&m_BufferInstance.isLoop, sizeof(_bool));
	out.write((char*)&m_BufferInstance.isAttach, sizeof(_bool));


	if (2 == m_iShaderPass)
	{
		out.write((char*)&m_vStartColor, sizeof(_float4));
		out.write((char*)&m_vEndColor, sizeof(_float4));
	}

	out.close();

	return S_OK;
}

HRESULT CAura::Load_Data(const string strDirectory)
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

	_int strToneTexturelength;
	char charToneTextureTag[MAX_PATH] = {};

	in.read((char*)&strToneTexturelength, sizeof(_int));

	in.read(charToneTextureTag, strToneTexturelength);
	string Tonetextag = charToneTextureTag;
	m_ToneTextureTag = m_pGameInstance->StringToWstring(Tonetextag);

	in.read((char*)&m_iShaderPass, sizeof(_int));

	in.read((char*)&m_fStartTime, sizeof(_float));

	in.read((char*)&m_vStartPos, sizeof(_float4));

	in.read((char*)&m_fLifeAlpha, sizeof(_float2));
	
	in.read((char*)&m_iAction, sizeof(_uint));

	in.read((char*)&m_fUVCount, sizeof(_float2));

	in.read((char*)&m_fToneUVCount, sizeof(_float2));

	in.read((char*)&m_fFlowPow, sizeof(_float));

	in.read((char*)&m_fFlowSpeed, sizeof(_float));




	in.read((char*)&m_BufferInstance.iNumInstance, sizeof(_uint));
	in.read((char*)&m_BufferInstance.vOffsetPos, sizeof(_float3));
	in.read((char*)&m_BufferInstance.vPivotPos, sizeof(_float3));
	in.read((char*)&m_BufferInstance.bRadius, sizeof(_bool));
	in.read((char*)&m_BufferInstance.vRange, sizeof(_float3));
	in.read((char*)&m_BufferInstance.fRadius, sizeof(_float));
	in.read((char*)&m_BufferInstance.vSize, sizeof(_float2));
	in.read((char*)&m_BufferInstance.vRectSize, sizeof(_float2));
	in.read((char*)&m_BufferInstance.vSpeed, sizeof(_float2));
	in.read((char*)&m_BufferInstance.vLifeTime, sizeof(_float2));
	in.read((char*)&m_BufferInstance.isLoop, sizeof(_bool));
	in.read((char*)&m_BufferInstance.isAttach, sizeof(_bool));

	if (2 == m_iShaderPass)
	{
		in.read((char*)&m_vStartColor, sizeof(_float4));
		in.read((char*)&m_vEndColor, sizeof(_float4));
	}

	in.close();

	return S_OK;
}

HRESULT CAura::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &m_BufferInstance)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Aura"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//톤텍스처 선택
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_ToneTextureTag,
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
		return E_FAIL;

	//베이스
	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_TextureTag,
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
		return E_FAIL;

	//플루드(흐름)
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Texture_AuraFluid"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTextureCom[2]))))
		return E_FAIL;
}

HRESULT CAura::Bind_ShaderResources()
{



	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_lifeAlpha", &m_fLifeAlpha, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartColor", &m_vStartColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &m_vEndColor, sizeof(_float4))))
		return E_FAIL;



	if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_ToneTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[1]->Bind_ShaderResource(m_pShaderCom, "g_BaseAlphaTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[2]->Bind_ShaderResource(m_pShaderCom, "g_FluidTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVCount", &m_fUVCount, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fToneUVCount", &m_fToneUVCount, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowPow", &m_fFlowPow, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowSpeed", &m_fFlowSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isAttach", &m_BufferInstance.isAttach, sizeof(_bool))))
		return E_FAIL;
	return S_OK;
}

CAura* CAura::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAura* pInstance = new CAura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAura");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAura* CAura::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath)
{
	CAura* pInstance = new CAura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
	{
		MSG_BOX("Failed To Created : CAura");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAura::Clone(void* pArg)
{
	CAura* pInstance = new CAura(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAura");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAura::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);

	for (auto& iter : m_pTextureCom)
		Safe_Release(iter);

	Safe_Release(m_pVIBufferCom);
}
