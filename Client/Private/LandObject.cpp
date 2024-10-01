#include "LandObject.h"  

#include "SystemManager.h"
#include "GameInstance.h"
#include "Collision_Manager.h"

#include "SocketCollider.h"
#include "SocketEffect.h"

#include "Mesh.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext},
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pCollisionManager{ CCollision_Manager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
}

CLandObject::CLandObject(const CLandObject& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager },
	m_pCollisionManager{ rhs.m_pCollisionManager }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLandObject::Priority_Tick(const _float& fTimeDelta)
{
}

void CLandObject::Tick(const _float& fTimeDelta)
{
}

void CLandObject::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::Render_LightDepth()
{
	const _float4x4* ViewMatrixArray = m_pGameInstance->Get_Shadow_Transform_View_Float4x4();
	const _float4x4* ProjMatrixArray = m_pGameInstance->Get_Shadow_Transform_Proj_Float4x4();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_ViewMatrixArray", ViewMatrixArray, 3)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_ProjMatrixArray", ProjMatrixArray, 3)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(i);
		m_pModelCom->Bind_Compute(m_pComputeShaderCom, i);
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CLandObject::Render_BoneCompute()
{
	if (nullptr == m_pComputeShaderCom)
		return S_OK;

	int i = 0;

	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(i);
		m_pModelCom->Bind_Compute(m_pComputeShaderCom, i);

		i++;
	}

	return S_OK;
}

void CLandObject::ImpulseResolution(CGameObject* pTargetObject, _float fDistance)
{
	if (nullptr == m_pColliderCom) return;

	_float3 vDir = m_pColliderCom->ImpulseResolution(pTargetObject->Get_Collider(), fDistance);

	if (!XMVector3Equal(XMLoadFloat3(&vDir), XMVectorZero()))
	{
		_vector vMovePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMLoadFloat3(&vDir));

		//CNavigation* pTargetNavi = dynamic_cast<CNavigation*>(pTargetObject->Get_Component(TEXT("Com_Navigation")));
		if(nullptr == m_pNavigationCom)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMovePos);
		else
		{
			// 네비 밖인지 아닌지 검사해야함.
			if (m_pNavigationCom->isMove(vMovePos))
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMovePos);
		}
	}
}

HRESULT CLandObject::Add_CharacterData()
{
	m_pData = CCharacterData::Create(this);

	if (nullptr == m_pData)
		return E_FAIL;

	Apply_ChracterData();

	return S_OK;
}

void CLandObject::Apply_ChracterData()
{
	auto& pAlphaMeshes = m_pData->Get_AlphaMeshes();
	auto pMeshes = m_pModelCom->Get_Meshes();

	for (size_t i = 0; i < pMeshes.size(); i++)
	{
		for (auto& iMeshIndex : pAlphaMeshes)
		{
			if (i == iMeshIndex)
				pMeshes[i]->Set_AlphaApply(true);
		}
	}

	auto& pLoopAnimations = m_pData->Get_LoopAnimations();
	for (auto& iAnimIndex : pLoopAnimations)
	{
		m_pModelCom->Set_AnimLoop(iAnimIndex, true);
	}

	auto& pColliders = m_pData->Get_Colliders();

	for (auto& Collider : pColliders)
	{
		CSocketCollider::SOCKET_COLLIDER_DESC Desc{};
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix_AtIndex(Collider.first);
		Desc.iBoneIndex = Collider.first;
		Desc.ColliderState = Collider.second;
		//Desc.iType = Collider.second.isAlways ? 1 : 0;
		Desc.pParentObject = this;

		CGameObject* pSoketCollider = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketCollider"), &Desc);
		if (nullptr == pSoketCollider)
			return;

		auto [it, success] = m_pColliders.emplace(Collider.first, static_cast<CSocketCollider*>(pSoketCollider));

		// 생성한 모든 콜라이더는 일단 꺼둔다.
		// 몸체에 붙일 (플레이어가 피격당할) 콜라이더는 항시 켜져있어야하므로 On으로 켜둔다
		if (Collider.second.isAlways)
			it->second->On();
		else
			it->second->Off();
	}

	auto& pEffects = m_pData->Get_Effets();

	for (auto& pEffect : pEffects)
	{
		CSocketEffect::SOKET_EFFECT_DESC Desc{};
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(pEffect.first.c_str());
		Desc.wstrEffectName = pEffect.second;

		CGameObject* pSoketEffect = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketEffect"), &Desc);
		if (nullptr == pSoketEffect)
			return;

		auto it = m_pEffects.emplace(pEffect.first, static_cast<CSocketEffect*>(pSoketEffect));

		//it->second->On();
		it->second->Off();
	}

	// 플레이어일 경우 기존 방식대로 트레일 이벤트 진행 몬스터의 경우, 트레일 안쓸거라 생성x
	if (!m_isMonster)
	{
		auto& pTrailEvents = m_pData->Get_TrailEvents();

		for (auto& pTrailEvent : pTrailEvents)
		{
			CSocketEffect::SOKET_EFFECT_DESC Desc{};
			Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(pTrailEvent.second.strBonelName.c_str());
			Desc.wstrEffectName = m_pGameInstance->StringToWstring(pTrailEvent.second.strTrailProtoName);

			CGameObject* pSoketEffect = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketEffect"), &Desc);
			if (nullptr == pSoketEffect)
				return;

			auto it = m_pTrailEffects.emplace(pTrailEvent.second.strBonelName, static_cast<CSocketEffect*>(pSoketEffect));

			if (it.second)
				it.first->second->Off();
			else
				Safe_Release(pSoketEffect);
		}

	}

	auto& pBloodEffectEvents = m_pData->Get_BloodEffectEvents();

	for (auto& pBloodEffectEvent : pBloodEffectEvents)
	{
		CSocketEffect::SOKET_EFFECT_DESC Desc{};
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(pBloodEffectEvent.second.strBonelName.c_str());

		if (pBloodEffectEvent.second.isLoop)
		{
			switch (pBloodEffectEvent.second.iBloodEffectType)
			{
			case 1:
			{
				Desc.wstrEffectName = TEXT("Prototype_GameObject_Particle_Point_Blood6");
			}
			break;
			}

			CGameObject* pSoketEffect = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketEffect"), &Desc);
			if (nullptr == pSoketEffect)
				return;

			auto it = m_pBloodEffects.emplace(pBloodEffectEvent.second.strBonelName, static_cast<CSocketEffect*>(pSoketEffect));

			if (it.second)
				it.first->second->Off();
			else
				Safe_Release(pSoketEffect);
		}

	}

}

void CLandObject::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

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
#endif // _DEBUG
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

void CLandObject::RimLight_Event()
{
	auto& pCurEvents = m_pData->Get_Current_Rim_Events();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

		if (CurPos >= pEvent.fAinmPosition && CurPos < Duration)
		{
			if (pEvent.iType == 0)
				m_strRimMeshName = pEvent.strMeshName;
			else
				m_strRimMeshName = "";
		}
	}
}

void CLandObject::Trail_Event()
{
	auto& pCurEvents = m_pData->Get_Current_Trail_Events();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

		if (CurPos >= pEvent.fAinmPosition && CurPos < Duration)
		{
			if (pEvent.iType == 0)		// 트레일 켜주기
			{
				auto iter = m_pTrailEffects.find(pEvent.strBonelName);
				iter->second->On();
			}
			else
			{
				auto iter = m_pTrailEffects.find(pEvent.strBonelName);
				iter->second->Off();
			}

		}
	}
}

void CLandObject::Sound_Event()
{
	auto& pCurEvents = m_pData->Get_Current_SoundEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

		if (!pEvent->isPlayed && CurPos >= pEvent->fAinmPosition && CurPos < Duration)
		{
			pEvent->isPlayed = true;
			wstring wstrSoundFile = m_pGameInstance->StringToWstring(pEvent->strSoundFileName);
			m_pGameInstance->StopAndPlaySound(wstrSoundFile, static_cast<CHANNELID>(pEvent->iChannel), pEvent->fSoundVolume);
		}
	}
}

_bool CLandObject::Checked_Animation_Ratio(_float fRatio)
{
	if (fRatio < *m_pModelCom->Get_AnimationCurrentPosition() / *m_pModelCom->Get_AnimationDuration())
		return true;

	return false;
}

// iHandType: 0 양손, 1 왼손, 2 오른손
void CLandObject::On_Separation_Hand(_uint iHandType)
{
	switch (iHandType)
	{
	case 0:		//양손
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", (_int)HAND_ANIM);
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", (_int)HAND_ANIM);
		m_pModelCom->Set_Separation_SingleBone("ude3_r_n", -1);
		m_pModelCom->Set_Separation_SingleBone("ude3_l_n", -1);
		break;
	case 1:		//왼손
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", (_int)HAND_ANIM);
		m_pModelCom->Set_Separation_SingleBone("ude3_l_n", -1);
		break;
	case 2:		//오른손
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", (_int)HAND_ANIM);
		m_pModelCom->Set_Separation_SingleBone("ude3_r_n", -1);
		break;
	}
}

void CLandObject::Off_Separation_Hand(_uint iHandType)
{
	switch (iHandType)
	{
	case 0:		//양손
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", -1);
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", -1);
		break;
	case 1:		//왼손
		m_pModelCom->Set_Separation_ParentBone("ude3_l_n", -1);
		break;
	case 2:		//오른손
		m_pModelCom->Set_Separation_ParentBone("ude3_r_n", -1);
		break;
	}
}

void CLandObject::On_Separation_Face()
{
	m_pModelCom->Set_Separation_ParentBone("face_c_n", (_int)FACE_ANIM);
	m_pModelCom->Set_Separation_SingleBone("face_c_n", -1);
}

void CLandObject::Off_Separation_Face()
{
	m_pModelCom->Set_Separation_ParentBone("face_c_n", -1);
}

void CLandObject::Separation_Bone(string strBoneName, _int iAnimType, _bool isExceptParent)
{
	m_pModelCom->Set_Separation_ParentBone(strBoneName, iAnimType);
	if(isExceptParent)
		m_pModelCom->Set_Separation_SingleBone(strBoneName, -1);
}

void CLandObject::Separation_SingleBone(string strBoneName, _int iAnimType)
{
	m_pModelCom->Set_Separation_SingleBone(strBoneName, iAnimType);
}

void CLandObject::Off_Attack_Colliders()
{
	for (auto& pSocketCollider : m_pColliders)
	{
		if(pSocketCollider.second->Get_CollierType() == CSocketCollider::ATTACK)
			pSocketCollider.second->Off();
	}
}

void CLandObject::Off_Trails()
{
	for (auto& pTrail : m_pTrailEffects)
	{
		pTrail.second->Off();
	}
}

void CLandObject::Set_NavigationIndex(int iIndex)
{
	m_pNavigationCom->Set_Index(iIndex); 
}

HRESULT CLandObject::Add_Components()
{
	return S_OK;
}

HRESULT CLandObject::Bind_ResourceData()
{
	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();
	
	if (m_isCloned)
	{
		//if (m_pColliders.size() > 0)
		//{
		//	for (auto& pCollider : m_pColliders)
		//		Safe_Release(pCollider.second);
		//	m_pColliders.clear();
		//}

		// 멀티맵이라 다르게 지워야한다.
		for (auto iter = m_pEffects.begin(); iter != m_pEffects.end();)
		{
			Safe_Release(iter->second);
			iter = m_pEffects.erase(iter);
		}

		for (auto& pEffect : m_pTrailEffects)
			Safe_Release(pEffect.second);
		m_pTrailEffects.clear();

		for (auto& pEffect : m_pBloodEffects)
			Safe_Release(pEffect.second);
		m_pBloodEffects.clear();
	}

	for (auto pAnimCom : m_SeparationAnimComs)
	{
		Safe_Release(pAnimCom);
	}
	m_SeparationAnimComs.clear();

	Safe_Release(m_pSystemManager);
	Safe_Release(m_pCollisionManager);
	Safe_Release(m_pData);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pComputeShaderCom);
}

void CLandObject::Compute_Height()
{
}

string CLandObject::ExtractString(string strName)
{
	size_t start = strName.find('[');
	size_t end = strName.find(']', start);

	if (start != std::string::npos && end != std::string::npos && end > start) {
		// 대괄호가 존재하면, 그 사이의 문자열 추출
		return strName.substr(start + 1, end - start - 1);
	}

	return "";
}
