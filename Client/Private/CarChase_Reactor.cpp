#include "CarChase_Reactor.h"

#include "GameInstance.h"
#include "CharacterData.h"

#include "Mesh.h"

#include "Player.h"
#include "CarChase_Monster.h"
#include "Highway_Taxi.h"
#include"EffectManager.h"

CCarChase_Reactor::CCarChase_Reactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CCarChase_Reactor::CCarChase_Reactor(const CCarChase_Reactor& rhs)
	: CLandObject{ rhs }
{
}

string CCarChase_Reactor::Get_CurrentAnimationName()
{
	return m_strAnimName;
}

HRESULT CCarChase_Reactor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Reactor::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	HIGHWAY_IODESC* gameobjDesc = (HIGHWAY_IODESC*)pArg;
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_wstrModelName = gameobjDesc->wstrModelName;
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;
	m_iStageDir = gameobjDesc->iStageDir;
	m_iLineDir = gameobjDesc->iLineDir;
	m_iWaypointIndex = gameobjDesc->iWaypointIndex;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Ready_Monster(gameobjDesc->iMonsterWeaponType)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pNavigationCom->Get_CurrentWaypointPos());

	return S_OK;
}

void CCarChase_Reactor::Priority_Tick(const _float& fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
		pMonster->Priority_Tick(fTimeDelta);
}

void CCarChase_Reactor::Tick(const _float& fTimeDelta)
{
	Change_Animation(); //애니메이션 변경

	if (m_iAnim != -1)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::ANIMATION_DESC{_uint(m_iAnim), m_isAnimLoop});

#ifdef _DEBUG
	if (m_pGameInstance->GetKeyState(DIK_LCONTROL) == TAP)
		m_isStop = !m_isStop;
#endif // _DEBUG

	if (!m_isStop)
		Move_Waypoint(fTimeDelta);

	//m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	for (auto& pMonster : m_Monsters)
	{
		if (!pMonster->isDead())
			pMonster->Tick(fTimeDelta);
	}
		
}

void CCarChase_Reactor::Late_Tick(const _float& fTimeDelta)
{
	//컬링
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (nullptr != m_pNavigationCom)
	{
		float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);
		vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
	}

	for (auto& pMonster : m_Monsters)
	{
		if (!pMonster->isDead())
			pMonster->Late_Tick(fTimeDelta);
	}
		
}

HRESULT CCarChase_Reactor::Render()
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

		m_pShaderCom->Begin(0);		//디폴트
		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

void CCarChase_Reactor::Change_Animation()
{
}

_bool CCarChase_Reactor::Check_Dead()
{
	if (m_isObjectDead)
	{
		if (m_isFinishEffect)
			return true;

		if (m_pModelCom->Get_AnimFinished())
		{
			m_isFinishEffect = true;
			//여기가 죽었을때 굴러가고 터지는 부분(몬스터가 죽으면? 불이 터짐)
			CEffect::EFFECT_DESC EffectDesc;

			EffectDesc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			CEffectManager::GetInstance()->Heli_Exp(EffectDesc);
		}
		return true;
	}

	for (auto& pMonster : m_Monsters)
	{
		//운전수가 죽으면 바로 죽게 된다.
		if (pMonster->isObjectDead() && pMonster->Get_WeaponType() == CCarChase_Monster::DRV)
		{
			for (auto& pMonster : m_Monsters)
				pMonster->Set_ReactorDead(true);
			//자동차가 죽으면 멈추고 불난다.+연기

			CEffect::EFFECT_DESC EffectDesc;

			EffectDesc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			CEffectManager::GetInstance()->Car_Fire(EffectDesc);
			
			return true;
		}

		if (!pMonster->isObjectDead())
			return false;
	}

	return true;
}

void CCarChase_Reactor::Move_Waypoint(const _float& fTimeDelta)
{
	if (m_isObjectDead)
	{
		m_fSpeed = m_fSpeed <= 0.f ? 0.f : m_fSpeed - fTimeDelta * 30.f;
		m_pTransformCom->Go_Straight_CustumSpeed(m_fSpeed, fTimeDelta, m_pNavigationCom);
		return;
	}

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//웨이포인트 
	_vector vDir = m_pNavigationCom->Compute_WayPointDir(vPosition, fTimeDelta, m_isStart);
	m_pTransformCom->LookAt_For_LandObject(vDir, true);
	
	//플레이어와 인덱스 값의 차이를 둬야 함
	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Taxi"), 0));
	_int iCurrentWaypointIndex = m_pNavigationCom->Get_WaypointIndex();
	_int iPlayerCurrentWaypointIndex = pPlayer->Get_CurrentWaypointIndex();
	_int iMaxIndex = m_pNavigationCom->Get_RouteSize();

	//DIR_F 일 경우 플레이어 인덱스 + 1
	//DIR_M 일 경우 플레이어 인덱스
	//DIR_B 일 경우 플레이어 인덱스 - 1
	_int iGoalIndex = 0;
	if (m_iStageDir == DIR_R || m_iStageDir == DIR_L)
	{
		if (m_iLineDir == DIR_F)
			iGoalIndex = iMaxIndex == iPlayerCurrentWaypointIndex + 1 ? 0 : iPlayerCurrentWaypointIndex + 1;
		else if (m_iLineDir == DIR_M)
			iGoalIndex = iPlayerCurrentWaypointIndex;
		else if (m_iLineDir == DIR_B)
			iGoalIndex = 0 > iPlayerCurrentWaypointIndex - 1 ? iMaxIndex - 1 : iPlayerCurrentWaypointIndex - 1;
	}
	else if (m_iStageDir == DIR_B)
	{
		iGoalIndex = 0 > iPlayerCurrentWaypointIndex - 2 ? iMaxIndex - 2 : iPlayerCurrentWaypointIndex - 2;
	}
	else if (m_iStageDir == DIR_F)
	{
		iGoalIndex = iMaxIndex == iPlayerCurrentWaypointIndex + 1 ? 0 : iPlayerCurrentWaypointIndex + 1;
	}

	_float fDistance = XMVectorGetX(XMVector3Length(m_pNavigationCom->Get_WaypointPos(iGoalIndex) - vPosition));

	//스피드 값 지정
	//스테이지 방향이 DIR_F이고 Start일 경우 앞에서 뒤로 이동하도록 함
	_float fBack = m_iStageDir == DIR_F && m_isStart ? -1.f : 1.f;

	//만약 몬스터가 플레이어보다 앞서 나가거나 너무 뒤로 이동한다면 반대방향으로 이동하도록 한다..
	if (fBack == 1.f && iPlayerCurrentWaypointIndex + 2 <= iCurrentWaypointIndex
		|| fBack == -1.f && iPlayerCurrentWaypointIndex - 3 >= iCurrentWaypointIndex)
		fBack *= -1.f;

	_float fFactor = min(fDistance, 20.f) / 20.f;

	m_fSpeed = m_fMaxSpeed * fFactor * fBack;
	if (m_fSpeed < m_fMinSpeed * fBack)
	{
		m_isStart = false;
		m_fSpeed = m_fMinSpeed * fBack;
	}
	else if (m_fSpeed > m_fMaxSpeed * fBack)
		m_fSpeed = m_fMaxSpeed * fBack;

	m_pTransformCom->Go_Straight_CustumSpeed(m_fSpeed, fTimeDelta, m_pNavigationCom);

}

HRESULT CCarChase_Reactor::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
		return E_FAIL;

	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Taxi"), 0));
	_vector vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	
	CNavigation::NAVIGATION_DESC Desc{};
	Desc.iCurrentLine = m_iNaviRouteNum;
	Desc.iWayPointIndex = m_iWaypointIndex;
	//Desc.vPosition = ;
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarChase_Reactor::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Reactor::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);

	for (auto& pMonster : m_Monsters)
		Safe_Release(pMonster);
	m_Monsters.clear();
}
