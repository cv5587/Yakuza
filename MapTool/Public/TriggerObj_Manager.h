#pragma once

#include "Base.h"
#include "imgui.h"
#include "GameObject.h"
#include "Construction.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

/* 데이터 추가할때마다 수정 */

class CTriggerObj_Manager final : public CBase
{
	DECLARE_SINGLETON(CTriggerObj_Manager);

public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* 그냥 건축물 */
		ROAD,
		ITEM,
		MONSTER_RUSH,
		PLAYER,
		SKY,
		LIGHT,
		MONSTER_WPA,
		MONSTER_SHAKEDOWN,
		MONSTER_YONEDA,
		MONSTER_KUZE,
		LARGE_CONSTRUCTION,
		ADTENTURE_SUIT,
		MAP, /* 도로 맵 안의 큰 맵 */
		MONSTER_WPH,
		MONSTER_DEFAULT,
		MAP_NONCULL,


		ROADNML,
		ROADCAB,
		ROADSTANDING_NML,
		ROADTISSUE,
		ROADYOP,
		NISHIKI,
		ADVENTURE_REACTOR,

		PLAYER_POSITION, // 포지션만 저장
		OBJ_END
	};

	//Edit 함수 사용하는중인것
	enum EDITTYPE
	{
		OBJECT,
		DECAL,
		COLLIDER,
		EDITTYPE_END
	};

	enum ColliderType
	{
		AABB,
		OBB,
		SPHERE,
		COLLIDER_END
	};

public:
	typedef struct tObjPlaceDesc
	{
		/* 오브젝트 설치할 때 고르는 항목 저장 */
		int				iLayer;
		int				iShaderPass;
		int				iObjType;
		int				iObjPropertyType;
		int				iNPCDirection;
		int				iNaviRouteNum;
		int				iGroupMonster; //bool 역할
		int				iGroupNum;
		XMFLOAT4X4		vOffsetMatrix;
	}MAPTOOL_OBJPLACE_DESC;


private:
	CTriggerObj_Manager();
	virtual ~CTriggerObj_Manager() = default;

public:
	HRESULT					Initialize();
	void					Priority_Tick(const _float& fTimeDelta);
	void					Tick(const _float& fTimeDelta);
	void					Late_Tick(const _float& fTimeDelta);
	void					Render();

public:
	void					Show_Installed_GameObjectsList(); /* imgui에 보여줄 아이들 */
	void					Save_GameObject(int iLevel); /* 현재 map에 놓여진 object 저장*/
	void					Load_GameObject(int iNum);
	void					Edit_Installed_GameObject(int iNumObject);
	void					Edit_GameObject_Transform(int iNumObject);
	void					Get_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC* mapDesc, int iNumObject);
	void					Edit_CurrentGameObject_Desc(CConstruction::MAPOBJ_DESC* mapDesc, int iNumObject);
	/* object 배치 */
	bool					Add_CloneObject_Imgui(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex);
	/* Object 종류 고르기 */
	void					Set_Map_Object();
	void					Add_Decal_IMGUI();
	void					Add_ObjectCollider_IMGUI();


private:
	void					Load_ModelName();
	HRESULT					Import_Bin_Map_Data_OnTool(MAP_TOTALINFORM_DESC* mapObjData, char* fileName);
	HRESULT					Export_Bin_Map_Data(MAP_TOTALINFORM_DESC* mapObjData);

	string					modifyString(string& input);
	void					Show_ExampleModel(MAPTOOL_OBJPLACE_DESC objDesc, _uint iFolderNum, _uint iObjectIndex);
	string					Find_ModelName(_uint iFolderNum, _uint iObjectIndex);

	void					Delete_Object(int iNumObject); /* 선택 object 삭제 */
	void					Update_ObjectNameList();

	void					Show_FileName();
	void					Update_FileName();

	void					Click_To_Select_Object(int& iObjectNum);
	int						Find_Layers_Index(char* strLayer);

	void					Get_Decal_Texture(int iNumObject);
	void					Update_DecalNameList();

	/* gameobject 안에 설치된 decal을 보여줌 */
	void					Update_DecalNameList_In_Object();
	void					Update_ColliderNameList();
	void					Update_ColliderList_In_Object();


	void					Update_DecalList_In_Object();
	void					Edit_Installed_Decal(int iObjectDecalNum);

	void					Reset_Collider();


private:
	vector<const char*>							m_Layers = { "Layer_GameObjects", "Layer_Monster", "Layer_Player", "Layer_NPC" , "Layer_Item", "Layer_Passerby", "Layer_MonsterGroup", "Layer_Nishiki" , "Layer_AdventureReactor" };
	/* object이름, cgameobject */
	multimap<wstring, CGameObject*>				m_GameObjects; /* 추가한 오브젝트 저장 */
	vector<char*>								m_ObjectNames_Map0; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_ObjectNames_Map1; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_ObjectNames_Map2; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_ObjectNames_Map3; /* 추가한 오브젝트 이름들 */
	vector<char*>								m_ObjectNames_MapKaraoke; /* 추가한 오브젝트 이름들 */

	vector<char*>								m_MonsterNames; /* 추가한 오브젝트 이름들 */

	vector<char*>								m_FileNames; /* 맵마다 저장한 bin 리스트들 */
	vector<char*>								m_DecalNames;
	vector<char*>								m_DecalNames_Obj;
	vector<char*>								m_ColliderNames;

	class CGameInstance* m_pGameInstance = { nullptr };
	/* 설치 전 무슨 오브젝트인지 보여주는 */
	CGameObject* m_pShownObject = { nullptr };
	bool										m_bShowExample = { false };
	vector<char*>								m_ObjectNames; /* 맵에 설치된 아이들에 대한 리스트 */
	class CDecal* m_pDecal;

private:
	bool										m_bFindObject = { false };
	bool										m_bAddDecal_IMGUI = { false };
	bool										m_bAddObjectCollider_IMGUI = { false };
	_uint										m_iObjectNums = 0;
	/* 배치관련 bool 변수 */
	_bool										m_bDoing_Place_Object = false;
	/* 마우스 Lbtn 누르면 한번만 추가되게 */
	_bool										m_bInstallOneTime = { false };
	CConstruction::MAPOBJ_DESC					m_tCurrentObjectDesc;
	int											m_iCurrentObjectIndex = { 0 };

	int											m_iCurrentCollidrNum = { 0 };

	vector<DECAL_DESC>							m_Decals; /* 해당 오브젝트에 있는 모든 decal을 저장 */
	vector<CDecal*>								m_ObjectDecals;
	vector<CCollider*>							m_ObjectColliders;
	int											m_iDecalObjNum = { -1 };

	EDITTYPE									m_eEditType;

	OBJCOLLIDER_DESC							m_tCurColliderDesc;
	int											m_iCurColliderIndex = { 0 };

public:
	virtual void Free() override;
};

