/*
* 24.06.26 혜원 추가
*/

#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CCharacterData : public CBase
{
public:
    struct ANIMATION_EVENT
    {
        _uint iType;            //0: 콜라이더 활성화, 1: 콜라이더 비활성화, 2:사운드, 3:이펙트
        _float fPlayPosition;
        _uint iBoneIndex;
        string strChannelName;
    };

    struct COLLIDER_STATE
    {   
        _int iActionType;       // ATTACK, HIT
        _int iPartType;         //HAND_A, FOOT_A, JOINT_A, HEAD_A, HEAD_H = 10, BODY_H, LEG_H
        _bool isAlways;
        _uint iType;             //(AABB, OBB, Sphere)
        _float3 vCenter;
        _float3 vValue;        // Sphere라면 x에만 값을 저장하자.
    };


    struct ANIMATION_RIMLIGHTSTATE
    {
        _uint iType;					//0번이 on 1번이 off
        _float fAinmPosition;
        string strMeshName;
    };

    struct ANIMATION_TRAILSTATE
    {
        _uint iType;					//0번이 on 1번이 off
        _float fAinmPosition;
        string strBonelName;
        string strTrailProtoName;
        _uint iBoneIndex;
    };

    struct ANIMATION_FACEEVENTSTATE
    {
        _uint iType;					//0번이 on 1번이 off 2번이 change
        _float fAinmPosition;
        _uint iFaceAnimIndex;
    };


    struct ANIMATION_BLOODEVENTSTATE
    {
        _float fAinmPosition;
        _uint iBoneIndex;
        string strBonelName;
        _uint iBloodEffectType;		// 출력시킬 이펙트의 타입 (0: 코, 1: 입, 2: 미정) / 2 이후로 알아서 정의해서 맞춰서 찍으면됨
        _bool isLoop;				    // 이펙트 루프여부
        _bool isOn;				    // 이펙트 On/Off
        _bool isPlayed = { false };				    // 이펙트 실행했었는지

    };

    struct ANIMATION_RADIALEVENTSTATE
    {
        _uint iType;				//0번이 on 1번이 off
        _float fAinmPosition;
        _float fForce;
    };

    struct ANIMATION_SOUNDEVENTSTATE
    {
        _uint iChannel;
        _float fAinmPosition;
        _float fSoundVolume;
        string strSoundFileName;
        _bool isPlayed = { false };
    };


private:
    CCharacterData();
    virtual ~CCharacterData() = default;

public:
    HRESULT Initialize(class CLandObject* pCharacter);

public:
    const vector<_uint>& Get_AlphaMeshes() const {
        return m_AlphaMeshes;
    }
    const vector<_uint>& Get_LoopAnimations() const {
        return m_LoopAnimations;
    }
    const multimap<string, ANIMATION_EVENT>& Get_AnimationEvents() const {
        return m_AnimationEvents;
    }
    const unordered_map<_uint, COLLIDER_STATE>& Get_Colliders() const {
        return m_Colliders;
    }

    const multimap<string, wstring>& Get_Effets() const {
        return m_Effects;
    }

    const multimap<string, ANIMATION_TRAILSTATE>& Get_TrailEvents() const {
        return m_TrailEvents;
    }

    const multimap<string, ANIMATION_BLOODEVENTSTATE>& Get_BloodEffectEvents() const {
        return m_BloodEvents;
    }

    const multimap<string, ANIMATION_RADIALEVENTSTATE>& Get_RadialEvents() const {
        return m_RadialEvents;
    }

    const multimap<string, ANIMATION_SOUNDEVENTSTATE>& Get_SoundEvents() const {
        return m_SoundEvents;
    }

    const vector<ANIMATION_EVENT>& Get_CurrentEvents() const {
        return m_CurrentEvents;
    }

    const vector<ANIMATION_RIMLIGHTSTATE>& Get_Current_Rim_Events() const {
        return m_CurrentRimEvents;
    }

    const vector<ANIMATION_TRAILSTATE>& Get_Current_Trail_Events() const {
        return m_CurrentTrailEvents;
    }

    const vector<ANIMATION_FACEEVENTSTATE>& Get_Current_FaceEvents() const {
        return m_CurrentFaceEvents;
    }

    vector<ANIMATION_BLOODEVENTSTATE>& Get_Current_BloodEffectEvents() {
        return m_CurrentBloodEffectEvents;
    }

    const vector<ANIMATION_RADIALEVENTSTATE>& Get_Current_RadialEvents() const {
        return m_CurrentRadialEvents;
    }

    vector<ANIMATION_SOUNDEVENTSTATE*>& Get_Current_SoundEvents() {
        return m_CurrentSoundEvents;
    }

public:
    void Set_CurrentAnimation(string strAnimName);          //현재 애니메이션에 맞는 이벤트를 설정해둔다.
    void Set_CurrentCutSceneAnimation(string strAnimName);          //현재 애니메이션에 맞는 이벤트를 설정해둔다.

private:
    HRESULT Load_AlphaMeshes(string strFilePath);
    HRESULT Load_LoopAnimations(string strFilePath);
    HRESULT Load_AnimationEvents(string strFilePath);
    HRESULT Load_Colliders(string strFilePath);
    HRESULT Load_EffectState(string strFilePath);
    HRESULT Load_RimLightEvent(string strFilePath);
    HRESULT Load_TrailEvent(string strFilePath);
    HRESULT Load_FaceEvent(string strFilePath);
    HRESULT Load_BloodEffectEvent(string strFilePath);
    HRESULT Load_RadialEvent(string strFilePath);
    HRESULT Load_SoundEvent(string strFilePath);
    HRESULT Load_CutSceneSoundEvent(string strFilePath);

private:
    CGameInstance* m_pGameInstance = { nullptr };
    class CLandObject* m_pCharacter = { nullptr };

    // 모델 전체에서의 각기 인덱스
    vector<_uint> m_AlphaMeshes;
    vector<_uint> m_LoopAnimations;

    string m_iCurrentAnimName = "";

    // first: 애니메이션 이름
    multimap<string, ANIMATION_EVENT>               m_AnimationEvents;            
    // first: 뼈인덱스
    unordered_map<_uint, COLLIDER_STATE>            m_Colliders;
    // first 뼈 이름, second 이펙트 파일 이름 (.dat 제거한 이름)
    multimap<string, wstring>                       m_Effects;
       
    // first: 애니메이션 이름, second: 림라이트 이벤트정보
    multimap<string, ANIMATION_RIMLIGHTSTATE>	    m_RimLightEvents;

    // first: 애니메이션 이름, second: 트레일 이벤트정보
    multimap<string, ANIMATION_TRAILSTATE>		    m_TrailEvents;

    // first: 애니메이션 이름, second: 래디얼 이벤트 정보
    multimap<string, ANIMATION_FACEEVENTSTATE>		    m_FaceEvents;

    // first: 애니메이션 이름, second: 래디얼 이벤트 정보
    multimap<string, ANIMATION_BLOODEVENTSTATE>		    m_BloodEvents;

    // first: 애니메이션 이름, second: 래디얼 이벤트 정보
    multimap<string, ANIMATION_RADIALEVENTSTATE>		m_RadialEvents;

    // first: 애니메이션 이름, second: 래디얼 이벤트 정보
    multimap<string, ANIMATION_SOUNDEVENTSTATE>		    m_SoundEvents;

    // first: 애니메이션 이름, second: 래디얼 이벤트 정보
    multimap<string, ANIMATION_SOUNDEVENTSTATE>		    m_CutSceneSoundEvents;


    vector<ANIMATION_EVENT> m_CurrentEvents; 
    vector<ANIMATION_RIMLIGHTSTATE> m_CurrentRimEvents;
    vector<ANIMATION_TRAILSTATE> m_CurrentTrailEvents;
    vector<ANIMATION_FACEEVENTSTATE> m_CurrentFaceEvents;
    vector<ANIMATION_BLOODEVENTSTATE> m_CurrentBloodEffectEvents;
    vector<ANIMATION_RADIALEVENTSTATE> m_CurrentRadialEvents;
    vector<ANIMATION_SOUNDEVENTSTATE*> m_CurrentSoundEvents;

public:
    static CCharacterData* Create(class CLandObject* pCharacter);
    virtual void Free() override;
};
END
