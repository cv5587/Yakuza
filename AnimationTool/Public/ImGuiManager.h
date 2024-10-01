#pragma once
#include "Client_Defines.h"
#include "Base.h"

#pragma region "Imgui"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#pragma endregion

BEGIN(Engine)
class CGameInstance;
class CAnimation;
END

BEGIN(Client)
class CImguiManager final :
	public CBase
{
private:
	enum Model_Type
	{
		PLAYER, ENEMY, NPC, 
	};

	enum AnimCom_Type
	{
		MODEL_ANIMS, ANIM_COM, SYNC_COM, 
	};

	enum Collider_Type
	{
		AABB, OBB, SPHERE
	};
	enum Collider_Action_Type
	{
		ATTACK, HIT
	};
	enum Collider_Part_Type
	{
		HAND_A, FOOT_A, JOINT_A, HEAD_A,
		HEAD_H = 10, BODY_H, LEG_H,
	};

	// 선택된거는 빨간색으로 보여준다
	//콜라이더 활성화(노랑), 콜라이더 비활성화(주황), 사운드 활성화(초록)
	enum Animation_Event_Type
	{
		COLLIDER_ACTIVATION, COLLIDER_DISABLE, SOUND_ACTIVATION, 
		ANIMATION_EVENT_TYPE_END
	};

public:
	struct Animation_Event
	{
		_uint iType;
		_float fAinmPosition;
		string strChannelName;
		_uint iBoneIndex;
		_bool isSelected = { false };
	};

	struct Collider_State
	{
		string strBoneName;
		int iColliderActionType;
		int iColliderPartType;
		_bool	isAlways;
	};

	struct Animation_RimLightState
	{
		_uint iType;					//0번이 on 1번이 off
		_float fAinmPosition;
		string strMeshName;
	};

	struct Animation_TrailState
	{
		_uint iType;					//0번이 on 1번이 off
		_float fAinmPosition;
		string strBonelName;
		string strTrailProtoName;
		_uint iBoneIndex;
	};

	struct Animation_FaceEventState
	{
		_uint iType;					//0번이 on 1번이 off 2번이 change
		_float fAinmPosition;
		_uint iFaceAnimIndex;
	};

	struct Animation_BloodEventState
	{
		_float fAinmPosition;
		_uint iBoneIndex;
		string strBonelName;
		_uint iBloodEffectType;		// 출력시킬 이펙트의 타입
		_bool isLoop;				// 루프 여부
		_bool isOn;					// 이펙트 On/Off 정보
	};

	struct Animation_RadialEventState
	{
		_uint iType;				//0번이 on 1번이 off
		_float fAinmPosition;
		_float fForce;
	};

	struct Animation_SoundEventState
	{
		_uint iChannel;
		_float fAinmPosition;
		_float fSoundVolume;
		string strSoundFileName;
		_bool isPlay = { false };
	};


private:
	CImguiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImguiManager() = default;

public:
	HRESULT Initialize(void* pArg = nullptr);
	void Tick(const _float& fTimeDelta);
	HRESULT Render();

public:
	void Connect_Model_Ref();
	string Get_FirstModel_Name();

private:
	void ModelList();
	void AnimListWindow();
	void BoneListWindow();
	void KeyFrameWindow();
	void MeshListWindow();
	void AnimEventWindow();

	/* 키프레임 윈도우에서 애니메이션 이벤트에 이펙트, 사운드 추가하는 창 */
	void EffectListWindow();
	void RimLightWindow();
	void TrailWindow();
	void SoundListWindow();

	void FaceEventWindow();
	void BloodEventWindow();
	void RadialEventWindow();

	void DrawTimeline(ImDrawList* d);
	void DrawChannels();

private:
	void Update_Model_Position();
	void Update_Model_RotationX();
	void Update_Model_RotationY();
	void Update_Model_RotationZ();
	void Update_Model_Scaled();

private:
	/* Initailize */
	void Setting_Collider_Value(_uint iBoneIndex);
	void Setting_InitialData(string strFolderType);
	void Setting_ModelList(string strFolderType);

	/* Reset */
	void Reset_Collider_Value();
	void Clear_EffectStateMap();

private:
	void Create_Effect(string& strBoneName, string& strEffectName, wstring wstrLayer = TEXT("Layer_Effect"));

	void Play_SoundEvent();
	void Reset_SoundEvent_Played();

private:
	/* Save */
	void All_Save();
	void AlphaMesh_Save(string strPath);
	void AnimationLoop_Save(string strPath);
	void AnimationEvent_Save(string strPath);
	void ColliderState_Save(string strPath);
	void EffectState_Save(string strPath);
	void RimEvent_Save(string strPath);
	void TrailEvent_Save(string strPath);
	void SoundEvent_Save(string strPath);
	void FaceEvent_Save(string strPath);
	void BloodEvent_Save(string strPath);
	void RadialEvent_Save(string strPath);

	/* Load */
	void All_Load();
	void AlphaMesh_Load(string strPath);
	void AnimationLoop_Load(string strPath);
	void AnimationEvent_Load(string strPath);
	void ColliderState_Load(string strPath);
	void EffectState_Load(string strPath);
	void RimEvent_Load(string strPath);
	void TrailEvent_Load(string strPath);
	void SoundEvent_Load(string strPath);
	void FaceEvent_Load(string strPath);
	void BloodEvent_Load(string strPath);
	void RadialEvent_Load(string strPath);

	/* Functional*/
private:
	void Gui_Select_Bone(_uint iBoneIndex);
	void Setting_AnimationList();			//플레이어,적 선택한 값에 따라 애니메이션 컴포넌트를 읽어온다
	void Setting_RimLight();				// 림라이트 매시 이름 적용하기
	void Setting_Trail();				// 트레일 온오프 적용하기
	void Change_Model();

private:
	ImGuiIO* io;
	fs::path m_RootDir;

private:
	_bool					m_isOnToolWindows = { false };

	int						m_iAnimIndex = { 0 };
	int						m_iSearchAnimIndex = { 0 };
	int						m_iAddedAnimSelectedIndex = { 0 };

	int						m_iModelSelectedIndex = { 0 };

	int						m_iBoneSelectedIndex = { 0 };
	int						m_iColliderSelectedIndex = { 0 };
	int						m_iEventBoneIndex = { 0 };

	int						m_iChannelSelectedIndex = { 0 };

	int						m_iEventSelectedIndex = { 0 };

	int						m_iMeshSelectedIndex = { 0 };
	int						m_iAddedMeshSelectedIndex = { 0 };

	char									m_szSearchChannelName[_MAX_PATH];
	char									m_szSearchBoneName[_MAX_PATH];

	vector<string>							m_ModelNameList;

	vector<string>							m_AnimNameList;
	unordered_map<_uint, string>			m_AddedAnims;			// 애니메이션 인덱스와 이름을 저장한다.

	vector<string>							m_BoneNameList;
	unordered_map<_uint, Collider_State>	m_AddedColliders;		// 뼈 인덱스와 뼈 이름을 저장한다

	vector<string>							m_ChannelNameList;

	vector<string>							m_MeshNameList;
	unordered_map<_uint, string>			m_AddedMeshes;			// 매쉬 인덱스와 이름을 저장한다.

	// first: 애니메이션 이름, second: 이벤트정보
	multimap<string, Animation_Event>		m_AnimationEvents;

	//first: 뼈 이름, second: 이름
	multimap<string, string>				m_EffectState;

private:
	vector<CAnimation*>			m_Anims;

	/* 이펙트 윈도우 */
private:
	_bool						m_isEffectListWindow = { false };

	_bool						m_isEffectOff = { false };

	_uint						m_iEffectType = { 0 };
	int							m_iEffectSelectedIndex = { 0 };
	int							m_iAddedEffectSelectedIndex = { 0 };

	vector<string>				m_EffectTypeList;
	multimap<_uint, string>		m_EffectFiles;
	string						m_SelectedEffectName = "Prototype_GameObject_Particle_Trail_asd";			// 트레일 조절 창에서 사용하려고 저장함

	/* 림라이트 윈도우 */
private:
	_bool						m_isRimLightWindow = { false };

	int							m_iRimLightEventIndex = { 0 };

	// first: 애니메이션 이름, second: 림라이트 이벤트정보
	multimap<string, Animation_RimLightState>		m_RimLightEvents;

	/* 트레일 윈도우 */
private:
	_bool						m_isTrailWindow = { false };

	int							m_iTrailEventIndex = { 0 };

	// first: 애니메이션 이름, second: 트레일 이벤트정보
	multimap<string, Animation_TrailState>		m_TrailEvents;

	/* 페이스 이벤트 윈도우 */
private:
	_bool						m_isFaceWindow = { false };
	int							m_iFaceEventIndex = { 0 };
	int							m_iFaceAnimIndex = { 0 };

	// first: 애니메이션 이름, second: 트레일 이벤트정보
	multimap<string, Animation_FaceEventState>		m_FaceEvents;

	/* 블러드 이펙트 윈도우 */
private:
	_bool						m_isBloodWindow = { false };
	int							m_iBloodEventIndex = { 0 };
	int							m_eBloodEffectType = { 0 };
	bool						m_isBloodEffectLoop = { false };
	bool						m_isBloodEffectOn = { true };

	// first: 애니메이션 이름, second: 이벤트정보
	multimap<string, Animation_BloodEventState>		m_BloodEvents;

	/* 레디얼 이벤트 윈도우 */
private:
	_bool						m_isRadialEventWindow = { false };
	int							m_iRadialEventIndex = { 0 };
	float						m_fRadialForce = { 1.f };

	// first: 애니메이션 이름, second: 이벤트정보
	multimap<string, Animation_RadialEventState>		m_RadialEvents;

	/* 사운드 이벤트 윈도우 */
private:
	_bool						m_isCutSceneEvents = { false };
	_bool						m_isSoundListWindow = { false };
	CHANNELID					m_eSoundCannel = { SOUND_EFFECT };
	string						m_strSelectSoundFileName = "";
	int							m_iSoundEventIndex = { 0 };
	float						m_fSoundVolume = { 1.f };

	// first: 애니메이션 이름, second: 이벤트정보
	multimap<string, Animation_SoundEventState>		m_SoundEvents;

private:
	_float						m_fTimeDeltaScale = { 1.f };

private:
	bool						m_isAlwaysCollider = { false };
	float						m_ModelPosition[3] = { 0.f };
	float						m_ModelRotation[3] = { 0.f };
	float						m_ModelScale = { 0.f };

	float						m_fAnimationPosition = { 0.f };

	int							m_iModelType = { PLAYER };
	int							m_iModelPathType = { PLAYER };

private:
	bool						m_isPause = { false };

	int							m_iColliderType = { 0 };
	int							m_iColliderActionType = { 0 };
	int							m_iColliderPartType = { 0 };

	float						m_fColliderRadius = { 0.f };
	float						m_ColliderPosition[3] = { 0.f };
	float						m_ColliderExtents[3] = { 0.f };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	class CAnimModel* m_pRenderModel = { nullptr };

public:
	static CImguiManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END