#include "Adventure.h"

#include "GameInstance.h"
#include "Mesh.h"

#include "SocketCollider.h"
#include "AI_Monster.h"


CAdventure::CAdventure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLandObject { pDevice, pContext }
{
}

CAdventure::CAdventure(const CAdventure& rhs)
	: CLandObject { rhs }
{
}

void CAdventure::Start_Root(_int iGoalIndex)
{
	m_pAStartCom->Start_Root(m_pNavigationCom, iGoalIndex);
}

HRESULT CAdventure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAdventure::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ADVENTURE_IODESC* gameobjDesc = static_cast<ADVENTURE_IODESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_wstrModelName = gameobjDesc->wstrModelName;
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;
	m_iNPCDirection = gameobjDesc->iNPCDirection;
	m_iGender = gameobjDesc->iObjectPropertyType;

	if (FAILED(CAdventure::Add_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);
	m_pModelCom->Set_AnimationIndex(0);

	return S_OK;
}

void CAdventure::Priority_Tick(const _float& fTimeDelta)
{
}

void CAdventure::Tick(const _float& fTimeDelta)
{
}

void CAdventure::Late_Tick(const _float& fTimeDelta)
{
	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);

	vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
}

HRESULT CAdventure::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		if (!m_pGameInstance->isShadow())
		{
			m_pModelCom->Bind_BoneMatrices(i);
			m_pModelCom->Bind_Compute(m_pComputeShaderCom, i);
		}

		if (nullptr != m_pMaterialCom)
		{
			if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
				return E_FAIL;
		}

		_float fFar = *m_pGameInstance->Get_CamFar();
		m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		//if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
		//	return E_FAIL;

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
		m_pShaderCom->Bind_RawValue("IsY3Shader", &isRS, sizeof(_bool));

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

void CAdventure::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
	m_isColl = true;
}

void CAdventure::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pAnimCom->Get_AnimPosition());
		_double Duration = *(m_pAnimCom->Get_AnimDuration());

		if (CurPos >= pEvent.fPlayPosition && CurPos < Duration)
		{
			CSocketCollider* pCollider = m_pColliders.at(pEvent.iBoneIndex);

			switch (pEvent.iType)
			{
			case 0:
				pCollider->On();
				break;
			case 1:
				pCollider->Off();
				break;
			case 2:
#ifdef _DEBUG
				cout << "사운드 재생" << endl;
#endif // DEBUG
				break;
			case 3:
#ifdef _DEBUG
				cout << "이펙트 재생" << endl;
#endif // _DEBUG
				break;
			}
		}

	}
}

string CAdventure::Get_CurrentAnimationName()
{
	return m_pAnimCom->Get_AnimationName(m_pAnimCom->Get_CurrentAnimIndex());
}

_bool CAdventure::Checked_Animation_Ratio(_float fRatio)
{
	if (fRatio < *m_pModelCom->Get_AnimationCurrentPosition(m_pAnimCom) / *m_pModelCom->Get_AnimationDuration(m_pAnimCom))
		return true;

	return false;
}

void CAdventure::Change_Animation()
{
	m_isAnimLoop = false;
	m_fOffset = 1.f;
}

void CAdventure::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove(m_pAnimCom)), 0);

	// m_pModelCom->Get_AnimChanged()  선형보간이 끝났는지
	// m_pModelCom->Get_AnimLerp() 선형보간이 필요한 애니메이션인지
	if (m_pAnimCom->Get_AnimChanged() || !m_pAnimCom->Get_AnimLerp(m_fChangeInterval))
	{
		if (m_pAnimCom->Get_AnimRestart(m_isAnimLoop))
		{
			XMStoreFloat4(&m_vPrevMove, XMVectorZero());
			m_fPrevSpeed = 0.f;
		}
		else
		{
			_float4 fMoveDir;
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));

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

	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
}

void CAdventure::Check_Separation()
{
	//내 범위 안에 들어온 NPC를 보고 피한다. 
	//Layer_NPC

	_float fRange = 1.f;
	vector<CGameObject*> NPCs = m_pGameInstance->Get_GameObjects(m_iCurrentLevel, TEXT("Layer_NPC"));
	
	_vector vMoveDir = {};
	for (auto& pObj : NPCs)
	{
		_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pObj->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		_float fDistance = XMVectorGetX(XMVector3Length(vDir));

		if (fDistance < fRange)
		{
			vMoveDir += vDir;
		}
	}

	vMoveDir = XMVector3Normalize(vMoveDir);

	//NPC별 충돌 시 피하는 방향 벡터 -> vMoveDir;
}


HRESULT CAdventure::Setup_Animation()
{
	if (m_iGender == GENDER_M)
	{
		m_iAnim = m_pAnimCom->Get_AnimationIndex(m_strAnimName.c_str());

		if (m_iAnim == -1)
			return E_FAIL;

		// 실제로 애니메이션 체인지가 일어났을 때 켜져있던 어택 콜라이더를 전부 끈다
		if (m_pModelCom->Set_AnimationIndex(m_iAnim, m_pAnimCom->Get_Animations(), m_fChangeInterval))
		{
			m_pModelCom->Set_PreAnimations(m_pAnimCom->Get_Animations());

			Off_Attack_Colliders();
		}

		if (nullptr != m_pData)
			m_pData->Set_CurrentAnimation(m_strAnimName);
	}
	else
	{

		if (m_iAnim == -1)
			return E_FAIL;

		// 실제로 애니메이션 체인지가 일어났을 때 켜져있던 어택 콜라이더를 전부 끈다
		if (m_pModelCom->Set_AnimationIndex(m_iAnim, m_pAnimCom->Get_Animations(), m_fChangeInterval))
		{
			m_pModelCom->Set_PreAnimations(m_pAnimCom->Get_Animations());

			Off_Attack_Colliders();
		}

		if (nullptr != m_pData)
			m_pData->Set_CurrentAnimation(m_strAnimName);
	}

	return S_OK;
}

HRESULT CAdventure::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim_NPC"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC Desc{};
	Desc.iCurrentLine = m_iNaviRouteNum;
	Desc.iCurrentRouteDir = DIR_F;
	Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_AStart"),
		TEXT("Com_AStart"), reinterpret_cast<CComponent**>(&m_pAStartCom))))
		return E_FAIL;


	//모델 이름 추출
	string strModelName = m_pGameInstance->WstringToString(m_wstrModelName);
	string strRemoveName = "Prototype_Component_Model_";
	_int iPos = strModelName.find(strRemoveName);

	if (iPos == string::npos)
		return E_FAIL;

	strModelName = strModelName.erase(iPos, strRemoveName.size());

	wstring strMaterialName = TEXT("Prototype_Component_Material_") + m_pGameInstance->StringToWstring(strModelName);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, strMaterialName,
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		m_pMaterialCom = nullptr;

	return S_OK;
}

HRESULT CAdventure::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CAdventure::Free()
{
	__super::Free();

	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pAStartCom);
}
