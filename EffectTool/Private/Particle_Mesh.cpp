#include "Particle_Mesh.h"
#include "GameInstance.h"
#include "VIBuffer_Instance_Mesh.h"
#include "Mesh.h"
CParticle_Mesh::CParticle_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CEffect{ pDevice , pContext }
{
}

CParticle_Mesh::CParticle_Mesh(const CParticle_Mesh& rhs)
    :CEffect{ rhs },
    m_BufferInstance{ rhs.m_BufferInstance },
    m_strModelTag{rhs.m_strModelTag }
{
    
}

void* CParticle_Mesh::Get_Instance()
{
    return &m_BufferInstance;
}

HRESULT CParticle_Mesh::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CParticle_Mesh::Initialize_Prototype(string strFilePath)
{
    if (FAILED(Load_Data(strFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Mesh::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        PARTICLE_MESH_DESC* pDesc = static_cast<PARTICLE_MESH_DESC*>(pArg);



        if (nullptr == pDesc->pWorldMatrix)
        {
            PARTICLE_MESH_DESC* pDesc = static_cast<PARTICLE_MESH_DESC*>(pArg);
            m_BufferInstance = pDesc->BufferInstance;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vStartPos));
            m_strModelTag = pDesc->strModelTag;
        }
        else
        {
            m_pWorldMatrix = pDesc->pWorldMatrix;
            m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pWorldMatrix));
        }
    }

    m_BufferInstance.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Mesh::Priority_Tick(const _float& fTimeDelta)
{
}

void CParticle_Mesh::Tick(const _float& fTimeDelta)
{
    m_fCurTime += fTimeDelta;
    if (!m_BufferInstance.isLoop)
    {
        _float fTotalTime = m_fStartTime + m_BufferInstance.vLifeTime.y;
        if (m_fCurTime > fTotalTime)
            m_isDead = true;
    }

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

    }


}

void CParticle_Mesh::Late_Tick(const _float& fTimeDelta)
{

    if (m_BufferInstance.isLoop)
    {
        if(0==m_iShaderPass)
            m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
        else if(1== m_iShaderPass)
            m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
    }
    else
    {
        if (m_fCurTime >= m_fStartTime && !m_isDead)
        {
            if (0 == m_iShaderPass)
                m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
            else if (1 == m_iShaderPass)
                m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
        }
    }

}

HRESULT CParticle_Mesh::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    

      m_pShaderCom->Begin(m_iShaderPass);

      m_pVIBufferCom->Render();
    

    return S_OK;
}

HRESULT CParticle_Mesh::Save_Data(const string strDirectory)
{
    string Directory = strDirectory;
    string ParticleTag = m_pGameInstance->WstringToString(m_ParticleTag);
    string TextureTag = m_pGameInstance->WstringToString(m_TextureTag);
    string ModelTag = m_pGameInstance->WstringToString(m_strModelTag);

    string headTag = "Prototype_GameObject_Particle_Mesh_";
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

    _int strModellength = ModelTag.length();
    out.write((char*)&strModellength, sizeof(_int));
    out.write(ModelTag.c_str(), strModellength);

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
    out.write((char*)&m_BufferInstance.vLifeTime, sizeof(_float2));
    out.write((char*)&m_BufferInstance.isLoop, sizeof(_bool));

    out.write((char*)&m_BufferInstance.LowStartRot, sizeof(_float3));
    out.write((char*)&m_BufferInstance.HighStartRot, sizeof(_float3));
    out.write((char*)&m_BufferInstance.LowAngleVelocity, sizeof(_float3));
    out.write((char*)&m_BufferInstance.HighAngleVelocity, sizeof(_float3));

    out.write((char*)&m_BufferInstance.GravityScale, sizeof(_float));
    out.write((char*)&m_BufferInstance.CrossArea, sizeof(_float));
    out.write((char*)&m_BufferInstance.isBillboard, sizeof(_bool));

    out.write((char*)&m_BufferInstance.vMinSpeed, sizeof(_float3));
    out.write((char*)&m_BufferInstance.vMaxSpeed, sizeof(_float3));
    out.write((char*)&m_BufferInstance.fWeight, sizeof(_float2));
    out.write((char*)&m_BufferInstance.vMinFrequency, sizeof(_float3));
    out.write((char*)&m_BufferInstance.vMaxFrequency, sizeof(_float3));
    out.write((char*)&m_BufferInstance.vMinAmplitude, sizeof(_float3));
    out.write((char*)&m_BufferInstance.vMaxAmplitude, sizeof(_float3));

    out.write((char*)&m_BufferInstance.isAttach, sizeof(_bool));

    out.close();

    return S_OK;
}

HRESULT CParticle_Mesh::Load_Data(const string strDirectory)
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

    _int strModellength;
    char charModelTag[MAX_PATH] = {};

    in.read((char*)&strModellength, sizeof(_int));

    in.read(charModelTag, strModellength);
    string modeltag = charModelTag;
    m_strModelTag = m_pGameInstance->StringToWstring(modeltag);


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
    in.read((char*)&m_BufferInstance.vLifeTime, sizeof(_float2));
    in.read((char*)&m_BufferInstance.isLoop, sizeof(_bool));


      in.read((char*)&m_BufferInstance.LowStartRot, sizeof(_float3));
      in.read((char*)&m_BufferInstance.HighStartRot, sizeof(_float3));
      in.read((char*)&m_BufferInstance.LowAngleVelocity, sizeof(_float3));
      in.read((char*)&m_BufferInstance.HighAngleVelocity, sizeof(_float3));

      in.read((char*)&m_BufferInstance.GravityScale, sizeof(_float));
      in.read((char*)&m_BufferInstance.CrossArea, sizeof(_float));
      in.read((char*)&m_BufferInstance.isBillboard, sizeof(_bool));

      in.read((char*)&m_BufferInstance.vMinSpeed, sizeof(_float3));
      in.read((char*)&m_BufferInstance.vMaxSpeed, sizeof(_float3));
      in.read((char*)&m_BufferInstance.fWeight, sizeof(_float2));
      in.read((char*)&m_BufferInstance.vMinFrequency, sizeof(_float3));
      in.read((char*)&m_BufferInstance.vMaxFrequency, sizeof(_float3));
      in.read((char*)&m_BufferInstance.vMinAmplitude, sizeof(_float3));
      in.read((char*)&m_BufferInstance.vMaxAmplitude, sizeof(_float3));

      in.read((char*)&m_BufferInstance.isAttach, sizeof(_bool));

      in.close();

    return S_OK;
}

HRESULT CParticle_Mesh::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, m_strModelTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

   

    CVIBuffer_Instance_Mesh::INSTANCE_MESH_DESC MeshDesc{};

    MeshDesc.InstanceDesc = &m_BufferInstance;
    MeshDesc.pMeshs = dynamic_cast<CModel*>(m_pModelCom)->Get_Meshes()[0];

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Instance_Mesh"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &MeshDesc)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture0 */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, m_TextureTag,
        TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
        return E_FAIL;

    if(m_isNormal)
    {
        /* For.Com_Texture1 */
        if (FAILED(__super::Add_Component(m_iCurrentLevel, m_NormalTag,
            TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CParticle_Mesh::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", 0, aiTextureType_DIFFUSE)))
    //    return E_FAIL;

    if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if(m_isNormal)
    {
        if (FAILED(m_pTextureCom[1]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", 0)))
            return E_FAIL;
    }

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
    //    return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartColor", &m_vStartColor, sizeof(_float4))))
    //    return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &m_vEndColor, sizeof(_float4))))
    //    return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_lifeAlpha", &m_fLifeAlpha, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_isAttach", &m_BufferInstance.isAttach, sizeof(_bool))))
        return E_FAIL;

    return S_OK;
}

CParticle_Mesh* CParticle_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Mesh* pInstance = new CParticle_Mesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CParticle_Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CParticle_Mesh* CParticle_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath)
{
    CParticle_Mesh* pInstance = new CParticle_Mesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
    {
        MSG_BOX("Failed To Created : CParticle_Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Mesh::Clone(void* pArg)
{
    CParticle_Mesh* pInstance = new CParticle_Mesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CParticle_Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Mesh::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    for (auto& iter : m_pTextureCom)
        Safe_Release(iter);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
