#include "Particle_Point.h"
#include "GameInstance.h"


CParticle_Point::CParticle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CEffect{ pDevice , pContext }
{
}

CParticle_Point::CParticle_Point(const CParticle_Point& rhs)
    :CEffect{ rhs },
    m_BufferInstance{ rhs.m_BufferInstance },
    m_fUVCount{ rhs.m_fUVCount }
{
}

HRESULT CParticle_Point::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CParticle_Point::Initialize_Prototype(string strFilePath)
{
    if (FAILED(Load_Data(strFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Point::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

        if (nullptr == pDesc->pWorldMatrix)
        {
            PARTICLE_POINT_DESC* pDesc = static_cast<PARTICLE_POINT_DESC*>(pArg);
            m_BufferInstance = pDesc->BufferInstance;
            m_fUVCount = pDesc->fUVCount;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vStartPos));
        }
        else
        {
            m_pWorldMatrix = pDesc->pWorldMatrix;
            m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pWorldMatrix));
        }
    }
#ifdef _CLIENT
    m_BufferInstance.WorldMatrix = m_pWorldMatrix;
#else
    m_BufferInstance.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
#endif  

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Point::Priority_Tick(const _float& fTimeDelta)
{
}

void CParticle_Point::Tick(const _float& fTimeDelta)
{
    m_fCurTime += fTimeDelta;
    if (!m_BufferInstance.isLoop)
    {
        if (0 == m_BufferInstance.fDelay)
        {
            _float fTotalTime = m_fStartTime + m_BufferInstance.vLifeTime.y;
            if (m_fCurTime > fTotalTime)
                m_isDead = true;
        }
    }

#ifdef _CLIENT

        m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pWorldMatrix));

#endif // _CLIENT

    if (m_fCurTime >= m_fStartTime)
    {
        if (m_iAction & iAction[ACTION_SPREAD])
        {
           m_pVIBufferCom->Spread(fTimeDelta); 
        }
        if (m_iAction & iAction[ACTION_SIZEUP])
        {
            m_pVIBufferCom->SizeUp_Time(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_SIZEDOWN])
        {
            m_pVIBufferCom->SizeDown_Time(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_NOBILLBOARD])    
        {
            m_pVIBufferCom->RotSpread(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_FALLSPREAD])
        {
           m_pVIBufferCom->FallSpread(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_BLOOD])
        {
            m_pVIBufferCom->BloodSpread(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_INTERVAL])
        {
           m_pVIBufferCom->IntervalSpread(fTimeDelta);
        }
    }



}

void CParticle_Point::Late_Tick(const _float& fTimeDelta)
{

    switch (m_eType)
    {
    case Client::CEffect::TYPE_POINT:
    {
        if (m_BufferInstance.isLoop)
        {
            if (7 == m_iShaderPass|| 9 == m_iShaderPass)
                m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
            else
                m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
        }
        else
        {
            if (m_fCurTime >= m_fStartTime && !m_isDead)
            {
                if (7 == m_iShaderPass|| 9 == m_iShaderPass)
                    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
                else
                   m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
            }
        }
    }
    break;
    case Client::CEffect::TYPE_TRAIL:
        break;
    case Client::CEffect::TYPE_GLOW:
    {
        if (m_BufferInstance.isLoop)
        {
            m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
        }
        else
        {
            if (m_fCurTime >= m_fStartTime && !m_isDead)
            {
                m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
            }
        }
    }
    break;
    case Client::CEffect::TYPE_AURA:
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
    break;
    case Client::CEffect::TYPE_END:
        break;
    default:
        break;
    }
    // Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));


}

HRESULT CParticle_Point::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(m_iShaderPass);

    m_pVIBufferCom->Render();

    return S_OK;
}

void* CParticle_Point::Get_Instance()
{
    return &m_BufferInstance;
}

void CParticle_Point::Reset_Buffer()
{

    m_pVIBufferCom->Reset();
}


HRESULT CParticle_Point::Save_Data(const string strDirectory)
{
    string Directory = strDirectory;
    string ParticleTag = m_pGameInstance->WstringToString(m_ParticleTag);
    string TextureTag = m_pGameInstance->WstringToString(m_TextureTag);
    string NormalTag = m_pGameInstance->WstringToString(m_NormalTag);

    string headTag = "Prototype_GameObject_Particle_Point_";
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

    out.write((char*)&m_fStartTime, sizeof(_float));

    out.write((char*)&m_vStartPos, sizeof(_float4));

    out.write((char*)&m_fRotate, sizeof(_float));

    out.write((char*)&m_fLifeAlpha, sizeof(_float2));

    out.write((char*)&m_iAction, sizeof(_uint));

    out.write((char*)&m_vStartColor, sizeof(_float4));

    out.write((char*)&m_vEndColor, sizeof(_float4));

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
    out.write((char*)&m_BufferInstance.fDelay, sizeof(_float));

    if(7==m_iShaderPass)
    {
        out.write((char*)&m_BufferInstance.LowStartRot, sizeof(_float3));
        out.write((char*)&m_BufferInstance.HighStartRot, sizeof(_float3));
        out.write((char*)&m_BufferInstance.LowAngleVelocity, sizeof(_float3));
        out.write((char*)&m_BufferInstance.HighAngleVelocity, sizeof(_float3));
        out.write((char*)&m_BufferInstance.GravityScale, sizeof(_float));
        out.write((char*)&m_BufferInstance.CrossArea, sizeof(_float));
    }


    if(6==m_iShaderPass|| 8 == m_iShaderPass)
     out.write((char*)&m_fDistortion, sizeof(_float));


    if (9 == m_iShaderPass)
    {
        out.write((char*)&m_isNormal, sizeof(_bool));

        _int strNormallength = NormalTag.length();
        out.write((char*)&strNormallength, sizeof(_int));
        out.write(NormalTag.c_str(), strNormallength);
        out.write((char*)&m_fUVCount, sizeof(_float2));


        out.write((char*)&m_BufferInstance.LowStartRot, sizeof(_float3));
        out.write((char*)&m_BufferInstance.HighStartRot, sizeof(_float3));
        out.write((char*)&m_BufferInstance.LowAngleVelocity, sizeof(_float3));
        out.write((char*)&m_BufferInstance.HighAngleVelocity, sizeof(_float3));
        out.write((char*)&m_BufferInstance.GravityScale, sizeof(_float));
        out.write((char*)&m_BufferInstance.CrossArea, sizeof(_float));

    }

    out.write((char*)&m_BufferInstance.isAttach, sizeof(_bool));
    out.close();

    return S_OK;

}

HRESULT CParticle_Point::Load_Data(const string strDirectory)
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

    in.read((char*)&m_fStartTime, sizeof(_float));

    in.read((char*)&m_vStartPos, sizeof(_float4));

    in.read((char*)&m_fRotate, sizeof(_float));

    in.read((char*)&m_fLifeAlpha, sizeof(_float2));

    in.read((char*)&m_iAction, sizeof(_uint));

    in.read((char*)&m_vStartColor, sizeof(_float4));

    in.read((char*)&m_vEndColor, sizeof(_float4));

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
    in.read((char*)&m_BufferInstance.fDelay, sizeof(_float));

    if (7 == m_iShaderPass)
    {
        in.read((char*)&m_BufferInstance.LowStartRot, sizeof(_float3));
        in.read((char*)&m_BufferInstance.HighStartRot, sizeof(_float3));
        in.read((char*)&m_BufferInstance.LowAngleVelocity, sizeof(_float3));
        in.read((char*)&m_BufferInstance.HighAngleVelocity, sizeof(_float3));
        in.read((char*)&m_BufferInstance.GravityScale, sizeof(_float));
        in.read((char*)&m_BufferInstance.CrossArea, sizeof(_float));
        m_BufferInstance.isBillboard = true;
    }


    if (6 == m_iShaderPass|| 8 == m_iShaderPass)
        in.read((char*)&m_fDistortion, sizeof(_float));

    if (9 == m_iShaderPass)
    {
        in.read((char*)&m_isNormal, sizeof(_bool));

        _int strNormallength;
        char charNormalTag[MAX_PATH] = {};

        in.read((char*)&strNormallength, sizeof(_int));

        in.read(charNormalTag, strNormallength);
        string Normaltag = charNormalTag;
        m_NormalTag = m_pGameInstance->StringToWstring(Normaltag);

        in.read((char*)&m_fUVCount, sizeof(_float2));

        in.read((char*)&m_BufferInstance.LowStartRot, sizeof(_float3));
        in.read((char*)&m_BufferInstance.HighStartRot, sizeof(_float3));
        in.read((char*)&m_BufferInstance.LowAngleVelocity, sizeof(_float3));
        in.read((char*)&m_BufferInstance.HighAngleVelocity, sizeof(_float3));
        in.read((char*)&m_BufferInstance.GravityScale, sizeof(_float));
        in.read((char*)&m_BufferInstance.CrossArea, sizeof(_float));
        
    }
    in.read((char*)&m_BufferInstance.isAttach, sizeof(_bool));
    in.close();

    return S_OK;
}

HRESULT CParticle_Point::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &m_BufferInstance)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, m_TextureTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
        return E_FAIL;

    if (m_isNormal)
    {
        /* For.Com_NormalTexture */
        if (FAILED(__super::Add_Component(m_iCurrentLevel, m_NormalTag,
            TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CParticle_Point::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_isNormal", &m_isNormal, sizeof(_bool))))
        return E_FAIL;

    if (m_isNormal)
    {
        if (FAILED(m_pTextureCom[1]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", 0)))
            return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartColor", &m_vStartColor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &m_vEndColor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_lifeAlpha", &m_fLifeAlpha, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_isAttach", &m_BufferInstance.isAttach, sizeof(_bool))))
        return E_FAIL;

    _float Radian = XMConvertToRadians(m_fRotate++);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRadian", &Radian, sizeof(_float))))
        return E_FAIL;

    if (6 == m_iShaderPass|| 8 == m_iShaderPass)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionWeight", &m_fDistortion, sizeof(_float))))
            return E_FAIL;
    }

    if (9 == m_iShaderPass)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVCount", &m_fUVCount, sizeof(_float2))))
            return E_FAIL;
    }

    return S_OK;
}

CParticle_Point* CParticle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Point* pInstance = new CParticle_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CParticle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CParticle_Point* CParticle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath)
{
    CParticle_Point* pInstance = new CParticle_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
    {
        MSG_BOX("Failed To Created : CParticle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Point::Clone(void* pArg)
{
    CParticle_Point* pInstance = new CParticle_Point(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CParticle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Point::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    for (auto& iter : m_pTextureCom)
        Safe_Release(iter);
    Safe_Release(m_pShaderCom);
}
