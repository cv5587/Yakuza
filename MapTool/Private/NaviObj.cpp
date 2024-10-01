#include "../Public/NaviObj.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "NaviObj.h"


CNaviObj::CNaviObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CNaviObj::CNaviObj(const CNaviObj& rhs)
	: CGameObject{ rhs },
	m_vRouteNums {rhs.m_vRouteNums}
{
}

HRESULT CNaviObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNaviObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		NAVIOBJ_DESC* naviDesc = (NAVIOBJ_DESC*)pArg;
		m_eRoute = naviDesc->tNaviDesc;
		m_pTransformCom->Set_WorldMatrix(naviDesc->vStartPos);

		if (0 < m_eRoute.iRouteNums)
		{
			for (int i = 0; i < m_eRoute.iRouteNums; i++)
			{
				m_vRouteNums.push_back(m_eRoute.pRouteID[i]);
			}
		}
		else
		{
			m_eRoute.pRouteID = nullptr;
		}
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(0.5, 0.5, 0.5);

	return S_OK;
}

void CNaviObj::Priority_Tick(const _float& fTimeDelta)
{
}

void CNaviObj::Tick(const _float& fTimeDelta)
{

}

void CNaviObj::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CNaviObj::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		/*m_pShaderCom->Begin(m_iShaderPassNum);*/

		if(m_iColor == 1)
			m_pShaderCom->Begin(m_iColor);
		else if (m_iColor == 3)
			m_pShaderCom->Begin(m_iColor);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

ROUTE_IO CNaviObj::Get_RouteIO()
{
	// 위치 반환
	XMStoreFloat4(&m_eRoute.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//혹시 모르니 비워주고 보내기
	m_eRoute.iRouteNums = m_vRouteNums.size();
		
	if(nullptr != m_eRoute.pRouteID)
		Safe_Delete(m_eRoute.pRouteID);

	if (0 < m_eRoute.iRouteNums)
	{
		m_eRoute.pRouteID = new int[m_vRouteNums.size()];

		for (int i = 0; i < m_vRouteNums.size(); i++)
		{
			m_eRoute.pRouteID[i] = m_vRouteNums[i];
		}
	}

	
	
	return m_eRoute;
}

void CNaviObj::Delete_RouteNum_InVec(int iNum)
{
	vector<int>::iterator iter = m_vRouteNums.begin();

	if (0 != iNum)
	{
		for (int i = 0; i < iNum; i++)
		{
			iter++;
		}
	}

	m_vRouteNums.erase(iter);
}



HRESULT CNaviObj::Add_Components(void* pArg)
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Model_Bone_Sphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_RUNMAP, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CNaviObj::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fObjID", m_iObjectIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ValueFloat("g_fFar", *m_pGameInstance->Get_CamFar())))
		return E_FAIL;


	bool	bWrite;
	if (CImgui_Manager::IDWRIE::NAVIOBJ == CImgui_Manager::GetInstance()->Get_Write())
	{
		bWrite = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bWriteID", &bWrite, sizeof(bool))))
			return E_FAIL;
	}
	else
	{
		bWrite = false;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bWriteID", &bWrite, sizeof(bool))))
			return E_FAIL;
	}

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}

CNaviObj* CNaviObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNaviObj* pInstance = new CNaviObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNaviObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNaviObj::Clone(void* pArg)
{
	CNaviObj* pInstance = new CNaviObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNaviObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNaviObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (nullptr != m_eRoute.pRouteID)
		Safe_Delete(m_eRoute.pRouteID);

	m_vRouteNums.clear();
}
