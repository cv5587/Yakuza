#include "Weapon_Gun_Cz75.h"

#include "GameInstance.h"
#include "Shader.h"

CGun_Cz75::CGun_Cz75(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSocketModel{ pDevice, pContext }
{
}

CGun_Cz75::CGun_Cz75(const CGun_Cz75& rhs)
	:CSocketModel{ rhs }
{
}

HRESULT CGun_Cz75::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGun_Cz75::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	CZ75_DESC* pDesc = static_cast<CZ75_DESC*>(pArg);

	_vector vAxis;
	if (pDesc->iLocalRotAxis == 0)
	{
		vAxis = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	}
	else if (pDesc->iLocalRotAxis == 1)
	{
		vAxis = m_pTransformCom->Get_State(CTransform::STATE_UP);
	}
	else
	{
		vAxis = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	}

	_vector vPos = XMLoadFloat3(&pDesc->vLocalPos);

	m_pTransformCom->Change_Rotation(vAxis, pDesc->fLocalAngle);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1));

	return S_OK;
}

void CGun_Cz75::Priority_Tick(const _float& fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CGun_Cz75::Tick(const _float& fTimeDelta)
{
	//if (m_pGameInstance->GetKeyState(DIK_UP) == HOLD)
	//{
	//	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta);
	//}
	//if (m_pGameInstance->GetKeyState(DIK_DOWN) == HOLD)
	//{
	//	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), -fTimeDelta);
	//}
	//if (m_pGameInstance->GetKeyState(DIK_LEFT) == HOLD)
	//{
	//	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta);
	//}
	//if (m_pGameInstance->GetKeyState(DIK_RIGHT) == HOLD)
	//{
	//	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -fTimeDelta);
	//}

	__super::Tick(fTimeDelta);
}

void CGun_Cz75::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CGun_Cz75::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);		//µðÆúÆ®
		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

HRESULT CGun_Cz75::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Gun_Cz75"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGun_Cz75::Bind_ResourceData()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	//if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CGun_Cz75* CGun_Cz75::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGun_Cz75* pInstance = new CGun_Cz75(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CGun_Cz75::Clone(void* pArg)
{
	CGun_Cz75* pInstance = new CGun_Cz75(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGun_Cz75::Free()
{
	__super::Free();
}
