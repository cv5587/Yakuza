#include "CharacterData.h"  
#include "GameInstance.h"
#include "SystemManager.h"

#include "Effect.h"
#include "Model.h"
#include "LandObject.h"

CCharacterData::CCharacterData()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCharacterData::Initialize(CLandObject* pCharacter)
{
	m_pCharacter = pCharacter;

	wstring wstrFilePath = TEXT("../Bin/DataFiles/Character/");
	wstrFilePath += m_pCharacter->Get_ModelName() + TEXT("/");

	// 알파 적용할 메시 로드
	string strFilePath = m_pGameInstance->WstringToString(wstrFilePath);
	string strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_AlphaMeshes.dat";

	string strFileFullPath;

	//Get_ScndModelName 값이 비어있지않다면 알파메시는 다른 이름으로 읽어와야한다.
	if (m_pCharacter->Get_ScndModelName() != TEXT(""))
	{
		wstring wstrFilePath_Scnd = TEXT("../Bin/DataFiles/Character/");
		wstrFilePath_Scnd += m_pCharacter->Get_ScndModelName() + TEXT("/");

		string strFilePath_Scnd = m_pGameInstance->WstringToString(wstrFilePath_Scnd);
		string strFileName_Scnd = m_pGameInstance->WstringToString(m_pCharacter->Get_ScndModelName()) + "_AlphaMeshes.dat";

		string strFileFullPath_Scnd = strFilePath_Scnd + strFileName_Scnd;
		if (FAILED(Load_AlphaMeshes(strFileFullPath_Scnd)))
			return E_FAIL;
	}
	else
	{
		strFileFullPath = strFilePath + strFileName;
		if (FAILED(Load_AlphaMeshes(strFileFullPath)))
			return E_FAIL;
	}

	// 루프애니메이션 정보 로드
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_LoopAnimations.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_LoopAnimations(strFileFullPath)))
		return E_FAIL;
	
	// 애니메이션 이벤트 정보 로드
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_AnimationEvents.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_AnimationEvents(strFileFullPath)))
		return E_FAIL;

	// 콜라이더 정보 로드
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_Colliders.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_Colliders(strFileFullPath)))
		return E_FAIL;

	//Get_ScndModelName 값이 비어있지않다면 다른 이름으로 읽어와야한다.
	if (m_pCharacter->Get_ScndModelName() != TEXT(""))
	{
		wstring wstrFilePath_Scnd = TEXT("../Bin/DataFiles/Character/");
		wstrFilePath_Scnd += m_pCharacter->Get_ScndModelName() + TEXT("/");

		string strFilePath_Scnd = m_pGameInstance->WstringToString(wstrFilePath_Scnd);
		string strFileName_Scnd = m_pGameInstance->WstringToString(m_pCharacter->Get_ScndModelName()) + "_EffectState.dat";

		string strFileFullPath_Scnd = strFilePath_Scnd + strFileName_Scnd;
		if (FAILED(Load_EffectState(strFileFullPath_Scnd)))
			return E_FAIL;
	}
	else
	{
		// 이펙트 정보는 레이어와 오브젝트가 전부 생성된 뒤에 뼈 정보를 꺼내와서 넘겨줘야한다.
		strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_EffectState.dat";
		strFileFullPath = strFilePath + strFileName;
		if (FAILED(Load_EffectState(strFileFullPath)))
			return E_FAIL;
	}

	// 이펙트 정보는 레이어와 오브젝트가 전부 생성된 뒤에 뼈 정보를 꺼내와서 넘겨줘야한다.
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_RimEvents.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_RimLightEvent(strFileFullPath)))
		return E_FAIL;

	//Get_ScndModelName 값이 비어있지않다면 알파메시는 다른 이름으로 읽어와야한다.
	if (m_pCharacter->Get_ScndModelName() != TEXT(""))
	{
		wstring wstrFilePath_Scnd = TEXT("../Bin/DataFiles/Character/");
		wstrFilePath_Scnd += m_pCharacter->Get_ScndModelName() + TEXT("/");

		string strFilePath_Scnd = m_pGameInstance->WstringToString(wstrFilePath_Scnd);
		string strFileName_Scnd = m_pGameInstance->WstringToString(m_pCharacter->Get_ScndModelName()) + "_TrailEvents.dat";

		string strFileFullPath_Scnd = strFilePath_Scnd + strFileName_Scnd;
		if (FAILED(Load_TrailEvent(strFileFullPath_Scnd)))
			return E_FAIL;
	}
	else
	{
		strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_TrailEvents.dat";
		strFileFullPath = strFilePath + strFileName;
		if (FAILED(Load_TrailEvent(strFileFullPath)))
			return E_FAIL;
	}

	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_FaceEvents.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_FaceEvent(strFileFullPath)))
		return E_FAIL;

	// 피 이펙트 이벤트는 컷신 애니메이션 컴포넌트를 쓰고있기때문에, 사용 시 주의가 필요하다
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_BloodEffectEvents.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_BloodEffectEvent(strFileFullPath)))
		return E_FAIL;
	
	// 레디얼 이벤트는 애니메이션 컴포넌트를 쓰고있기때문에, 사용 시 주의가 필요하다
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_RadialEvents.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_RadialEvent(strFileFullPath)))
		return E_FAIL;

	//Get_ScndModelName 값이 비어있지않다면 알파메시는 다른 이름으로 읽어와야한다.
	if (m_pCharacter->Get_ScndModelName() != TEXT(""))
	{
		wstring wstrFilePath_Scnd = TEXT("../Bin/DataFiles/Character/");
		wstrFilePath_Scnd += m_pCharacter->Get_ScndModelName() + TEXT("/");

		string strFilePath_Scnd = m_pGameInstance->WstringToString(wstrFilePath_Scnd);
		string strFileName_Scnd = m_pGameInstance->WstringToString(m_pCharacter->Get_ScndModelName()) + "_SoundEvents.dat";

		string strFileFullPath_Scnd = strFilePath_Scnd + strFileName_Scnd;
		if (FAILED(Load_SoundEvent(strFileFullPath_Scnd)))
			return E_FAIL;

		strFilePath_Scnd = m_pGameInstance->WstringToString(wstrFilePath_Scnd);
		strFileName_Scnd = m_pGameInstance->WstringToString(m_pCharacter->Get_ScndModelName()) + "_CutSoundEvents.dat";

		strFileFullPath_Scnd = strFilePath_Scnd + strFileName_Scnd;
		if (FAILED(Load_CutSceneSoundEvent(strFileFullPath_Scnd)))
			return E_FAIL;
	}
	else
	{
		// 레디얼 이벤트는 애니메이션 컴포넌트를 쓰고있기때문에, 사용 시 주의가 필요하다
		strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_SoundEvents.dat";
		strFileFullPath = strFilePath + strFileName;
		if (FAILED(Load_SoundEvent(strFileFullPath)))
			return E_FAIL;

		// 레디얼 이벤트는 애니메이션 컴포넌트를 쓰고있기때문에, 사용 시 주의가 필요하다
		strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_CutSoundEvents.dat";
		strFileFullPath = strFilePath + strFileName;
		if (FAILED(Load_CutSceneSoundEvent(strFileFullPath)))
			return E_FAIL;
	}



	
	return S_OK;
}

void CCharacterData::Set_CurrentAnimation(string strAnimName)
{
	// 일반 애니메이션 이벤트 설정해주기
	m_CurrentEvents.clear();
	auto lower_bound_iter = m_AnimationEvents.lower_bound(strAnimName);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (!(lower_bound_iter != m_AnimationEvents.end() && (*lower_bound_iter).first != strAnimName))
	{
		auto upper_bound_iter = m_AnimationEvents.upper_bound(strAnimName);

		if (lower_bound_iter == upper_bound_iter && lower_bound_iter != m_AnimationEvents.end())
		{
			m_CurrentEvents.push_back((*lower_bound_iter).second);
		}
		else
		{
			for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
			{
				m_CurrentEvents.push_back((*lower_bound_iter).second);
			}
		}
	}


	//림라이트 이벤트 설정해주기
	m_CurrentRimEvents.clear();
	auto rim_lower_bound_iter = m_RimLightEvents.lower_bound(strAnimName);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (!(rim_lower_bound_iter != m_RimLightEvents.end() && (*rim_lower_bound_iter).first != strAnimName))
	{
		auto rim_upper_bound_iter = m_RimLightEvents.upper_bound(strAnimName);

		if (rim_lower_bound_iter == rim_upper_bound_iter && rim_lower_bound_iter != m_RimLightEvents.end())
		{
			m_CurrentRimEvents.push_back((*rim_lower_bound_iter).second);
		}
		else
		{
			for (; rim_lower_bound_iter != rim_upper_bound_iter; ++rim_lower_bound_iter)
			{
				m_CurrentRimEvents.push_back((*rim_lower_bound_iter).second);
			}
		}
	}

	//트레일 이벤트 설정해주기
	m_CurrentTrailEvents.clear();
	auto trail_lower_bound_iter = m_TrailEvents.lower_bound(strAnimName);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (!(trail_lower_bound_iter != m_TrailEvents.end() && (*trail_lower_bound_iter).first != strAnimName))
	{
		auto trail_upper_bound_iter = m_TrailEvents.upper_bound(strAnimName);

		if (trail_lower_bound_iter == trail_upper_bound_iter && trail_lower_bound_iter != m_TrailEvents.end())
		{
			m_CurrentTrailEvents.push_back((*trail_lower_bound_iter).second);
		}
		else
		{
			for (; trail_lower_bound_iter != trail_upper_bound_iter; ++trail_lower_bound_iter)
			{
				m_CurrentTrailEvents.push_back((*trail_lower_bound_iter).second);
			}
		}
	}

	// 바꾸기 전에 초기화해주기.
	if (m_iCurrentAnimName != strAnimName)
	{
		for (auto& pSoundEvent : m_CurrentSoundEvents)
			pSoundEvent->isPlayed = false;

		m_iCurrentAnimName = strAnimName;
	}

	m_CurrentSoundEvents.clear();
	auto sound_lower_bound_iter = m_SoundEvents.lower_bound(strAnimName);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (!(sound_lower_bound_iter != m_SoundEvents.end() && (*sound_lower_bound_iter).first != strAnimName))
	{
		auto sound_upper_bound_iter = m_SoundEvents.upper_bound(strAnimName);

		if (sound_lower_bound_iter == sound_upper_bound_iter && sound_lower_bound_iter != m_SoundEvents.end())
		{
			m_CurrentSoundEvents.push_back(&(*sound_lower_bound_iter).second);
		}
		else
		{
			for (; sound_lower_bound_iter != sound_upper_bound_iter; ++sound_lower_bound_iter)
			{
				m_CurrentSoundEvents.push_back(&(*sound_lower_bound_iter).second);
			}
		}
	}
}

void CCharacterData::Set_CurrentCutSceneAnimation(string strAnimName)
{
	//피 이펙트 이벤트 설정해주기
	m_CurrentFaceEvents.clear();
	// 반복자를 사용하여 문자열이 포함된 키를 찾기
	for (auto it = m_FaceEvents.begin(); it != m_FaceEvents.end(); ++it) {
		if (it->first.find(strAnimName) != string::npos) {
			m_CurrentFaceEvents.push_back((*it).second);
		}
	}

	auto face_lower_bound_iter = m_FaceEvents.lower_bound(strAnimName);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (face_lower_bound_iter != m_FaceEvents.end())
	{
		auto face_upper_bound_iter = m_FaceEvents.upper_bound(strAnimName);

		for (; face_lower_bound_iter != face_upper_bound_iter; ++face_lower_bound_iter)
		{
			// 추가적으로, 부분 문자열을 포함하는지 확인
			if (face_lower_bound_iter->first.find(strAnimName) != string::npos)
			{
				m_CurrentFaceEvents.push_back(face_lower_bound_iter->second);
			}
		}
	}

	//피 이펙트 이벤트 설정해주기
	m_CurrentBloodEffectEvents.clear();
	// 반복자를 사용하여 문자열이 포함된 키를 찾기
	for (auto it = m_BloodEvents.begin(); it != m_BloodEvents.end(); ++it) {
		if (it->first.find(strAnimName) != string::npos) {
			m_CurrentBloodEffectEvents.push_back((*it).second);
		}
	}

	auto bloodEffect_lower_bound_iter = m_BloodEvents.lower_bound(strAnimName);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (bloodEffect_lower_bound_iter != m_BloodEvents.end())
	{
		auto bloodEffect_upper_bound_iter = m_BloodEvents.upper_bound(strAnimName);

		for (; bloodEffect_lower_bound_iter != bloodEffect_upper_bound_iter; ++bloodEffect_lower_bound_iter)
		{
			// 추가적으로, 부분 문자열을 포함하는지 확인
			if (bloodEffect_lower_bound_iter->first.find(strAnimName) != string::npos)
			{
				m_CurrentBloodEffectEvents.push_back(bloodEffect_lower_bound_iter->second);
			}
		}
	}

	//라디얼 이벤트 설정해주기
	m_CurrentRadialEvents.clear();
	// 반복자를 사용하여 문자열이 포함된 키를 찾기
	for (auto it = m_RadialEvents.begin(); it != m_RadialEvents.end(); ++it) {
		if (it->first.find(strAnimName) != string::npos) {
			m_CurrentRadialEvents.push_back((*it).second);
		}
	}

	auto radial_lower_bound_iter = m_RadialEvents.lower_bound(strAnimName);

	// 반환된 iter의 키값이 다르다면 맵 내에 해당 키값이 존재하지 않는다는 뜻
	if (radial_lower_bound_iter != m_RadialEvents.end())
	{
		auto radial_upper_bound_iter = m_RadialEvents.upper_bound(strAnimName);

		for (; radial_lower_bound_iter != radial_upper_bound_iter; ++radial_lower_bound_iter)
		{
			// 추가적으로, 부분 문자열을 포함하는지 확인
			if (radial_lower_bound_iter->first.find(strAnimName) != std::string::npos)
			{
				m_CurrentRadialEvents.push_back(radial_lower_bound_iter->second);
			}
		}
	}

	// 바꾸기 전에 초기화해주기.
	if (m_iCurrentAnimName != strAnimName)
	{
		for (auto& pSoundEvent : m_CurrentSoundEvents)
			pSoundEvent->isPlayed = false;

		m_iCurrentAnimName = strAnimName;
	}

	m_CurrentSoundEvents.clear();
	for (auto iter = m_CutSceneSoundEvents.begin(); iter != m_CutSceneSoundEvents.end(); ++iter)
	{
		// 키값이 strAnimName을 포함하고 있는지 확인
		if (iter->first.find(strAnimName) != std::string::npos)
		{
			m_CurrentSoundEvents.push_back(&iter->second);
		}
	}
	
}

HRESULT CCharacterData::Load_AlphaMeshes(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_AlphaMeshes Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("AlphaMeshes 개방 실패");
			return E_FAIL;
		}

		_uint iNumMeshes = { 0 };
		in.read((char*)&iNumMeshes, sizeof(_uint));

		m_AlphaMeshes.reserve(iNumMeshes);

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			_uint iMeshIndex = { 0 };
			in.read((char*)&iMeshIndex, sizeof(_uint));

			m_AlphaMeshes.push_back(iMeshIndex);
		}
		in.close();
	}

	return S_OK;
}

HRESULT CCharacterData::Load_LoopAnimations(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_LoopAnimations Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("LoopAnimations 개방 실패");
			return E_FAIL;
		}

		_uint iNumAnimations;
		in.read(reinterpret_cast<char*>(&iNumAnimations), sizeof(iNumAnimations));

		m_LoopAnimations.reserve(iNumAnimations);

		for (size_t i = 0; i < iNumAnimations; i++)
		{
			// 키를 이진 형식으로 읽어옴
			_uint iAnimationIndex = 0;
			in.read(reinterpret_cast<char*>(&iAnimationIndex), sizeof(iAnimationIndex));

			// 문자열 길이와 문자열을 이진 형식으로 읽어옴
			_uint length = 0;
			in.read(reinterpret_cast<char*>(&length), sizeof(length));

			string strAnimName(length, ' ');
			in.read(&strAnimName[0], length);

			m_LoopAnimations.push_back(iAnimationIndex);
		}

		in.close();
	}

	return S_OK;
}

HRESULT CCharacterData::Load_AnimationEvents(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_AnimationEvents Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("AnimationEvents 개방 실패");
			return E_FAIL;
		}

		_uint iNumEvents = { 0 };

		in >> iNumEvents;

		for (size_t i = 0; i < iNumEvents; i++)
		{
			string strAnimName = "";
			ANIMATION_EVENT EventDesc{};

			in >> strAnimName;
			in >> EventDesc.iType;
			in >> EventDesc.fPlayPosition;
			in >> EventDesc.strChannelName;
			in >> EventDesc.iBoneIndex;

			m_AnimationEvents.emplace(strAnimName, EventDesc);
		}

		in.close();
	}

	return S_OK;
}

HRESULT CCharacterData::Load_Colliders(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_Colliders Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("Colliders 개방 실패");
			return E_FAIL;
		}

		_uint iNumColliders = { 0 };
		in.read((char*)&iNumColliders, sizeof(_uint));

		for (size_t i = 0; i < iNumColliders; i++)
		{
			_uint iBoneIndex = { 0 };

			COLLIDER_STATE ColliderDesc{};

			in.read((char*)&iBoneIndex, sizeof(_uint));
			in.read((char*)&ColliderDesc.iType, sizeof(_uint));
			in.read((char*)&ColliderDesc.vCenter, sizeof(_float3));

			// 2는 Sphere라서 Vlaue가 Radius라 float1개만 쓴다
			if (2 == ColliderDesc.iType)
				in.read((char*)&ColliderDesc.vValue, sizeof(_float));
			else
				in.read((char*)&ColliderDesc.vValue, sizeof(_float3));

			in.read((char*)&ColliderDesc.iActionType, sizeof(_int));
			in.read((char*)&ColliderDesc.iPartType, sizeof(_int));
			in.read((char*)&ColliderDesc.isAlways, sizeof(_bool));

			m_Colliders.emplace(iBoneIndex, ColliderDesc);
		}

		in.close();
	}

	return S_OK;
}

HRESULT CCharacterData::Load_EffectState(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_EffectState Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("EffectState 개방 실패");
			return E_FAIL;
		}

		_uint iNumEffectState = { 0 };
		in >> iNumEffectState;

		for (size_t i = 0; i < iNumEffectState; i++)
		{
			string strBoneName = "";
			in >> strBoneName;

			string strEffectName = "";
			in >> strEffectName;

			m_Effects.emplace(strBoneName, m_pGameInstance->StringToWstring(strEffectName));
		}

		in.close();
	}

	return S_OK;
}

HRESULT CCharacterData::Load_RimLightEvent(string strFilePath)
{
	/*
		in >> key;
		in >> RimEvent.iType;
		in >> RimEvent.strMeshName;
		in >> RimEvent.fAinmPosition;
	*/
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_RimLightEvent Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("RimLightEvent 개방 실패");
			return E_FAIL;
		}

		_uint iNumRimLightEvent = { 0 };
		in >> iNumRimLightEvent;

		for (size_t i = 0; i < iNumRimLightEvent; i++)
		{
			string strAnimName = "";
			in >> strAnimName;				//Key값으로 쓰일 애니메이션 이름

			ANIMATION_RIMLIGHTSTATE Desc{};

			in >> Desc.iType;
			in >> Desc.strMeshName;
			in >> Desc.fAinmPosition;

			m_RimLightEvents.emplace(strAnimName, Desc);
		}

		in.close();
	}

	return S_OK;
}

HRESULT CCharacterData::Load_TrailEvent(string strFilePath)
{
	// 트레일 생섣해야함
	/*
		in >> key;
		in >> TrailEvent.iType;
		in >> TrailEvent.strBonelName;
		in >> TrailEvent.iBoneIndex;
		in >> TrailEvent.fAinmPosition;
	*/
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_RimLightEvent Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("RimLightEvent 개방 실패");
			return E_FAIL;
		}

		_uint iNumRimLightEvent = { 0 };
		in >> iNumRimLightEvent;

		for (size_t i = 0; i < iNumRimLightEvent; i++)
		{
			string strAnimName = "";
			in >> strAnimName;				//Key값으로 쓰일 애니메이션 이름

			ANIMATION_TRAILSTATE Desc{};

			in >> Desc.iType;
			in >> Desc.strBonelName;
			in >> Desc.strTrailProtoName;
			in >> Desc.iBoneIndex;
			in >> Desc.fAinmPosition;

			m_TrailEvents.emplace(strAnimName, Desc);
		}

		in.close();
	}
	return S_OK;
}

HRESULT CCharacterData::Load_FaceEvent(string strFilePath)
{
	// 트레일 생섣해야함
/*
	_float fAinmPosition;
	_uint iBoneIndex;
	string strBonelName;
	_uint iBloodEffectType;
*/
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_FaceEvent Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("FaceEvent 개방 실패");
			return E_FAIL;
		}

		_uint iNumEvent = { 0 };
		in >> iNumEvent;

		for (size_t i = 0; i < iNumEvent; i++)
		{
			string strAnimName = "";
			in >> strAnimName;				//Key값으로 쓰일 애니메이션 이름

			ANIMATION_FACEEVENTSTATE Desc{};

			in >> Desc.iType;
			in >> Desc.fAinmPosition;
			in >> Desc.iFaceAnimIndex;

			m_FaceEvents.emplace(strAnimName, Desc);
		}

		in.close();
	}
	return S_OK;
}

HRESULT CCharacterData::Load_BloodEffectEvent(string strFilePath)
{
	// 트레일 생섣해야함
	/*
		_float fAinmPosition;
		_uint iBoneIndex;
		string strBonelName;
		_uint iBloodEffectType;
	*/
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_BloodEffect Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("BloodEffect 개방 실패");
			return E_FAIL;
		}

		_uint iNumEvent = { 0 };
		in >> iNumEvent;

		for (size_t i = 0; i < iNumEvent; i++)
		{
			string strAnimName = "";
			in >> strAnimName;				//Key값으로 쓰일 애니메이션 이름

			ANIMATION_BLOODEVENTSTATE Desc{};

			in >> Desc.fAinmPosition;
			in >> Desc.iBoneIndex;
			in >> Desc.strBonelName;
			in >> Desc.iBloodEffectType;
			in >> Desc.isLoop;
			in >> Desc.isOn;

			m_BloodEvents.emplace(strAnimName, Desc);
		}

		in.close();
	}
	return S_OK;
}

HRESULT CCharacterData::Load_RadialEvent(string strFilePath)
{
	// 트레일 생섣해야함
	/*
		_uint iType;				//0번이 on 1번이 off
		_float fAinmPosition;
		_float fForce;
	*/
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_RadialEvent Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("RadialEvent 개방 실패");
			return E_FAIL;
		}

		_uint iNumEvent = { 0 };
		in >> iNumEvent;

		for (size_t i = 0; i < iNumEvent; i++)
		{
			string strAnimName = "";
			in >> strAnimName;				//Key값으로 쓰일 애니메이션 이름

			ANIMATION_RADIALEVENTSTATE Desc{};

			in >> Desc.iType;
			in >> Desc.fAinmPosition;
			in >> Desc.fForce;

			m_RadialEvents.emplace(strAnimName, Desc);
		}

		in.close();
	}
	return S_OK;
}

HRESULT CCharacterData::Load_SoundEvent(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_SoundEvent Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("SoundEvent 개방 실패");
			return E_FAIL;
		}

		_uint iNumEvent = { 0 };
		in >> iNumEvent;

		/*
		*   _uint iChannel;
			_float fAinmPosition;
			_float fSoundVolume;
			string strSoundFileName;
		*/

		for (size_t i = 0; i < iNumEvent; i++)
		{
			string strAnimName = "";
			in >> strAnimName;				//Key값으로 쓰일 애니메이션 이름

			ANIMATION_SOUNDEVENTSTATE Desc{};

			in >> Desc.iChannel;
			in >> Desc.fAinmPosition;
			in >> Desc.fSoundVolume;
			in >> Desc.strSoundFileName;

			m_SoundEvents.emplace(strAnimName, Desc);
		}

		in.close();
	}

	return S_OK;
}

HRESULT CCharacterData::Load_CutSceneSoundEvent(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
#ifdef _DEBUG
		cout << "_CutSoundEvent Yes!!" << endl;
#endif // _DEBUG

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("CutSoundEvent 개방 실패");
			return E_FAIL;
		}

		_uint iNumEvent = { 0 };
		in >> iNumEvent;

		/*
		*   _uint iChannel;
			_float fAinmPosition;
			_float fSoundVolume;
			string strSoundFileName;
		*/

		for (size_t i = 0; i < iNumEvent; i++)
		{
			string strAnimName = "";
			in >> strAnimName;				//Key값으로 쓰일 애니메이션 이름

			ANIMATION_SOUNDEVENTSTATE Desc{};

			in >> Desc.iChannel;
			in >> Desc.fAinmPosition;
			in >> Desc.fSoundVolume;
			in >> Desc.strSoundFileName;

			m_CutSceneSoundEvents.emplace(strAnimName, Desc);
		}

		in.close();
	}

	return S_OK;
}

CCharacterData* CCharacterData::Create(CLandObject* pCharacter)
{
	CCharacterData* pInstnace = new CCharacterData();

	if (FAILED(pInstnace->Initialize(pCharacter)))
	{
		MSG_BOX("Failed To Created : CCharacterData");
		Safe_Release(pInstnace);
	}

	return pInstnace;
}

void CCharacterData::Free()
{
	Safe_Release(m_pGameInstance);
}
