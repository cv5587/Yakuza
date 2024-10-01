#include "FileTotalMgr.h"
#include "GameInstance.h"
#include "MapDataMgr.h"
#include "Terrain.h"
#include "GameObject.h"
#include "Construction.h"
#include "SystemManager.h"
#include "RushYakuza.h"
#include "SkyDome.h"
#include "Player.h"
#include "LightConstruction.h"
#include "WPAYakuza.h"
#include "Shakedown.h"
#include "Yoneda.h"
#include "Kuze.h"
#include "RoadNML.h"
#include "Adventure.h"
#include "Map.h"
#include "Item.h"
#include "Highway_Taxi.h"
#include "Trigger.h"
#include "RoadNML.h"
#include "RoadCAB.h"
#include "RoadStanding_NML.h"
#include "RoadTissue.h"
#include "RoadYOP.h"
#include "MonsterGroup.h"
#include "Nishiki.h"
#include "Adventure_Reactor.h"
#include "LandObject.h"

IMPLEMENT_SINGLETON(CFileTotalMgr)


CFileTotalMgr::CFileTotalMgr()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CFileTotalMgr::Tick(_float fTimeDelta)
{
    //if (nullptr != m_pCinemachineCam)
    //    m_pCinemachineCam->Tick(fTimeDelta);
}

void CFileTotalMgr::Late_Tick(_float fTimeDelta)
{
    //if (nullptr != m_pCinemachineCam)
    //    m_pCinemachineCam->Late_Tick(fTimeDelta);
}



HRESULT CFileTotalMgr::Set_MapObj_In_Client(int iMapLoadingNum, int iStageLevel)
{
    m_PlayerPosition.clear();

    for (int i = 0; i < m_MapTotalInform.iNumMapObj; i++)
    {
        if (0 < m_MapTotalInform.pMapObjDesc[i].iDecalNum)
        {
            Safe_Delete_Array(m_MapTotalInform.pMapObjDesc[i].pDecals);
        }
    }

    for (int i = 0; i < m_MapTotalInform.iNumMapObj; i++)
    {
        if (0 < m_MapTotalInform.pMapObjDesc[i].iColliderNum)
        {
            Safe_Delete_Array(m_MapTotalInform.pMapObjDesc[i].pObjColliders);
        }
    }

    for (int i = 0; i < m_MapTotalInform.iNumMapObj; i++)
    {
        Safe_Delete(m_MapTotalInform.pMapObjDesc);
    }

    Safe_Delete_Array(m_MapTotalInform.pMapObjDesc);



    Import_Bin_Map_Data_OnClient(&m_MapTotalInform, iMapLoadingNum);

    /* terrain 수정*/
    Set_Terrain_Size(iStageLevel);
    Set_GameObject_In_Client(iStageLevel);



    //Safe_Delete_Array(m_MapTotalInform.pMapObjDesc);

    return S_OK;
}


HRESULT CFileTotalMgr::Set_GameObject_In_Client(int iStageLevel)
{
    map<int, vector<CLandObject::LANDOBJ_MAPDESC>>      vGroupMonster;

    for (int i = 0; i < m_MapTotalInform.iNumMapObj; i++)
    {

        if (OBJECT_TYPE::MONSTER_RUSH == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CLandObject::LANDOBJ_MAPDESC        monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;
            monsterDesc.iGroupMonster = m_MapTotalInform.pMapObjDesc[i].iGroupMonster;
            monsterDesc.iGroupNum = m_MapTotalInform.pMapObjDesc[i].iGroupNum;
            monsterDesc.iObjectType = m_MapTotalInform.pMapObjDesc[i].iObjType;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            if (1 == m_MapTotalInform.pMapObjDesc[i].iGroupMonster)
            {
                // 그룹몬스터
                if (vGroupMonster.end() == vGroupMonster.find(monsterDesc.iGroupNum))
                {
                    // 없음
                    vector<CLandObject::LANDOBJ_MAPDESC> vDesc;
                    vDesc.push_back(monsterDesc);
                    vGroupMonster.emplace(monsterDesc.iGroupNum, vDesc);
                }
                else
                {
                    vGroupMonster.find(monsterDesc.iGroupNum)->second.push_back(monsterDesc);
                }
            }
            else
            {
                // 일반 몬스터
                m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RushYakuza"), m_Layers[iLayer], &monsterDesc);
            }

        }
        else if (OBJECT_TYPE::MONSTER_DEFAULT == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CLandObject::LANDOBJ_MAPDESC        monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iGroupMonster = m_MapTotalInform.pMapObjDesc[i].iGroupMonster;
            monsterDesc.iGroupNum = m_MapTotalInform.pMapObjDesc[i].iGroupNum;
            monsterDesc.iObjectType = m_MapTotalInform.pMapObjDesc[i].iObjType;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }
            
            if (1 == m_MapTotalInform.pMapObjDesc[i].iGroupMonster)
            {
                // 그룹몬스터
                if (vGroupMonster.end() == vGroupMonster.find(monsterDesc.iGroupNum))
                {
                    // 없음
                    vector<CLandObject::LANDOBJ_MAPDESC> vDesc;
                    vDesc.push_back(monsterDesc);
                    vGroupMonster.emplace(monsterDesc.iGroupNum, vDesc);
                }
                else
                {
                    vGroupMonster.find(monsterDesc.iGroupNum)->second.push_back(monsterDesc);
                }
            }
            else
            {
                // 일반 몬스터
                m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Default"), m_Layers[iLayer], &monsterDesc);
            }

        }
        else if (OBJECT_TYPE::MONSTER_WPA == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CWPAYakuza::MONSTER_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_WPAYakuza"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::MONSTER_SHAKEDOWN == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CRushYakuza::MONSTER_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;
            
            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Shakedown"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::MONSTER_YONEDA == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CYoneda::MONSTER_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Yoneda"), TEXT("Layer_Yoneda"), &monsterDesc);
        }
        else if (OBJECT_TYPE::MONSTER_KUZE == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CKuze::MONSTER_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Kuze"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::PLAYER == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            XMMATRIX vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);

            if (nullptr == m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))
            {
                //Layer_Taxi
                dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Taxi"), 0))->Set_StartPos(vStartPos);

                // 네비 루트 세팅
                dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Taxi"), 0))->Set_NaviRouteIndex(m_MapTotalInform.pMapObjDesc[i].iNaviRoute);

                if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
                {
                    // 예외처리
                    dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Taxi"), 0))->Set_NavigationIndex(0);
                }
                else
                {
                    dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Taxi"), 0))->Set_NavigationIndex(m_MapTotalInform.pMapObjDesc[i].iNaviNum);
                }

            }
            else
            {
                dynamic_cast<CLandObject*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))->Set_StartPos(vStartPos);

                // 네비 루트 세팅
                dynamic_cast<CLandObject*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))->Set_NaviRouteIndex(m_MapTotalInform.pMapObjDesc[i].iNaviRoute);

                if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
                {
                    // 예외처리
                    dynamic_cast<CLandObject*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))->Set_NavigationIndex(0);
                }
                else
                {
                    dynamic_cast<CLandObject*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))->Set_NavigationIndex(m_MapTotalInform.pMapObjDesc[i].iNaviNum);
                }
            }
            
        }
        else if (OBJECT_TYPE::SKY == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CSkyDome::SkyObj_Desc		skyDesc;
            skyDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            skyDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);


            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_SkyDome"), m_Layers[iLayer], &skyDesc);
        }
        else if (OBJECT_TYPE::LIGHT == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CLightConstruction::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTotalInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTotalInform.pMapObjDesc[i].iDecalNum;

            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTotalInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTotalInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTotalInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_LightConstruction"), m_Layers[iLayer], &mapDesc);
        }
        else if (OBJECT_TYPE::ADTENTURE_SUIT == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadNML"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::MONSTER_WPH == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_WPH"), m_Layers[iLayer], &monsterDesc);
        }
        else if(OBJECT_TYPE::MAP == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CMap::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.bCull = true;
            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTotalInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTotalInform.pMapObjDesc[i].iDecalNum;

            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTotalInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTotalInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTotalInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            if (nullptr != m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))
            {
                mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();
            }
            else
            {
                mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Taxi"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();
            }
           

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Map"), m_Layers[iLayer], &mapDesc);
        }
        else if (OBJECT_TYPE::MAP_NONCULL == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CMap::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.bCull = false;
            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTotalInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTotalInform.pMapObjDesc[i].iDecalNum;

            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTotalInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTotalInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTotalInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            if (nullptr != m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))
            {
                mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();
            }
            else
            {
                mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Taxi"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Map"), m_Layers[iLayer], &mapDesc);
        }
        else if (OBJECT_TYPE::ITEM == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CItem::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTotalInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTotalInform.pMapObjDesc[i].iDecalNum;
            mapDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;


            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTotalInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTotalInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTotalInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

            mapDesc.vOffsetMatrix = m_MapTotalInform.pMapObjDesc[i].vOffsetTransform;

            mapDesc.fSpeedPecSec = 1.f;

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Item"), m_Layers[iLayer], &mapDesc);
        }
        else if (OBJECT_TYPE::ROADNML == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            //Layer그냥 지정해주기
            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadNML"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::ROADCAB == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadCAB"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::ROADSTANDING_NML == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;
            monsterDesc.iObjectPropertyType = m_MapTotalInform.pMapObjDesc[i].iObjPropertyType;


            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadStanding_NML"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::ROADTISSUE == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadTissue"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::ROADYOP == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadYOP"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::NISHIKI == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CNPC::NPC_IODESC		npcDesc;
            npcDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            npcDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            npcDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            npcDesc.fSpeedPecSec = 10.f;
            npcDesc.fRotatePecSec = XMConvertToRadians(0.f);
            npcDesc.fRotatePecSec = XMConvertToRadians(180.f);
            npcDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            npcDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                npcDesc.iNaviNum = 0;
            }
            else
            {
                npcDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Nishiki"), m_Layers[7], &npcDesc);
        }
        else if (OBJECT_TYPE::ADVENTURE_REACTOR == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            CAdventure_Reactor::ADVENTURE_REACTOR_IODESC		adventureReactDesc;
            adventureReactDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            adventureReactDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            adventureReactDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;

            adventureReactDesc.fSpeedPecSec = 10.f;
            adventureReactDesc.fRotatePecSec = XMConvertToRadians(0.f);
            adventureReactDesc.fRotatePecSec = XMConvertToRadians(180.f);
            adventureReactDesc.iNaviRouteNum = m_MapTotalInform.pMapObjDesc[i].iNaviRoute;
            adventureReactDesc.iNPCDirection = m_MapTotalInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTotalInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                adventureReactDesc.iNaviNum = 0;
            }
            else
            {
                adventureReactDesc.iNaviNum = m_MapTotalInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_AdventureReactor"), m_Layers[8], &adventureReactDesc);
        }
        else if (OBJECT_TYPE::PLAYER_POSITION == m_MapTotalInform.pMapObjDesc[i].iObjType)
        {
            m_PlayerPosition.push_back(m_MapTotalInform.pMapObjDesc[i].vTransform);
        }
        else 
        {
            CConstruction::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTotalInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTotalInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTotalInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTotalInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTotalInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTotalInform.pMapObjDesc[i].iDecalNum;

            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTotalInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTotalInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTotalInform.pMapObjDesc[i].iColliderNum;
            
            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTotalInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Construction"), m_Layers[iLayer], &mapDesc);
        }
        

    }

    if (0 < vGroupMonster.size())
    {
        map<int, vector<CLandObject::LANDOBJ_MAPDESC>>::iterator        iter = vGroupMonster.begin();

        while (iter != vGroupMonster.end())
        {
            CMonsterGroup::MONSTERGROUPDESC     monsterGroupDesc;
            monsterGroupDesc.vMonsters = iter->second;
            monsterGroupDesc.iGroupNum = iter->first;

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_MonsterGroup"), TEXT("Layer_MonsterGroup"), &monsterGroupDesc);

            iter++;
        }
    }
    

    return S_OK;
}

HRESULT CFileTotalMgr::Set_Lights_In_Client(int iLightLoadingNum)
{
    Safe_Delete_Array(m_LightTotalInform.pLightDesc);

    /* 모든 라이트 삭제 */
    m_pGameInstance->Delete_AllLights();

    if(FAILED(Import_Bin_Light_Data_OnClient(&m_LightTotalInform, iLightLoadingNum)))
        return E_FAIL;

    for (int i = 0; i < m_LightTotalInform.iNumLights; i++)
    {
        LIGHT_DESC		lightDesc;
        lightDesc = m_LightTotalInform.pLightDesc[i];

        /* gameinstance에 추가 */
        m_pGameInstance->Add_Light(lightDesc);
    }

    return S_OK;
}

HRESULT CFileTotalMgr::Set_Collider_In_Client(int iColliderLoadingNum, int iStageLevel)
{
    Safe_Delete_Array(m_MapCollider.pColliderDesc);

    if (FAILED(Import_Bin_Collider_Data_OnTool(&m_MapCollider, iColliderLoadingNum)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_MapCollider"), TEXT("Layer_MapCollider"), &m_MapCollider)))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CFileTotalMgr::Set_Trigger_In_Client(int iTriggerLoadingNum, int iStageLevel)
{
    Safe_Delete_Array(m_Trigger.pTriggers);

    if (FAILED(Import_Bin_Trigger_Data_OnTool(&m_Trigger, iTriggerLoadingNum)))
        return E_FAIL;

    for (int i = 0; i < m_Trigger.iTriggerNum ; i++)
    {
        if (TRIGGER_MOVE_LEVEL == m_Trigger.pTriggers[i].iTriggerType)
        {
            CTrigger::TRIGGEROBJ_DESC       triggerObjDesc;
            triggerObjDesc.tTriggerDesc = m_Trigger.pTriggers[i];

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_LevelTrigger"), TEXT("Layer_Trigger"), &triggerObjDesc);
        }
        else if (TRIGGER_CINEMACHINE == m_Trigger.pTriggers[i].iTriggerType)
        {

        }
        else if (TRIGGER_YONEDA == m_Trigger.pTriggers[i].iTriggerType)
        {
            CTrigger::TRIGGEROBJ_DESC       triggerObjDesc;
            triggerObjDesc.tTriggerDesc = m_Trigger.pTriggers[i];

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_YonedaTrigger"), TEXT("Layer_Trigger"), &triggerObjDesc);

        }
        else if (TRIGGER_MONSTER == m_Trigger.pTriggers[i].iTriggerType)
        {
            CTrigger::TRIGGEROBJ_DESC       triggerObjDesc;
            triggerObjDesc.tTriggerDesc = m_Trigger.pTriggers[i];

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_MonsterTrigger"), TEXT("Layer_Trigger"), &triggerObjDesc);
        }
        else if (TRIGGER_QUEST == m_Trigger.pTriggers[i].iTriggerType)
        {
            CTrigger::TRIGGEROBJ_DESC       triggerObjDesc;
            triggerObjDesc.tTriggerDesc = m_Trigger.pTriggers[i];

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_QuestTrigger"), TEXT("Layer_Trigger"), &triggerObjDesc);
        }
    }

    return S_OK;
}

HRESULT CFileTotalMgr::Set_MapObj_In_Client_Trigger(int iMapLoadingNum, int iStageLevel)
{
    for (int i = 0; i < m_MapTriggerInform.iNumMapObj; i++)
    {
        if (0 < m_MapTriggerInform.pMapObjDesc[i].iDecalNum)
        {
            Safe_Delete_Array(m_MapTriggerInform.pMapObjDesc[i].pDecals);
        }
    }

    for (int i = 0; i < m_MapTriggerInform.iNumMapObj; i++)
    {
        if (0 < m_MapTriggerInform.pMapObjDesc[i].iColliderNum)
        {
            Safe_Delete_Array(m_MapTriggerInform.pMapObjDesc[i].pObjColliders);
        }
    }

    for (int i = 0; i < m_MapTriggerInform.iNumMapObj; i++)
    {
        Safe_Delete(m_MapTriggerInform.pMapObjDesc);
    }

    Safe_Delete_Array(m_MapTriggerInform.pMapObjDesc);


    Import_Bin_TriggerMap_Data_OnClient(&m_MapTriggerInform, iMapLoadingNum);


    Set_GameObject_In_Client_Trigger(iStageLevel);


    return S_OK;
}

HRESULT CFileTotalMgr::Set_GameObject_In_Client_Trigger(int iStageLevel)
{
    map<int, vector<CLandObject::LANDOBJ_MAPDESC>>      vGroupMonster;

    for (int i = 0; i < m_MapTriggerInform.iNumMapObj; i++)
    {

        if (OBJECT_TYPE::MONSTER_RUSH == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CLandObject::LANDOBJ_MAPDESC        monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;
            monsterDesc.iGroupMonster = m_MapTriggerInform.pMapObjDesc[i].iGroupMonster;
            monsterDesc.iGroupNum = m_MapTriggerInform.pMapObjDesc[i].iGroupNum;
            monsterDesc.iObjectType = m_MapTriggerInform.pMapObjDesc[i].iObjType;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            if (1 == m_MapTriggerInform.pMapObjDesc[i].iGroupMonster)
            {
                // 그룹몬스터
                if (vGroupMonster.end() == vGroupMonster.find(monsterDesc.iGroupNum))
                {
                    // 없음
                    vector<CLandObject::LANDOBJ_MAPDESC> vDesc;
                    vDesc.push_back(monsterDesc);
                    vGroupMonster.emplace(monsterDesc.iGroupNum, vDesc);
                }
                else
                {
                    vGroupMonster.find(monsterDesc.iGroupNum)->second.push_back(monsterDesc);
                }
            }
            else
            {
                // 일반 몬스터
                m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RushYakuza"), m_Layers[iLayer], &monsterDesc);
            }

        }
        else if (OBJECT_TYPE::MONSTER_DEFAULT == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CLandObject::LANDOBJ_MAPDESC        monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iGroupMonster = m_MapTriggerInform.pMapObjDesc[i].iGroupMonster;
            monsterDesc.iGroupNum = m_MapTriggerInform.pMapObjDesc[i].iGroupNum;
            monsterDesc.iObjectType = m_MapTriggerInform.pMapObjDesc[i].iObjType;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            if (1 == m_MapTriggerInform.pMapObjDesc[i].iGroupMonster)
            {
                // 그룹몬스터
                if (vGroupMonster.end() == vGroupMonster.find(monsterDesc.iGroupNum))
                {
                    // 없음
                    vector<CLandObject::LANDOBJ_MAPDESC> vDesc;
                    vDesc.push_back(monsterDesc);
                    vGroupMonster.emplace(monsterDesc.iGroupNum, vDesc);
                }
                else
                {
                    vGroupMonster.find(monsterDesc.iGroupNum)->second.push_back(monsterDesc);
                }
            }
            else
            {
                // 일반 몬스터
                m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Default"), m_Layers[iLayer], &monsterDesc);
            }

        }
        else if (OBJECT_TYPE::MONSTER_WPA == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CWPAYakuza::MONSTER_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_WPAYakuza"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::MONSTER_SHAKEDOWN == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CRushYakuza::MONSTER_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Shakedown"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::MONSTER_YONEDA == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CYoneda::MONSTER_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Yoneda"), TEXT("Layer_Yoneda"), &monsterDesc);
        }
        else if (OBJECT_TYPE::MONSTER_KUZE == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CKuze::MONSTER_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Kuze"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::PLAYER == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            XMMATRIX vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);

            if (nullptr == m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))
            {
                //Layer_Taxi
                dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Taxi"), 0))->Set_StartPos(vStartPos);

                // 네비 루트 세팅
                dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Taxi"), 0))->Set_NaviRouteIndex(m_MapTotalInform.pMapObjDesc[i].iNaviRoute);

                if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
                {
                    // 예외처리
                    dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Taxi"), 0))->Set_NavigationIndex(0);
                }
                else
                {
                    dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Taxi"), 0))->Set_NavigationIndex(m_MapTotalInform.pMapObjDesc[i].iNaviNum);
                }

            }
            else
            {
                dynamic_cast<CLandObject*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))->Set_StartPos(vStartPos);

                // 네비 루트 세팅
                dynamic_cast<CLandObject*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))->Set_NaviRouteIndex(m_MapTotalInform.pMapObjDesc[i].iNaviRoute);

                if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
                {
                    // 예외처리
                    dynamic_cast<CLandObject*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))->Set_NavigationIndex(0);
                }
                else
                {
                    dynamic_cast<CLandObject*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))->Set_NavigationIndex(m_MapTotalInform.pMapObjDesc[i].iNaviNum);
                }
            }

        }
        else if (OBJECT_TYPE::SKY == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CSkyDome::SkyObj_Desc		skyDesc;
            skyDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            skyDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);


            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_SkyDome"), m_Layers[iLayer], &skyDesc);
        }
        else if (OBJECT_TYPE::LIGHT == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CLightConstruction::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTriggerInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTriggerInform.pMapObjDesc[i].iDecalNum;

            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTriggerInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_LightConstruction"), m_Layers[iLayer], &mapDesc);
        }
        else if (OBJECT_TYPE::ADTENTURE_SUIT == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadNML"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::MONSTER_WPH == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_WPH"), m_Layers[iLayer], &monsterDesc);
        }
        else if (OBJECT_TYPE::MAP == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CMap::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.bCull = true;
            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTriggerInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTriggerInform.pMapObjDesc[i].iDecalNum;

            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTriggerInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            if (nullptr != m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))
            {
                mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();
            }
            else
            {
                mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Taxi"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();
            }


            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Map"), m_Layers[iLayer], &mapDesc);
        }
        else if (OBJECT_TYPE::MAP_NONCULL == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CMap::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.bCull = false;
            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTriggerInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTriggerInform.pMapObjDesc[i].iDecalNum;

            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTriggerInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            if (nullptr != m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Player"), 0))
            {
                mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();
            }
            else
            {
                mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Taxi"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Map"), m_Layers[iLayer], &mapDesc);
        }
        else if (OBJECT_TYPE::ITEM == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CItem::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTriggerInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTriggerInform.pMapObjDesc[i].iDecalNum;
            mapDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;


            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTriggerInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            mapDesc.vPlayerMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Get_GameObject_Component(iStageLevel, TEXT("Layer_Player"), TEXT("Com_Transform", 0)))->Get_WorldFloat4x4();

            mapDesc.vOffsetMatrix = m_MapTriggerInform.pMapObjDesc[i].vOffsetTransform;

            mapDesc.fSpeedPecSec = 1.f;

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Item"), m_Layers[iLayer], &mapDesc);
        }
        else if (OBJECT_TYPE::ROADNML == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            //Layer그냥 지정해주기
            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadNML"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::ROADCAB == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadCAB"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::ROADSTANDING_NML == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;
            monsterDesc.iObjectPropertyType = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadStanding_NML"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::ROADTISSUE == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadTissue"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::ROADYOP == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CAdventure::ADVENTURE_IODESC		monsterDesc;
            monsterDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            monsterDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            monsterDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            monsterDesc.fSpeedPecSec = 10.f;
            monsterDesc.fRotatePecSec = XMConvertToRadians(0.f);
            monsterDesc.fRotatePecSec = XMConvertToRadians(180.f);
            monsterDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            monsterDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                monsterDesc.iNaviNum = 0;
            }
            else
            {
                monsterDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_RoadYOP"), m_Layers[5], &monsterDesc);
        }
        else if (OBJECT_TYPE::NISHIKI == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CNPC::NPC_IODESC		npcDesc;
            npcDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            npcDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            npcDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            npcDesc.fSpeedPecSec = 10.f;
            npcDesc.fRotatePecSec = XMConvertToRadians(0.f);
            npcDesc.fRotatePecSec = XMConvertToRadians(180.f);
            npcDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            npcDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                npcDesc.iNaviNum = 0;
            }
            else
            {
                npcDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Nishiki"), m_Layers[7], &npcDesc);
        }
        else if (OBJECT_TYPE::ADVENTURE_REACTOR == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            CAdventure_Reactor::ADVENTURE_REACTOR_IODESC		adventureReactDesc;
            adventureReactDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            adventureReactDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            adventureReactDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;

            adventureReactDesc.fSpeedPecSec = 10.f;
            adventureReactDesc.fRotatePecSec = XMConvertToRadians(0.f);
            adventureReactDesc.fRotatePecSec = XMConvertToRadians(180.f);
            adventureReactDesc.iNaviRouteNum = m_MapTriggerInform.pMapObjDesc[i].iNaviRoute;
            adventureReactDesc.iNPCDirection = m_MapTriggerInform.pMapObjDesc[i].iNPCDirection;

            if (-1 == m_MapTriggerInform.pMapObjDesc[i].iNaviNum)
            {
                // 예외처리
                adventureReactDesc.iNaviNum = 0;
            }
            else
            {
                adventureReactDesc.iNaviNum = m_MapTriggerInform.pMapObjDesc[i].iNaviNum;
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_AdventureReactor"), m_Layers[8], &adventureReactDesc);
        }
        else if (OBJECT_TYPE::PLAYER_POSITION == m_MapTriggerInform.pMapObjDesc[i].iObjType)
        {
            m_PlayerPosition.push_back(m_MapTriggerInform.pMapObjDesc[i].vTransform);
        }
        else
        {
            CConstruction::MAPOBJ_DESC		mapDesc;
            mapDesc.vStartPos = XMLoadFloat4x4(&m_MapTriggerInform.pMapObjDesc[i].vTransform);
            int		iLayer = Find_Layers_Index(m_MapTriggerInform.pMapObjDesc[i].strLayer);

            /* Layer 정보 안들어옴 */
            if (iLayer < 0)
                return S_OK;

            mapDesc.iLayer = iLayer;
            mapDesc.wstrModelName = m_pGameInstance->StringToWstring(m_MapTriggerInform.pMapObjDesc[i].strModelCom);
            mapDesc.iShaderPass = m_MapTriggerInform.pMapObjDesc[i].iShaderPassNum;
            mapDesc.iObjType = m_MapTriggerInform.pMapObjDesc[i].iObjType;
            mapDesc.iDecalNum = m_MapTriggerInform.pMapObjDesc[i].iDecalNum;

            if (0 < mapDesc.iDecalNum)
            {
                mapDesc.pDecal = new DECAL_DESC_IO[mapDesc.iDecalNum];

                for (int j = 0; j < mapDesc.iDecalNum; j++)
                {
                    mapDesc.pDecal[j].iMaterialNum = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].iMaterialNum;
                    mapDesc.pDecal[j].vTransform = m_MapTriggerInform.pMapObjDesc[i].pDecals[j].vTransform;
                }
            }

            mapDesc.iColliderNum = m_MapTriggerInform.pMapObjDesc[i].iColliderNum;

            if (0 < mapDesc.iColliderNum)
            {
                mapDesc.pColliderDesc = new OBJCOLLIDER_DESC[mapDesc.iColliderNum];

                for (int j = 0; j < mapDesc.iColliderNum; j++)
                {
                    mapDesc.pColliderDesc[j].iColliderType = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].iColliderType;
                    mapDesc.pColliderDesc[j].vCenter = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vCenter;
                    mapDesc.pColliderDesc[j].vExtents = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vExtents;
                    mapDesc.pColliderDesc[j].vQuaternion = m_MapTriggerInform.pMapObjDesc[i].pObjColliders[j].vQuaternion;
                }
            }

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Construction"), m_Layers[iLayer], &mapDesc);
        }


    }

    if (0 < vGroupMonster.size())
    {
        map<int, vector<CLandObject::LANDOBJ_MAPDESC>>::iterator        iter = vGroupMonster.begin();

        while (iter != vGroupMonster.end())
        {
            CMonsterGroup::MONSTERGROUPDESC     monsterGroupDesc;
            monsterGroupDesc.vMonsters = iter->second;
            monsterGroupDesc.iGroupNum = iter->first;

            m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_MonsterGroup"), TEXT("Layer_MonsterGroup"), &monsterGroupDesc);

            iter++;
        }
    }


    return S_OK;
}

HRESULT CFileTotalMgr::Set_NaviRoute_In_Client(int iRouteNum)
{
    _ulong		dwByte = {};
    char fullPath[MAX_PATH];
    strcpy_s(fullPath, ".../Bin/DataFiles/RouteData/");

    strcat_s(fullPath, "Route");
    strcat_s(fullPath, "_");

    string strNum = to_string(iRouteNum);
    char cLevel[20];
    strcpy_s(cLevel, strNum.c_str());
    strcat_s(fullPath, cLevel);
    strcat_s(fullPath, ".dat");

    ifstream in(fullPath, ios::binary);

    if (!in.is_open()) {
        MSG_BOX("파일 개방 실패");
        return E_FAIL;
    }


    _uint NumCells = { 0 };
    in.read((_char*)&NumCells, sizeof(_uint));

    int* arr = new int[NumCells];
    in.read(reinterpret_cast<char*>(arr), sizeof(arr));


    in.close();

    return S_OK;
}

void CFileTotalMgr::Load_Cinemachine(int iCineNum, int iStageLevel)
{
    if (nullptr == m_pCinemachineCam)
    {
        m_pCinemachineCam = dynamic_cast<CCineCamera*>(m_pGameInstance->Get_GameObject(iStageLevel, TEXT("Layer_Camera"), 1));
        Safe_AddRef(m_pCinemachineCam);
        m_pCinemachineCam->Load_CamBin(iCineNum);
        /*m_pCinemachineCam->Initialize_Camera_Class();*/
        //m_pCinemachineCam->Setting_Start_Cinemachine();
    }
    else
    {
        m_pCinemachineCam->Load_CamBin(iCineNum);
        //m_pCinemachineCam->Setting_Start_Cinemachine();
    }

    //CSystemManager::GetInstance()->Set_Camera(CAMERA::CAMERA_CINEMACHINE);
}

void CFileTotalMgr::Setting_Start_Cinemachine(int iCineNum)
{
    m_pCinemachineCam->Setting_Start_Cinemachine(iCineNum);
}

_uint CFileTotalMgr::Get_CineCameraIndex()
{
    return m_pCinemachineCam->Get_CameraIndex();
}

void CFileTotalMgr::Reset_Cinemachine()
{
    if (nullptr != m_pCinemachineCam)
        Safe_Release(m_pCinemachineCam);
}

HRESULT CFileTotalMgr::Set_Terrain_Size(int iStageLevel)
{
    /* 땅 크기 수정*/
    CTerrain::TERRAIN_DESC terrainDesc;
    terrainDesc.vPlaneSize = m_MapTotalInform.vPlaneSize;

    if (FAILED(m_pGameInstance->Add_GameObject(iStageLevel, TEXT("Prototype_GameObject_Terrain"), TEXT("Layer_BackGround"), &terrainDesc)))
    {
        return E_FAIL;
    }

    return S_OK;
}

int CFileTotalMgr::Find_Layers_Index(char* strLayer)
{
    for (int i = 0; i < m_Layers.size(); i++)
    {
        if (0 == strcmp(strLayer, m_pGameInstance->WstringToString(m_Layers[i]).c_str()))
        {
            return i;
        }
    }

    return -1;
}

HRESULT CFileTotalMgr::Import_Bin_Map_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel)
{
    char fullPath[MAX_PATH];
    strcpy_s(fullPath, "../Bin/DataFiles/MapData/");

    strcat_s(fullPath, "MapObject_Data");
    strcat_s(fullPath, "_");

    string strNum = to_string(iLevel);
    char cLevel[20];
    strcpy_s(cLevel, strNum.c_str());
    strcat_s(fullPath, cLevel);
    strcat_s(fullPath, ".dat");

    ifstream in(fullPath, ios::binary);

    if (!in.is_open()) {
        MSG_BOX("파일 개방 실패");
        return E_FAIL;
    }

    in.read((char*)&mapObjData->iLevelIndex, sizeof(int));

    in.read((char*)&mapObjData->vPlaneSize, sizeof(XMFLOAT2));

    in.read((char*)&mapObjData->iNumMapObj, sizeof(int));

    mapObjData->pMapObjDesc = new OBJECTPLACE_DESC[mapObjData->iNumMapObj];

    for (int i = 0; i < mapObjData->iNumMapObj; i++)
    {
        //in.read((char*)&mapObjData->pMapObjDesc[i], sizeof(OBJECTPLACE_DESC));

        OBJECTPLACE_DESC* pMapObj = &mapObjData->pMapObjDesc[i];

        in.read((char*)&pMapObj->vTransform, sizeof(XMFLOAT4X4));
        in.read((char*)&pMapObj->vOffsetTransform, sizeof(XMFLOAT4X4));
        in.read((char*)&pMapObj->strLayer, sizeof(char) * MAX_PATH);
        in.read((char*)&pMapObj->strModelCom, sizeof(char) * MAX_PATH);
        in.read((char*)&pMapObj->iShaderPassNum, sizeof(int));
        in.read((char*)&pMapObj->iObjType, sizeof(int));
        in.read((char*)&pMapObj->iObjPropertyType, sizeof(int));
        in.read((char*)&pMapObj->iNPCDirection, sizeof(int));
        in.read((char*)&pMapObj->iGroupMonster, sizeof(int));
        in.read((char*)&pMapObj->iGroupNum, sizeof(int));

        in.read((char*)&pMapObj->iNaviNum, sizeof(int));
        in.read((char*)&pMapObj->iNaviRoute, sizeof(int));

        in.read((char*)&pMapObj->iDecalNum, sizeof(int));

        pMapObj->pDecals = new DECAL_DESC_IO[pMapObj->iDecalNum];

        for (int j = 0; j < pMapObj->iDecalNum; j++)
        {
            in.read((char*)&pMapObj->pDecals[j], sizeof(DECAL_DESC_IO));
        }


        in.read((char*)&pMapObj->iColliderNum, sizeof(int));

        pMapObj->pObjColliders = new OBJCOLLIDER_DESC[pMapObj->iColliderNum];

        for (int j = 0; j < pMapObj->iColliderNum; j++)
        {
            in.read((char*)&pMapObj->pObjColliders[j].iColliderType, sizeof(int));
            in.read((char*)&pMapObj->pObjColliders[j].vCenter, sizeof(_float3));
            in.read((char*)&pMapObj->pObjColliders[j].vExtents, sizeof(_float3));
            in.read((char*)&pMapObj->pObjColliders[j].vQuaternion, sizeof(_float3));
        }
    }

    in.close();

    return S_OK;
}

HRESULT CFileTotalMgr::Import_Bin_Light_Data_OnClient(LIGHT_DESC_IO* lightData, int iLevel)
{
    char fullPath[MAX_PATH];
    strcpy_s(fullPath, "../Bin/DataFiles/LightData/");

    strcat_s(fullPath, "Light_Data");
    strcat_s(fullPath, "_");


    string strNum = to_string(iLevel);
    char cLevel[20];
    strcpy_s(cLevel, strNum.c_str());

    strcat_s(fullPath, cLevel);
    strcat_s(fullPath, ".dat");

    ifstream in(fullPath, ios::binary);

    if (!in.is_open()) {
        MSG_BOX("파일 개방 실패");
        return E_FAIL;
    }


    in.read((char*)&lightData->iNumLights, sizeof(int));

    lightData->pLightDesc = new LIGHT_DESC[lightData->iNumLights];

    for (int i = 0; i < lightData->iNumLights; i++)
    {
        in.read((char*)&lightData->pLightDesc[i], sizeof(LIGHT_DESC));
    }

    in.close();

    return S_OK;
}

HRESULT CFileTotalMgr::Import_Bin_Collider_Data_OnTool(COLLIDER_IO* ColliderData, int iLevel)
{
    char fullPath[MAX_PATH];
    strcpy_s(fullPath, "../Bin/DataFiles/ColliderData/");

    strcat_s(fullPath, "Collider_Data");
    strcat_s(fullPath, "_");

    string strNum = to_string(iLevel);
    char cLevel[20];
    strcpy_s(cLevel, strNum.c_str());
    strcat_s(fullPath, cLevel);
    strcat_s(fullPath, ".dat");

    ifstream in(fullPath, ios::binary);

    if (!in.is_open()) {
        MSG_BOX("파일 개방 실패");
        return E_FAIL;
    }


    in.read((char*)&ColliderData->iColliderNum, sizeof(int));

    ColliderData->pColliderDesc = new COLLIDER_DESC[ColliderData->iColliderNum];

    for (int i = 0; i < ColliderData->iColliderNum; i++)
    {
        in.read((char*)&ColliderData->pColliderDesc[i], sizeof(COLLIDER_DESC));
    }

    in.close();

    return S_OK;
}

HRESULT CFileTotalMgr::Import_Bin_Trigger_Data_OnTool(TRIGGER_IO* TriggerData, int iLevel)
{
    char fullPath[MAX_PATH];
    strcpy_s(fullPath, "../Bin/DataFiles/TriggerData/");

    strcat_s(fullPath, "Trigger_Data");
    strcat_s(fullPath, "_");

    string strNum = to_string(iLevel);
    char cLevel[20];
    strcpy_s(cLevel, strNum.c_str());
    strcat_s(fullPath, cLevel);
    strcat_s(fullPath, ".dat");

    ifstream in(fullPath, ios::binary);

    if (!in.is_open()) {
        MSG_BOX("파일 개방 실패");
        return E_FAIL;
    }

    in.read((char*)&TriggerData->iTriggerNum, sizeof(int));

    TriggerData->pTriggers = new TRIGGER_DESC[TriggerData->iTriggerNum];

    for (int i = 0; i < TriggerData->iTriggerNum; i++)
    {
        in.read((char*)&TriggerData->pTriggers[i], sizeof(TRIGGER_DESC));
    }

    return S_OK;
}

HRESULT CFileTotalMgr::Import_Bin_TriggerMap_Data_OnClient(MAP_TOTALINFORM_DESC* mapObjData, int iLevel)
{
    char fullPath[MAX_PATH];
    strcpy_s(fullPath, "../Bin/DataFiles/TriggerMapData/");

    strcat_s(fullPath, "TriggerMapObject_Data");
    strcat_s(fullPath, "_");

    string strNum = to_string(iLevel);
    char cLevel[20];
    strcpy_s(cLevel, strNum.c_str());
    strcat_s(fullPath, cLevel);
    strcat_s(fullPath, ".dat");

    ifstream in(fullPath, ios::binary);

    if (!in.is_open()) {
        MSG_BOX("파일 개방 실패");
        return E_FAIL;
    }

    in.read((char*)&mapObjData->iLevelIndex, sizeof(int));

    in.read((char*)&mapObjData->vPlaneSize, sizeof(XMFLOAT2));

    in.read((char*)&mapObjData->iNumMapObj, sizeof(int));

    mapObjData->pMapObjDesc = new OBJECTPLACE_DESC[mapObjData->iNumMapObj];

    for (int i = 0; i < mapObjData->iNumMapObj; i++)
    {
        //in.read((char*)&mapObjData->pMapObjDesc[i], sizeof(OBJECTPLACE_DESC));

        OBJECTPLACE_DESC* pMapObj = &mapObjData->pMapObjDesc[i];

        in.read((char*)&pMapObj->vTransform, sizeof(XMFLOAT4X4));
        in.read((char*)&pMapObj->vOffsetTransform, sizeof(XMFLOAT4X4));
        in.read((char*)&pMapObj->strLayer, sizeof(char) * MAX_PATH);
        in.read((char*)&pMapObj->strModelCom, sizeof(char) * MAX_PATH);
        in.read((char*)&pMapObj->iShaderPassNum, sizeof(int));
        in.read((char*)&pMapObj->iObjType, sizeof(int));
        in.read((char*)&pMapObj->iObjPropertyType, sizeof(int));
        in.read((char*)&pMapObj->iNPCDirection, sizeof(int));
        in.read((char*)&pMapObj->iGroupMonster, sizeof(int));
        in.read((char*)&pMapObj->iGroupNum, sizeof(int));

        in.read((char*)&pMapObj->iNaviNum, sizeof(int));
        in.read((char*)&pMapObj->iNaviRoute, sizeof(int));

        in.read((char*)&pMapObj->iDecalNum, sizeof(int));

        pMapObj->pDecals = new DECAL_DESC_IO[pMapObj->iDecalNum];

        for (int j = 0; j < pMapObj->iDecalNum; j++)
        {
            in.read((char*)&pMapObj->pDecals[j], sizeof(DECAL_DESC_IO));
        }


        in.read((char*)&pMapObj->iColliderNum, sizeof(int));

        pMapObj->pObjColliders = new OBJCOLLIDER_DESC[pMapObj->iColliderNum];

        for (int j = 0; j < pMapObj->iColliderNum; j++)
        {
            in.read((char*)&pMapObj->pObjColliders[j].iColliderType, sizeof(int));
            in.read((char*)&pMapObj->pObjColliders[j].vCenter, sizeof(_float3));
            in.read((char*)&pMapObj->pObjColliders[j].vExtents, sizeof(_float3));
            in.read((char*)&pMapObj->pObjColliders[j].vQuaternion, sizeof(_float3));
        }
    }

    in.close();

    return S_OK;
}

void CFileTotalMgr::Free()
{
    for (int i = 0; i < m_MapTotalInform.iNumMapObj; i++)
    {
        if (0 < m_MapTotalInform.pMapObjDesc[i].iDecalNum)
        {
            Safe_Delete_Array(m_MapTotalInform.pMapObjDesc[i].pDecals);
        }
    }


    for (int i = 0; i < m_MapTotalInform.iNumMapObj; i++)
    {
        if (0 < m_MapTotalInform.pMapObjDesc[i].iColliderNum)
        {
            Safe_Delete_Array(m_MapTotalInform.pMapObjDesc[i].pObjColliders);
        }
    }

    for (int i = 0; i < m_MapTotalInform.iNumMapObj; i++)
    {
        Safe_Delete(m_MapTotalInform.pMapObjDesc);
    }

    Safe_Delete_Array(m_MapTotalInform.pMapObjDesc);


    Safe_Delete_Array(m_LightTotalInform.pLightDesc);
    Safe_Delete_Array(m_MapCollider.pColliderDesc);

    // 트리거로 불러오는 오브젝트
    for (int i = 0; i < m_MapTriggerInform.iNumMapObj; i++)
    {
        if (0 < m_MapTriggerInform.pMapObjDesc[i].iDecalNum)
        {
            Safe_Delete_Array(m_MapTriggerInform.pMapObjDesc[i].pDecals);
        }
    }


    for (int i = 0; i < m_MapTriggerInform.iNumMapObj; i++)
    {
        if (0 < m_MapTriggerInform.pMapObjDesc[i].iColliderNum)
        {
            Safe_Delete_Array(m_MapTriggerInform.pMapObjDesc[i].pObjColliders);
        }
    }

    for (int i = 0; i < m_MapTriggerInform.iNumMapObj; i++)
    {
        Safe_Delete(m_MapTriggerInform.pMapObjDesc);
    }

    Safe_Delete_Array(m_MapTriggerInform.pMapObjDesc);


    m_Layers.clear();

    if (nullptr != m_pCinemachineCam)
        Safe_Release(m_pCinemachineCam);

    Safe_Release(m_pGameInstance);
}