#include "Navigation.h"

#include "Shader.h"
#include "GameInstance.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& rhs)
    : CComponent{ rhs },
    m_pShaderCom{ rhs.m_pShaderCom },
    m_Routes{ rhs.m_Routes }
{

    Safe_AddRef(m_pShaderCom);

    for (auto& pCell : rhs.m_Cells)
    {
        m_Cells.push_back(pCell);
        Safe_AddRef(pCell);
    }
        
}

void CNavigation::Set_Points(const _float3* vPoints, _int OptionType)
{
    CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_iIndexCount++, CCell::OPTION(OptionType));
    if (nullptr == pCell)
        return;

    m_Cells.emplace_back(pCell);
}

vector<ROUTE_IO> CNavigation::Get_RouteIndexs(_int iIndex)
{
    return m_Routes.find(iIndex)->second;
}

HRESULT CNavigation::Initialize_Prototype()
{
    m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    m_bOrigin = true;

    return S_OK;
}

HRESULT CNavigation::Initialize_Prototype(const wstring strFilePath)
{
    m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_Cell.hlsl", VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    // 파일 열어서 가져오기 
    if (FAILED(Load_File(strFilePath)))
        return E_FAIL;

    if (FAILED(SetUp_Neighbors()))
        return E_FAIL;

    m_bOrigin = true;

    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);
        m_iRouteDir = pDesc->iCurrentRouteDir;
        m_iCurrentRouteIndex = pDesc->iCurrentLine;
        m_iPreRouteIndex = m_iCurrentRouteIndex;
        m_iCurrentWayPointIndex = pDesc->iWayPointIndex;
        m_iPreWayPointIndex = m_iCurrentWayPointIndex;
        if (m_iCurrentWayPointIndex == -1) Find_WayPointIndex(pDesc->vPosition);

        if(0 < m_Routes.size() && 0 <= m_iCurrentRouteIndex && 0 <= m_iCurrentWayPointIndex)
            m_iCurrentIndex = Find_PlayerMonster_Index(XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition));
    }

    

    return S_OK;
}

void CNavigation::Tick()
{
}

int CNavigation::Find_PlayerMonster_Index(_fvector vTargetPos)
{
    for (int i = 0; i < m_Cells.size(); i++)
    {
        /* 그냥 넣어주는거 */
        int			iNeighborsIndex;

        _vector vSlidingNormal;
        if (true == m_Cells[i]->isIn(vTargetPos, &iNeighborsIndex, vSlidingNormal))
        {
            return i;
        }
    }

    return -1;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_WorldMatrix.m[3][1] += 0.2f;
    m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

    m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
    m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

    for (auto& pCell : m_Cells)
    {
        _vector vColor{};
        if (pCell->Get_Option() == CCell::OPTION_NONE)
        {
            vColor = XMVectorSet(0.f, 0.f, 1.f, 1.f);
            m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));
        }

        else if (pCell->Get_Option() == CCell::OPTION_STAIRS)
        {
            vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);
            m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vector));
        }

        m_pShaderCom->Begin(0);

        pCell->Render();
    }

    return S_OK;
}

_bool CNavigation::isRemove(_int iIndex)
{
    for (auto iter = m_Cells.begin(); iter != m_Cells.end();)
    {
        if ((*iter)->Get_Index() == iIndex)
        {
            Safe_Release(*iter);
            m_Cells.erase(iter);

            return true;
        }
        else ++iter;
    }
    return false;
}
#endif // _DEBUG



HRESULT CNavigation::Load_File(const wstring strFilePath)
{
    ifstream ifs(strFilePath, ios::binary | ios::in);

    if (ifs.fail())
        return E_FAIL;

    _uint NumCells = { 0 };
    ifs.read((_char*)&NumCells, sizeof(_uint));

    for (size_t i = 0; i < NumCells; ++i)
    {
        _float3* vPoint = new _float3[3];
        ifs.read((_char*)vPoint, sizeof(_float3) * 3);

        _int iOption = { 0 };
        ifs.read((_char*)&iOption, sizeof(_int));

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoint, i, CCell::OPTION(iOption));

        m_Cells.emplace_back(pCell);
        Safe_Delete(vPoint);
    }

    // 루트 개수 읽어오기
    _uint		iRouteCnt = 0;
    ifs.read((_char*)&iRouteCnt, sizeof(_uint));




    for (int i = 0; i < iRouteCnt; i++)
    {
        vector<ROUTE_IO>		routeCells;

        _uint		iCellCnt = 0;

        ifs.read((_char*)&iCellCnt, sizeof(_uint));
        
        for (int j = 0; j < iCellCnt; j++)
        {
            ROUTE_IO    routePoint;

            ifs.read((char*)&routePoint.iCellNums, sizeof(int));
            ifs.read((_char*)&routePoint.vPosition, sizeof(_float4));
            ifs.read((char*)&routePoint.iPointOption, sizeof(int));

            // 루트 인덱스 저장
            ifs.read((char*)&routePoint.iRouteNums, sizeof(int));

            if (0 < routePoint.iRouteNums)
            {
                routePoint.pRouteID = new int[routePoint.iRouteNums];

                for (int k = 0; k < routePoint.iRouteNums; k++)
                {
                    ifs.read((char*)&routePoint.pRouteID[k], sizeof(int));
                }
            }

            routeCells.push_back(routePoint);
        }

        m_Routes.emplace(i, routeCells);

    }

    ifs.close();

    return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDescCell : m_Cells)
        {
            if (pSourCell == pDescCell)
                continue;

            if (pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
                pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDescCell);

            if (pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
                pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDescCell);

            if (pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
                pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDescCell);
        }
    }

    return S_OK;
}

void CNavigation::Find_WayPointIndex(_vector vPosition)
{
    //포지션으로 웨이포인트 인덱스 만들기
    _float fMinDistance = 0.f;
    _int iIndex = 0;
    for (auto& vWayPoint : m_Routes[m_iCurrentRouteIndex])
    {
        _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vWayPoint.vPosition) - vPosition));

        ++iIndex;

        if (fMinDistance == 0.f || fMinDistance >= fDistance)
        {
            if (iIndex >= m_Routes[m_iCurrentRouteIndex].size())
                iIndex = 0;

            m_iCurrentWayPointIndex = iIndex;
            m_iPreWayPointIndex = iIndex == 0 ? m_Routes[m_iCurrentRouteIndex].size() - 1 : iIndex - 1;
            fMinDistance = fDistance;
        }
    }

    if (0 < m_Routes.size() && 0 < m_iCurrentRouteIndex && 0 < m_iCurrentWayPointIndex)
        m_vNextDir = XMVector3Normalize(XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition) - XMLoadFloat4(&m_Routes[m_iPreRouteIndex][m_iPreWayPointIndex].vPosition));
}

//Index가 코너 일 경우 스왑한다.
//모든 인덱스의 처음과 끝은 코너로 생성한다고 가정한다.
//정방향일 경우 루트의 첫 인덱스는 코너에서 제외한다.
//인접한 루트 + 내가 가지고 있는 루트를 고려한다.
void CNavigation::Swap_Route(vector<ROUTE_IO> CurrentRoute, _vector vCurrnetDir, _bool* isTurn, _int* iDir)
{
    if (*isTurn)
        return;

    if (CurrentRoute[m_iCurrentWayPointIndex].iPointOption == CORNEL)
    {
        // DIR_F : 정방향
        // DIR_B : 역방향
        // m_iCurrent : 목표삼은 index
        // 코너 바꿀때
        if ((m_iRouteDir == DIR_F && m_iCurrentWayPointIndex == 0) ||
            (m_iRouteDir == DIR_B && m_iCurrentWayPointIndex == CurrentRoute.size() - 1))
            return;

        // 코너일떄 route index 저장하고 있었음
        // 가고 있는 방향 index와 코너가 중간에 있을때도 고려.
        // Route : 스왑할수있는 선택지를 저장
        vector<_uint> Route = { m_iCurrentRouteIndex };
        for (size_t i = 0; i < CurrentRoute[m_iCurrentWayPointIndex].iRouteNums; ++i)
            Route.emplace_back(CurrentRoute[m_iCurrentWayPointIndex].pRouteID[i]);

        //랜덤으로 Route를 스왑.
        _uint iRandomIdex = m_pGameInstance->Get_Random(0, _int(Route.size() - 1));
        _uint iRouteInx = Route[iRandomIdex];

        if (iRouteInx == m_iCurrentRouteIndex || 0 == Route.size())
        {
            return;
        }

           
        m_iCurrentRouteIndex = iRouteInx;
        if (m_iRouteDir == DIR_F)
            m_iCurrentWayPointIndex = 0; // 정방향일 경우
        else if (m_iRouteDir == DIR_B)
            m_iCurrentWayPointIndex = m_Routes[m_iCurrentRouteIndex].size() - 1; // 역방향일 경우

        _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(vCurrnetDir));
        //이동하는 방향 구하기
        _float fAngle = XMVectorGetX(XMVector3Dot(XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition), vRight));
        
        // iDir : 회전방향
        if (fAngle > 0)
            *iDir = DIR_R;
        else if (fAngle < 0)
            *iDir = DIR_L;

        // 회전해야하는지 안해야하는지 체크
        *isTurn = true;
    }
}



CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring strFilePath)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
    {
        MSG_BOX("Failed To Created : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNavigation");
        Safe_Release(pInstance);
    }



    return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

_bool CNavigation::isMove(_fvector vMovePos)
{
    _int iNeighborsIndex = { -1 };

    // 셀 안에 있는지?
    if (m_Cells[m_iCurrentIndex]->isIn(vMovePos, &iNeighborsIndex, m_vSlidingNormal))
        return true;
    else
    {
        if (-1 != iNeighborsIndex)
        {
            // 셀 밖에 있는지?
            while (true)
            {
                if (-1 == iNeighborsIndex)
                    return false;

                if (m_Cells[iNeighborsIndex]->isIn(vMovePos, &iNeighborsIndex, m_vSlidingNormal))
                {
                    m_iCurrentIndex = iNeighborsIndex;
                    return true;
                }

            }
        }
        else
            return false;
    }
}

_vector CNavigation::Compute_WayPointDir(_vector vPosition, const _float& fTimeDelta, _bool isStart)
{
    //현재 플레이어의 위치에서 다음 웨이포인트까지의 방향벡터를 구한다.
    vector<ROUTE_IO> CurrentRoute = m_Routes[m_iCurrentRouteIndex];
    _vector vCurrentWayPoint = XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition);
    _vector vDir = vCurrentWayPoint - vPosition;
    _float fDistance = XMVectorGetX(XMVector3Length(vDir));

    _vector vResultDir;
    if (m_vPreDir.m128_f32[0] == 0.f)
    {
        vResultDir = XMVector3Normalize(vDir);
    }
    else
    {
        //방향벡터 선형보간
        m_fTime += fTimeDelta * 6.f;
        vResultDir = XMVectorLerp(m_vPreDir, m_vNextDir, m_fTime >= 1.f ? 1.f : m_fTime);
    }

    if (fDistance <= (isStart ? 20.f : m_fMaxDistance))
    {
        m_iCurrentWayPointIndex++;

        // 인덱스가 배열의 길이보다 크다면 다시 초기값으로 돌아간다.
        if (m_iCurrentWayPointIndex >= m_Routes[m_iCurrentRouteIndex].size())
            m_iCurrentWayPointIndex = 0.f;

        if (fDistance == 0.f)
        {
            return Compute_WayPointDir(vPosition, fTimeDelta, isStart);
        }

        if (fDistance == 0.f)
        {
            return Compute_WayPointDir(vPosition, fTimeDelta, isStart);
        }

        m_vPreDir = XMVector3Normalize(vDir);
        m_vNextDir = XMVector3Normalize(XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition) - XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iPreWayPointIndex].vPosition));
        m_fTime = 0.f;
        m_iPreWayPointIndex = m_iCurrentWayPointIndex;
    }

    return vResultDir;
}



_vector CNavigation::Compute_WayPointDir_Adv(_vector vPosition, const _float& fTimeDelta, _bool* isTurn, _int* iDir, _bool* isBack)
{
    // 현재 내 앞의 waypoint
    _vector vCurrentWayPoint = XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition);
    _vector vDir = vCurrentWayPoint - vPosition;
    _float fDistance = XMVectorGetX(XMVector3Length(vDir));

    // 특정 range 안에 들어오면 다음 point로 목표잡기
    if (fDistance <= 2.5f)
    {
        // 스왑되면
        m_iPreRouteIndex = m_iCurrentRouteIndex;
        m_iPreWayPointIndex = m_iCurrentWayPointIndex;

        // 코너일때 waypoint swap을 위한 함수
        Swap_Route(m_Routes[m_iCurrentRouteIndex], vDir, isTurn, iDir);

        // turn하면 
        // isBack : 뒤돌지아닐지
        // 뒤도는 모션 - isBack
        // 회전중일때는 뒤로가는 모션 안나오게
        // isBack : 뒤도는거고 - 순수 뒤도는거
        // isTurn : 루트바꿔서 회전하는거
        if (!*isTurn)
        {
            // 정방향 - 플러스, 역방향 - 마이너스로 다음 지점 정해주기
            if (m_iRouteDir == DIR_F)
                m_iCurrentWayPointIndex++;
            else
                m_iCurrentWayPointIndex--;

            if (m_iCurrentWayPointIndex >= m_Routes[m_iCurrentRouteIndex].size() || m_iCurrentWayPointIndex < 0)
            {
                //정방향 -> 역방향
                //역방향 -> 정방향 
                if (m_iRouteDir == DIR_F)
                {
                    m_iRouteDir = DIR_B;
                    m_iCurrentWayPointIndex = m_Routes[m_iCurrentRouteIndex].size() - 1;
                }
                else
                {
                    m_iRouteDir = DIR_F;
                    m_iCurrentWayPointIndex = 0;
                }
                *isBack = true;
            }
        }

        // 예외처리해준거(아예 포인트에서 시작할떄)
        if (fDistance == 0.f)
        {
            return Compute_WayPointDir_Adv(vPosition, fTimeDelta, isTurn, iDir, isBack);
        }

        // 현재 웨이포인트와 다음 웨이포인트 - 방향 구하는식 -> 이걸로 움직임
        m_vNextDir = XMVector3Normalize(XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition) - XMLoadFloat4(&m_Routes[m_iPreRouteIndex][m_iPreWayPointIndex].vPosition));

        
    }

    return m_vNextDir;
}



_vector CNavigation::Compute_NishikiDir(_fvector vPosition, const _float& fTimeDelta, _bool* isFinished)
{
    // 현재 내 앞의 waypoint
    _vector vCurrentWayPoint = XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition);
    _vector vDir = vCurrentWayPoint - vPosition;
    _float fDistance = XMVectorGetX(XMVector3Length(vDir));

    if (fDistance <= 1.3f)
    {
        m_iCurrentWayPointIndex++;

        if (m_iCurrentWayPointIndex >= m_Routes[m_iCurrentRouteIndex].size())
        {
            *isFinished = true;
            return vDir;
        }

        // 예외처리해준거(아예 포인트에서 시작할떄)
        if (fDistance == 0.f)
        {
            return Compute_NishikiDir(vPosition, fTimeDelta, isFinished);
        }

        // 현재 웨이포인트와 다음 웨이포인트 - 방향 구하는식 -> 이걸로 움직임
        // 현재 웨이포인트와 다음 웨이포인트 - 방향 구하는식 -> 이걸로 움직임
        m_vNextDir = XMVector3Normalize(XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iCurrentWayPointIndex].vPosition) - XMLoadFloat4(&m_Routes[m_iCurrentRouteIndex][m_iPreWayPointIndex].vPosition));

        // 스왑되면
        m_iPreWayPointIndex = m_iCurrentWayPointIndex;
    }

    return m_vNextDir;
}

_float CNavigation::Compute_Height(_fvector vPosition)
{
    // 예외처리
    if (-1 == m_iCurrentIndex)
        m_iCurrentIndex = 0;

    return m_Cells[m_iCurrentIndex]->Compute_Height(vPosition);
}

void CNavigation::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);

    
    for (auto& Pair : m_Routes)
    {
        for (auto& route : Pair.second)
        {
            if (0 < route.iRouteNums && true == m_bOrigin)
                Safe_Delete(route.pRouteID);
        }
        Pair.second.clear();
    }

    m_Routes.clear();
    
    

    for (auto& pCell : m_Cells)
        Safe_Release(pCell);
    m_Cells.clear();
}