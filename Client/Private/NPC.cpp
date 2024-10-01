#include "NPC.h"

#include "GameInstance.h"

#include "Mesh.h"

CNPC::CNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext}
{
}

CNPC::CNPC(const CNPC& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CNPC::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	NPC_IODESC* gameobjDesc = static_cast<NPC_IODESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_wstrModelName = gameobjDesc->wstrModelName;
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;
	m_iNPCDirection = gameobjDesc->iNPCDirection;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);
}

void CNPC::Priority_Tick(const _float& fTimeDelta)
{
}

void CNPC::Tick(const _float& fTimeDelta)
{
}

void CNPC::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CNPC::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(i);
		m_pModelCom->Bind_Compute(m_pComputeShaderCom, i);

		if (nullptr != m_pMaterialCom)
		{
			if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
				return E_FAIL;
		}

		//if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
		//	return E_FAIL;

		_float fFar = *m_pGameInstance->Get_CamFar();
		m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;
		_bool isRM = true;
		_bool isRT = true;
		_bool isMulti = true;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS)))
			isMulti = false;
		m_pShaderCom->Bind_RawValue("g_isMulti", &isMulti, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;
		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
			isRM = false;
		m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RTTexture", i, aiTextureType_EMISSIVE)))
			isRT = false;
		m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //블랜드
		else
			m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}


HRESULT CNPC::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CNPC::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove()), 0);

	// m_pModelCom->Get_AnimChanged()  선형보간이 끝났는지
	// m_pModelCom->Get_AnimLerp() 선형보간이 필요한 애니메이션인지
	if (m_pModelCom->Get_AnimChanged() || !m_pModelCom->Get_AnimLerp())
	{
		if (m_pModelCom->Get_AnimRestart())
		{
			XMStoreFloat4(&m_vPrevMove, XMVectorZero());
			m_fPrevSpeed = 0.f;
		}
		else
		{
			_float4 fMoveDir;
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove))) * m_fOffset;

			//Y값 이동을 죽인 방향으로 적용해야한다.
			_vector vTemp = XMVector3Normalize((vFF - XMLoadFloat4(&m_vPrevMove)));

			//Z가 Y처럼 쓰임
			vTemp = XMVectorSetZ(vTemp, XMVectorGetY(vTemp));
			XMStoreFloat4(&fMoveDir, XMVector3TransformNormal(XMVectorSetY(vTemp, 0.f), m_pTransformCom->Get_WorldMatrix()));

			if (0.01 > m_fPrevSpeed)
				m_fPrevSpeed = 0.f;

			m_pTransformCom->Go_Move_Custum(fMoveDir, m_fPrevSpeed, 1, m_pNavigationCom);
			m_fPrevSpeed = fMoveSpeed;

			XMStoreFloat4(&m_vPrevMove, vFF);
		}
	}
	else
	{
		// 선형보간중일때는 무조건 초기화
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
	}
}

void CNPC::Change_Animation()
{
}

string CNPC::Get_CurrentAnimationName()
{
	return string();
}

void CNPC::Free()
{
	__super::Free();
}