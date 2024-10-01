#include "AStart.h"

#include "GameInstance.h"

CAStart::CAStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent {pDevice, pContext}
{
}

CAStart::CAStart(const CAStart& rhs)
	: CComponent { rhs }
{
}

HRESULT CAStart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAStart::Initialize(void* pArg)
{
#ifdef _DEBUG
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Line.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	m_pCellShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

void CAStart::Start_Root(CNavigation* pNavi, _int iGoalIndex)
{
	_int iStartIndex = pNavi->Get_Index();

	if (iGoalIndex == -1)
		return;

	Data_Release();
#ifdef _DEBUG
	Safe_Release(m_pVIBufferCom);
#endif // _DEBUG

	vector<CCell*> Cells = pNavi->Get_Cells();

	if (Make_Route(iStartIndex, iGoalIndex, Cells))
	{
		Make_BastList(iStartIndex, iGoalIndex, Cells);

		auto iter = m_BestList.begin();
		Make_FunnelList(iStartIndex, iGoalIndex, iter, Cells);

#ifdef _DEBUG
		m_pVIBufferCom = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_BestList);
		if (nullptr == m_pVIBufferCom)
			MSG_BOX("VIBuffer_Lint 생성 불가");
#endif // _DEBUG
	}
}

#ifdef _DEBUG
HRESULT CAStart::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix.m[3][1] += 0.2f;
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

	_vector vColor = { 1.f, 0.f, 0.f, 1.f };
	m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix.m[3][1] += 0.2f;
	m_pCellShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pCellShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
	m_pCellShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

	m_pCellShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));

	for (auto& pCell : m_FunnelList)
	{
		m_pCellShaderCom->Begin(0);
		pCell->Render();
	}

	return S_OK;
}
#endif // _DEBUG

//인접한 셀의 코스트 구하기
_bool CAStart::Make_Route(_uint iStartIndex, _uint iGoalIndex, vector<class CCell*> Cells)
{
	_uint iStartIdx = iStartIndex;

	while (true)
	{
		if (!m_OpenList.empty())
			m_OpenList.pop_front();

		m_CloseList.push_back(iStartIdx);

		for (size_t i = 0; i < CCell::LINE_END; ++i)
		{
			_int iNeighborIndex = Cells[iStartIdx]->Get_NeighborIndex(i);
			if (iNeighborIndex == -1)
				continue;

			CCell* pCell = Cells[iNeighborIndex];

			if (iGoalIndex == pCell->Get_Index())
			{
				pCell->Set_ParentIndex(iStartIdx);
				return true;
			}

			if (false == Check_Close(pCell->Get_Index()) &&
				false == Check_Open(pCell->Get_Index())) {
				pCell->Set_ParentIndex(iStartIdx);
				m_OpenList.push_back(pCell->Get_Index());
			}
		}

		if (m_OpenList.empty())
			return false;

		_uint iStart = iStartIndex;

		m_OpenList.sort([&](int Dst, int Src) -> _bool {
			//Cost = PCost + GCost

			_vector vPCost1 = Cells[iStart]->Get_WayPoint() - Cells[Dst]->Get_WayPoint();
			_vector vPCost2 = Cells[iStart]->Get_WayPoint() - Cells[Src]->Get_WayPoint();

			_vector vGCost1 = Cells[iGoalIndex]->Get_WayPoint() - Cells[Dst]->Get_WayPoint();
			_vector vGCost2 = Cells[iGoalIndex]->Get_WayPoint() - Cells[Src]->Get_WayPoint();

			_float fCost1 = XMVectorGetX(XMVector3Length(vPCost1)) + XMVectorGetX(XMVector3Length(vGCost1));
			_float fCost2 = XMVectorGetX(XMVector3Length(vPCost2)) + XMVectorGetX(XMVector3Length(vGCost2));

			return fCost1 < fCost2;
		});

		iStartIdx = m_OpenList.front();
	}
}

void CAStart::Make_BastList(_uint iStartIndex, _uint iGoalIndex, vector<class CCell*> Cells)
{
	m_BestList.push_front(Cells[iGoalIndex]);

	_uint iRouteIndex = Cells[iGoalIndex]->Get_ParentIndex();

	while (true)
	{
		if (iRouteIndex == iStartIndex)
			break;

		m_BestList.push_front(Cells[iRouteIndex]);
		iRouteIndex = Cells[iRouteIndex]->Get_ParentIndex();
	}
}

// Stupid Funnel 알고리즘 사용하여 길찾기 보안하기
void CAStart::Make_FunnelList(_uint iStartIndex, _uint iGoalIndex, list<CCell*>::iterator iter, vector<class CCell*> Cells)
{
	if (iStartIndex == -1 || iGoalIndex == -1)
		return;
	// 현재 캐릭터의 위치에서 다음 노드의 맞닿아 있는 면까지를 최초 프러스텀으로 하여,
	//  다음 경로의 맞닿은 면이 이전 프러스텀에 포함되게끔 재귀적으로 다음 프러스텀을 구하여,
	// 프러스텀 면 크기가 캐릭터의 크기보다 작아지게 되면 그 위치까지 직선으로 이동하는 것.
	// 프러스텀과 포탈이 전혀 겹치지 않은 경우, 이 포탈은 통과 불가능하다.

	//플레이어의 첫 위치를 기점으로 프러스텀을 제작한다 (최초의 프러스텀)
	_vector vFrustum[3]; // vFrusteum의 0번째를 시작점의 기준
	_uint iIndex = iStartIndex;

	//스타트 지점에서 프러스트 구하기 
	_uint iLine = Cells[iIndex]->Get_NeighborLine((*iter)->Get_Index());
	
	_vector vPoint1, vPoint2;
	if (iLine == CCell::LINE_AB)
	{
		vPoint1 = Cells[iIndex]->Get_Point(CCell::POINT_A);
		vPoint2 = Cells[iIndex]->Get_Point(CCell::POINT_B);

		vFrustum[0] = Cells[iIndex]->Get_Point(CCell::POINT_C);
	}
	else if (iLine == CCell::LINE_BC)
	{
		vPoint1 = Cells[iIndex]->Get_Point(CCell::POINT_B);
		vPoint2 = Cells[iIndex]->Get_Point(CCell::POINT_C);
		vFrustum[0] = Cells[iIndex]->Get_Point(CCell::POINT_A);
	}
	else if (iLine == CCell::LINE_CA)
	{
		vPoint1 = Cells[iIndex]->Get_Point(CCell::POINT_C);
		vPoint2 = Cells[iIndex]->Get_Point(CCell::POINT_A);

		vFrustum[0] = Cells[iIndex]->Get_Point(CCell::POINT_B);
	}
	else return;

	vFrustum[1] = vPoint1;
	vFrustum[2] = vPoint2;

	iIndex = (*iter)->Get_Index();

	while (true)
	{
		// 인접해있는 엣지를 구한다.
		if (iIndex == iGoalIndex)
			break;

		++iter;

		_uint iLine = Cells[iIndex]->Get_NeighborLine((*iter)->Get_Index());

		_vector vPoint1, vPoint2;
		if (iLine == CCell::LINE_AB)
		{
			vPoint1 = Cells[iIndex]->Get_Point(CCell::POINT_A);
			vPoint2 = Cells[iIndex]->Get_Point(CCell::POINT_B);
		}
		else if (iLine == CCell::LINE_BC)
		{
			vPoint1 = Cells[iIndex]->Get_Point(CCell::POINT_B);
			vPoint2 = Cells[iIndex]->Get_Point(CCell::POINT_C);
		}
		else if (iLine == CCell::LINE_CA)
		{
			vPoint1 = Cells[iIndex]->Get_Point(CCell::POINT_C);
			vPoint2 = Cells[iIndex]->Get_Point(CCell::POINT_A);
		}

		//벡터곱을 통해서 값 구하기
		_bool isResult[2] = { false, false };
		if (Triarea2(vFrustum[0], vFrustum[1], vPoint1) <= 0.f)  //왼 result <= 0.f
		{
			if (Triarea2(vFrustum[0], vFrustum[2], vPoint1) > 0.f) // 오 result >= 0.f
			{
				vFrustum[1] = vPoint1;
				isResult[0] = true;
			}
		}

		if (Triarea2(vFrustum[0], vFrustum[2], vPoint2) >= 0.f)
		{
			if (Triarea2(vFrustum[0], vFrustum[1], vPoint2) < 0.f)
			{
				vFrustum[2] = vPoint2;
				isResult[1] = true;
			}
		}
		
		if (!isResult[0] && !isResult[1])
			break;

		iIndex = (*iter)->Get_Index();
	}
	
	//루프를 빠져 나오면 새로운 셀을 생성해서 저장한다.
	_float3 vPosition[3];
	XMStoreFloat3(&vPosition[0], vFrustum[0]);
	XMStoreFloat3(&vPosition[1], vFrustum[1]);
	XMStoreFloat3(&vPosition[2], vFrustum[2]);

	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPosition, 0, CCell::OPTION_NONE);
	if (nullptr == pCell)
		MSG_BOX("셀 생성 실패");

	m_FunnelList.emplace_back(pCell);

	if (iIndex != iGoalIndex)
		Make_FunnelList(iIndex, iGoalIndex, iter, Cells);
}

_bool CAStart::Check_Close(_uint iIndex)
{
	for (_uint& iCloseIndex : m_CloseList)
	{
		if (iIndex == iCloseIndex)
			return true;
	}

	return false;
}

_bool CAStart::Check_Open(_uint iIndex)
{
	for (_uint& iOpenIndex : m_OpenList)
	{
		if (iIndex == iOpenIndex)
			return true;
	}

	return false;
}

void CAStart::Data_Release()
{
	m_OpenList.clear();
	m_CloseList.clear();
	m_BestList.clear();

	for (auto& pCell : m_FunnelList)
		Safe_Release(pCell);
	m_FunnelList.clear();
}

_float CAStart::Triarea2(_vector vPos1, _vector vPos2, _vector vPos3)
{
	_float iResult = (XMVectorGetX(vPos2) - XMVectorGetX(vPos1)) * (XMVectorGetZ(vPos3) - XMVectorGetZ(vPos1)) - (XMVectorGetZ(vPos2) - XMVectorGetZ(vPos1)) * (XMVectorGetX(vPos3) - XMVectorGetX(vPos1));

	return iResult;
}

CAStart* CAStart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAStart* pInstance = new CAStart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* CAStart::Clone(void* pArg)
{
	CAStart* pInstance = new CAStart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAStart::Free()
{
	__super::Free();

#ifdef _DEBUG
	Data_Release();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pCellShaderCom);
#endif // _DEBUG
}
