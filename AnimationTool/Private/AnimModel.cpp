#include "AnimModel.h"
#include "GameInstance.h"
#include "ImGuiManager.h"
#include "ModelBoneSphere.h"
#include "Bone.h"
#include "Mesh.h"
#include "Effect.h"


CAnimModel::CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice, pContext}
{
}

CAnimModel::CAnimModel(const CAnimModel& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CAnimModel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAnimModel::Initialize(void* pArg)
{
    GAMEOBJECT_DESC pDesc{};
    pDesc.fRotatePecSec = XMConvertToRadians(90.f);

    if (FAILED(CGameObject::Initialize(&pDesc)))
        return E_FAIL;

    m_strModelName = g_wstrModelName;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Ready_BoneSphere();

    return S_OK;
}

void CAnimModel::Priority_Tick(const _float& fTimeDelta)
{
    	// center_c_n의 컴바인드 행렬을 가져온다 (모델 본들의 스페이스 상태임)
	string strBoneName = "center_c_n";
	const _float4x4* pCenterBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(strBoneName.c_str());

	// RootNode의 컴바인드 행렬을 가져온다. (모델 본들의 스페이스 상태임)
	strBoneName = "RootNode";
	const _float4x4* pRootBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(strBoneName.c_str());
	
    if (nullptr != pCenterBoneMatrix && nullptr != pRootBoneMatrix)
    {
	    // 구해온 각각의 매트릭스들을 본들의 스페이스에서 해당 객체의 월드 스페이스로 변환한다.
	    _matrix CenterBoneMatrix = XMLoadFloat4x4(pCenterBoneMatrix) * m_pTransformCom->Get_WorldMatrix();
	    _matrix RootBoneMatrix = XMLoadFloat4x4(pRootBoneMatrix) * m_pTransformCom->Get_WorldMatrix();

	    // 월드 변환이 완료된 행렬들에서 포지션값을 꺼내 저장한다.
	    _vector vCenterBonePosistion, vRootBonePosistion;
	    memcpy(&vCenterBonePosistion, &CenterBoneMatrix.r[CTransform::STATE_POSITION], sizeof(_float4));
	    memcpy(&vRootBonePosistion, &RootBoneMatrix.r[CTransform::STATE_POSITION], sizeof(_float4));

	    // 애니메이션이 새로 시작하면 m_vPrevMove에는 이전 애님의 마지막 move의 큰 값이 남아있고, vMovePos는 새로운정보가 되어서 초기화가 필요하다.
	    _vector vMovePos = (XMVectorSet(XMVectorGetX(vCenterBonePosistion), 0, XMVectorGetZ(vCenterBonePosistion), 1.f) - XMVectorSet(XMVectorGetX(vRootBonePosistion), 0, XMVectorGetZ(vRootBonePosistion), 1.f));

        XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
        _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vMovePos;
        memcpy(&m_ModelWorldMatrix.m[CTransform::STATE_POSITION], &vPos, sizeof(_float4));
    }

}

void CAnimModel::Tick(const _float& fTimeDelta)
{
    if (m_pGameInstance->GetKeyState(DIK_TAB) == TAP)
    {
        if (m_iPassIndex == 0)
            m_iPassIndex = 4;
        else if (m_iPassIndex == 4)
            m_iPassIndex = 0;
    }

    CModel::ANIMATION_DESC desc{ m_iAnimIndex, true };

    if (m_isMonster)
    {
        m_pModelCom->Set_AnimationIndex(m_iAnimIndex, m_pAnimCom->Get_Animations(), 7.f);
        m_pModelCom->Play_Animation(fTimeDelta, m_pAnimCom, true);
    }
    else
    {
        m_pModelCom->Set_AnimationIndex(desc, 7.f);
        m_pModelCom->Play_Animation(fTimeDelta);
    }

    

    for (auto& pSphere : m_BoneSpheres)
        pSphere->Tick(fTimeDelta);

    for (auto& pair : m_TrailEffects)
    {
        if(pair.second.isOn)
            pair.second.pTrail->Tick(fTimeDelta);
    }
}

void CAnimModel::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

    for (auto& pSphere : m_BoneSpheres)
        pSphere->Late_Tick(fTimeDelta);

    for (auto& pair : m_TrailEffects)
    {
        if (pair.second.isOn)
            pair.second.pTrail->Late_Tick(fTimeDelta);
    }
}

HRESULT CAnimModel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    int i = 0;
    for (auto& pMesh : m_pModelCom->Get_Meshes())
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

        if (m_strRimMeshName == pMesh->Get_Name())
        {
            if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
                return E_FAIL;
            if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimPartsUV, sizeof(_float2))))
                return E_FAIL;
        }
        else
        {
            _float fNone = 0.f;
            if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &fNone, sizeof(_float))))
                return E_FAIL;
            if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimUV", &m_fRimPartsUV, sizeof(_float2))))
                return E_FAIL;
        }


        if (m_iSelectedMeshIndex == i)
        {
            m_pShaderCom->Begin(3);     //마젠타
        }
        else
        {
            if (pMesh->Get_AlphaApply())
                m_pShaderCom->Begin(1);     //블랜드
            else
                m_pShaderCom->Begin(m_iPassIndex);
        }

        m_pModelCom->Render(i);

        i++;
    }

    return S_OK;
}

void CAnimModel::Set_Position(_fvector vPosition)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CAnimModel::Set_Scaled(_float x, _float y, _float z)
{
    m_pTransformCom->Set_Scale(x, y, z);
}

void CAnimModel::Set_Rotation(_uint iAxis, _float vRadian, _float fTimeDelta)
{
    m_pTransformCom->Turn(m_pTransformCom->Get_State((CTransform::STATE)iAxis), vRadian * fTimeDelta);
}

void CAnimModel::Trail_On(string& strBoneName, _bool isOn)
{
    if (m_TrailEffects.find(strBoneName) == m_TrailEffects.end()) return;
    (*m_TrailEffects.find(strBoneName)).second.isOn = isOn;
}

const vector<class CAnimation*>& CAnimModel::Get_Animations()
{
    return m_pModelCom->Get_Animations();
}

const vector<class CBone*>& CAnimModel::Get_Bones()
{
    return m_pModelCom->Get_Bones();
}

const vector<class CMesh*>& CAnimModel::Get_Meshes()
{
    return m_pModelCom->Get_Meshes();
}

_bool CAnimModel::Get_AnimLoop(_uint iAnimIndex)
{
    return m_pModelCom->Get_AnimLoop(iAnimIndex);
}

void CAnimModel::Change_Model(wstring strModelName, wstring strAnimName)
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pAnimCom);

    m_iAnimIndex = 0;
    m_strModelName = strModelName;

    wstring strComponentTag = TEXT("Prototype_Component_Model_") + m_strModelName;
    m_pModelCom = reinterpret_cast<CModel*>(m_pGameInstance->Add_Component_Clone(LEVEL_EDIT, strComponentTag));

    if (TEXT("") != strAnimName)
    {
        wstring strAnimComponentTag = TEXT("Prototype_Component_Anim_") + strAnimName;
        m_pAnimCom = reinterpret_cast<CAnim*>(m_pGameInstance->Add_Component_Clone(LEVEL_EDIT, strAnimComponentTag));
    }

    Ready_BoneSphere();
}

void CAnimModel::Change_Animation(_uint iAnimIndex, _bool isMonster)
{
    m_isMonster = isMonster;
    m_iAnimIndex = iAnimIndex;
}

void CAnimModel::Select_Bone(_uint iBoneIndex)
{
    if (m_BoneSpheres.size() <= iBoneIndex) return;

    for (auto& pBoneSphere : m_BoneSpheres)
        pBoneSphere->Change_TexutreIndex();

    m_BoneSpheres[iBoneIndex]->Change_TexutreIndex(true);
}

void CAnimModel::Create_BoneCollider(_uint iType, _uint iIndex, const _float3& vCenter, void* pDesc)
{
    switch (iType)
    {
    case CCollider::COLLIDER_AABB:
    {
        CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

        ColliderDesc.eType = CCollider::COLLIDER_AABB;
        ColliderDesc.vExtents = *((_float3*)pDesc);
        ColliderDesc.vCenter = _float3(vCenter);

        m_BoneSpheres[iIndex]->Create_Collider(CCollider::COLLIDER_AABB, &ColliderDesc);
        break;
    }
    case CCollider::COLLIDER_OBB:
    {
        CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

        ColliderDesc.eType = CCollider::COLLIDER_OBB;
        ColliderDesc.vExtents = *((_float3*)pDesc);
        ColliderDesc.vCenter = _float3(vCenter);
        ColliderDesc.vRotation = _float3(0, 0.f, 0.f);

        m_BoneSpheres[iIndex]->Create_Collider(CCollider::COLLIDER_OBB, &ColliderDesc);
        break;
    }

    case CCollider::COLLIDER_SPHERE:
    {
        CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

        ColliderDesc.eType = CCollider::COLLIDER_SPHERE;
        ColliderDesc.fRadius = *((_float*)pDesc);
        ColliderDesc.vCenter = _float3(vCenter);

        m_BoneSpheres[iIndex]->Create_Collider(CCollider::COLLIDER_SPHERE, &ColliderDesc);
        break;
    }

    default:
        break;
    }
}

void CAnimModel::Create_Trail(string& strBoneName, CGameObject* pEffect)
{
    Trail_Desc Desc{};
    Desc.pTrail = reinterpret_cast<CEffect*>(pEffect);

    m_TrailEffects.emplace(strBoneName, Desc);
}

void CAnimModel::Set_Collider_Center(_uint iIndex, const _float3& vCenter)
{
    m_BoneSpheres[iIndex]->Set_Collider_Center(vCenter);
}

void CAnimModel::Set_Collider_Value(_uint iIndex, void* pDesc)
{
    m_BoneSpheres[iIndex]->Set_Collider_Value(pDesc);
}

_bool CAnimModel::Created_BoneCollider(_uint iIndex)
{
    return m_BoneSpheres[iIndex]->Created_Collider();
}

HRESULT CAnimModel::Release_BoneCollider(_uint iIndex)
{
    return m_BoneSpheres[iIndex]->Release_Collider();
}

HRESULT CAnimModel::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    wstring strComponentTag = TEXT("Prototype_Component_Model_") + m_strModelName;

    if (FAILED(__super::Add_Component(LEVEL_EDIT, strComponentTag, TEXT("Com_Model"),
        reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Anim"),
        TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CAnimModel::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelWorldMatrix)))
    //    return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

void CAnimModel::Ready_BoneSphere()
{
    for (auto& pBoneSphere : m_BoneSpheres)
        Safe_Release(pBoneSphere);
    m_BoneSpheres.clear();

    auto BoneList = m_pModelCom->Get_Bones();
    m_BoneSpheres.resize(BoneList.size());

    CModelBoneSphere::BoneSphereDesc BoneSphereDesc{};

    BoneSphereDesc.pModelWorldMatrix = &m_ModelWorldMatrix;

    _uint i = 0;

    for (auto& pBone : BoneList)
    {
        BoneSphereDesc.pBoneWorldMatrix = pBone->Get_CombinedTransformationMatrix();

        CGameObject* pBoneSphere = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BoneSphere"), &BoneSphereDesc);

        m_BoneSpheres[i] = reinterpret_cast<CModelBoneSphere*>(pBoneSphere);

        i++;
    }
}

CAnimModel* CAnimModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAnimModel* pInstance = new CAnimModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CAnimModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAnimModel::Clone(void* pArg)
{
    CAnimModel* pInstance = new CAnimModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CAnimModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimModel::Free()
{
    __super::Free();

    for (auto& pBoneSphere : m_BoneSpheres)
        Safe_Release(pBoneSphere);
    m_BoneSpheres.clear();

    Safe_Release(m_pAnimCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
