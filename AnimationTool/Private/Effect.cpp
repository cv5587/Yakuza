
#include "Effect.h"
#include "GameInstance.h"

const _uint CEffect::iAction[CEffect::ACTION_END] = {
    0x00000001, /* 0000 0001 */
    0x00000002, /* 0000 0010 */
    0x00000004, /*0000 0100 */
    0x00000008, /*0000 1000 */
    0x00000010, /*0001 0000 */
    //0x00000020, /*0010 0000 */
   // 0x00000040, /*0100 0000 */
};


CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CBlendObject{ pDevice,pContext }
{
}

CEffect::CEffect(const CEffect& rhs)
    :CBlendObject{ rhs },
    m_eType{ rhs.m_eType },
    m_ParticleTag{ rhs.m_ParticleTag },
    m_TextureTag{ rhs.m_TextureTag },
    m_iShaderPass{ rhs.m_iShaderPass },
    m_fStartTime{ rhs.m_fStartTime },
    m_vStartPos{ rhs.m_vStartPos },
    m_fLifeAlpha{ rhs.m_fLifeAlpha },
    m_iAction{ rhs.m_iAction },
    m_vStartColor{ rhs.m_vStartColor },
    m_vEndColor{ rhs.m_vEndColor },
    m_fDistortion{ rhs.m_fDistortion },
    m_isNormal{ rhs.m_isNormal },
    m_NormalTag{ rhs.m_NormalTag }
{
}

HRESULT CEffect::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

        if (nullptr == pDesc->pWorldMatrix)
        {
            EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
            m_eType = pDesc->eType;
            m_vStartPos = pDesc->vStartPos;
            m_ParticleTag = pDesc->ParticleTag;
            m_fStartTime = pDesc->fStartTime;
            m_vStartColor = pDesc->vStartColor;
            m_vEndColor = pDesc->vEndColor;
            m_iShaderPass = pDesc->iShaderPass;
            m_TextureTag = pDesc->TextureTag;
            m_fLifeAlpha = pDesc->fLifeAlpha;
            m_fRotate = pDesc->fRotate;
            m_fDistortion = pDesc->fDistortion;
            m_isNormal = pDesc->isNormal;
            m_NormalTag = pDesc->NormalTag;
        }
        else
        {
            m_pWorldMatrix = pDesc->pWorldMatrix;
        }

    }

    return S_OK;
}

void CEffect::Priority_Tick(const _float& fTimeDelta)
{
}

void CEffect::Tick(const _float& fTimeDelta)
{
    //_matrix matWorld = XMMatrixIdentity();
    //memcpy(&matWorld.r[CTransform::STATE_POSITION], m_pWorldMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));

    if (nullptr != m_pWorldMatrix)
        m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pWorldMatrix));
}

void CEffect::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CEffect::Render()
{
    return S_OK;
}

HRESULT CEffect::Edit_Action(ACTION iEditAction)
{
    _uint position = (_uint)iEditAction;

    _uint mask = 1 << position;

    m_iAction ^= mask;

    return S_OK;
}

void* CEffect::Get_Instance()
{
    return nullptr;
}

void CEffect::Reset_Buffer()
{
}

HRESULT CEffect::Save_Data(const string strDirectory)
{
    return S_OK;
}

HRESULT CEffect::Load_Data(const string strDirectory)
{
    return S_OK;
}

void CEffect::Free()
{
    __super::Free();
}
