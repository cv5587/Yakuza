#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CModel;
class CAnim;
class CNeoShader;
class CNavigation;
class CComputeShader;
END

BEGIN(Client)
class CLandObject abstract :
    public CGameObject
{
public:
    enum ANIMATION_COMPONENT_TYPE {
        DEFAULT,
        CUTSCENE,
        ADVENTURE,
        ANIM_TYPE_END
    };

    // 분할된 애니메이션 컴포넌트 종류 (필요 시 늘어날 예정)
    enum SEPARATION_ANIMATION_COM_TYPE
    {
        FACE_ANIM, HAND_ANIM, DEFAULT_ANIM, PART_ANIMATION_COM_TYPE_END
    };

    /* 
    * 원형 이름: Prototype_Component_Anim_Hand 
    * 컴포넌트 이름: Com_Anim_Hand
    */
    // HandAnim 데이터의 인덱스를 enum으로 정의해둠
    enum HAND_ANIMATION_TYPE {
        HAND_NEUTRAL,               //[0] [hand_parts_0_neutral] 평소 편하게 펴고있는 상태
        HAND_MIDDLE,                //[1] [hand_parts_10_hand_middle] 
        HAND_MIDDLE2,               //[2] [hand_parts_11_hand_middle2]
        HAND_MIN,                   //[3] [hand_parts_12_hand_min]
        HAND_CHOP = 8,              //[8] [hand_parts_17_chop]  촙!!! 스킬 쓸 때 손 날로 치기 때문에 손을 편 상태의 모양이다
        HAND_GU,                    //[9] [hand_parts_1_gu] 주먹 쥐는거
        HAND_HAKO,                  //[10] [hand_parts_3_hako] 큰 물체 쥘 때 펼쳐진 손
        HAND_TAIKEN,                //[11] [hand_parts_4_taiken] 두꺼운 한손 물체 쥔 손
        HAND_BOU,                   //[12] [hand_parts_5_bou] 일반 봉같은거 쥔 손
        HAND_GUN = 14,              //[14] [hand_parts_7_gun] 권총 쥔 손
        HAND_TONFA,                 //[15] [hand_parts_8_tonfa] 봉같은거 쥔 손 (얇은버전)
        HAND_MAX                    //[16] [hand_parts_9_hand_max] 전부 핀 손
    };

    //이거 ui에서 알려고 public으로 변환시킴
public: //오브젝트들의 정보를 저장 
    typedef struct tLandObjectInfo {
        _float fMaxHP;
        _float fHp;
    } LAND_OBJ_INFO;

    typedef struct tMapLandObj : public CGameObject::GAMEOBJECT_DESC
    {
        XMMATRIX		vStartPos;
        wstring			wstrModelName;
        int             iObjectType;
        int				iShaderPass;
        int             iNaviNum;
        int             iNaviRouteNum;
        int             iNPCDirection;

        int             iGroupMonster;
        int             iGroupNum;
    }LANDOBJ_MAPDESC;

protected:
    CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLandObject(const CLandObject& rhs);
    virtual ~CLandObject() = default;

public:
    LAND_OBJ_INFO Get_Info() { return m_Info; } //객체의 정보를 저장하는 구조체 반환

#ifdef _DEBUG
public:
    void Set_ObjectRender(_bool isRender) { m_isObjectRender = isRender; }
    
public:
    _bool Get_ObjectRender() { return m_isObjectRender; }
#endif // _DEBUG

    /* Default Virtual Funtion */
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Render_LightDepth() override;
    virtual HRESULT Render_BoneCompute() override;

    /* 충돌관련 함수들 */
    virtual void ImpulseResolution(CGameObject* pTargetObject, _float fDistance = 0.5f) override;
    virtual void Attack_Event(CGameObject* pHitObject, _bool isItem = false) override {};
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) {};

    /* Virtual Funtion */
protected:
    virtual HRESULT Add_CharacterData();
    virtual void Apply_ChracterData();

    virtual void Animation_Event();
    virtual void RimLight_Event();
    virtual void Trail_Event();
    virtual void Sound_Event();

    virtual _bool Checked_Animation_Ratio(_float fRatio);

public:
    // iHandType: 0양손, 1 왼손, 2 오른손
    virtual void On_Separation_Hand(_uint iHandType = 0);           // 뼈 이름이 다르면 재정의해서 사용해야한다
    virtual void Off_Separation_Hand(_uint iHandType = 0);
    virtual void On_Separation_Face();
    virtual void Off_Separation_Face();

    // iAnimType이 -1이라면 분리해제, 그 외의 값이라면 애니메이션 컴포넌트 인덱스와 맞춰줘야한다.
    // isExceptParent 이 true라면 부모뼈만 분리 제외한다.
    void Separation_Bone(string strBoneName, _int iAnimType = -1, _bool isExceptParent = true);
    void Separation_SingleBone(string strBoneName, _int iAnimType = -1);

    /* Fublic Virtual Funtion */
public:
    virtual string Get_CurrentAnimationName() = 0;

    virtual void Off_Attack_Colliders();                               // 어택 콜라이더만 전부 끄는 기능
    virtual void Off_Trails();                               // 어택 콜라이더만 전부 끄는 기능

    /* Getter */
public:
    const wstring& Get_ModelName() {
        return m_wstrModelName;
    }
    
    const wstring& Get_ScndModelName() {
        return m_wstrScndModelName;
    }

    const unordered_map<_uint, class CSocketCollider*>& Get_Colliders() {
        return m_pColliders;
    }

    _float Get_DamageAmplify() {
        return m_fDamageAmplify;
    }

    /* Setter */
public:
    void Set_RimMeshName(string strMeshName) {
        m_strRimMeshName = strMeshName;
    }

    void Set_DamageAmplify(_float fAmplify) {
        m_fDamageAmplify = fAmplify;
    }

    void    Set_FaceAnimIndex(_uint iFaceAnimIndex) { 
        m_iFaceAnimIndex = iFaceAnimIndex; 
    }

    void    Set_HandAnimIndex(_uint iHandAnimIndex) { 
        m_iHandAnimIndex = iHandAnimIndex;
    }

    // 맵 용 set
    void    Set_StartPos(XMMATRIX    vStartPos) { m_pTransformCom->Set_WorldMatrix(vStartPos); }
    void    Set_NavigationIndex(int iIndex);
    void    Set_NaviRouteIndex(int iIndex) { m_iNaviRouteNum = iIndex; }

#ifdef _DEBUG
public:
    //uv 체크 디버그용
    _float2 Get_TopUV() { return m_fRimTopUV; }
    _float2 Get_BotUV() { return m_fRimBotUV; }
    _float2 Get_PartUV() { return m_fRimPartsUV; }

    void Set_TopUV(_float2 UV) { m_fRimTopUV = UV; }
    void Set_BotUV(_float2 UV) { m_fRimBotUV = UV; }
    void Set_PartUV(_float2 UV) { m_fRimPartsUV = UV; }
#endif // DEBUG

protected:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CCollision_Manager* m_pCollisionManager = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CComputeShader* m_pComputeShaderCom = { nullptr };
    CNeoShader* m_pMaterialCom = { nullptr };
    CNavigation* m_pNavigationCom = { nullptr };

    // 애니메이션 툴에서 찍은 데이터 파싱해서 들고있는 객체
    class CCharacterData* m_pData = { nullptr };

protected:
    //객체의 정보 구조체 
    LAND_OBJ_INFO m_Info = {};

    // 사용할 모델의 이름 (객체 생성 이후 바뀌지 않는다)
    wstring m_wstrModelName = TEXT("");
    wstring m_wstrScndModelName = TEXT("");

    // 림라이트를 적용해줄 메시의 이름 (필요할 때마다 바뀌는 값)
    string m_strRimMeshName = "";

    // 데미지 증감 값 (기본은 1배, 스킬 사용 시 마다 변경해준다.)
    _float m_fDamageAmplify = { 1.f };

protected:
    // first: 뼈 인덱스, secone: 소켓 콜라이더
    // 파싱된 콜라이더 객체를 저장할 배열
    unordered_map<_uint, class CSocketCollider*>      m_pColliders;

    //뼈 이름, 소켓 이펙트
    multimap<string, class CSocketEffect*>      m_pEffects;
    map<string, class CSocketEffect*>           m_pTrailEffects;
    map<string, class CSocketEffect*>           m_pBloodEffects;

    // 분할한 애니메이션을 실행할 애님 컴포넌트를 들고있을 배열
    vector<CAnim*>        m_SeparationAnimComs;
    _uint           m_iHandAnimIndex = HAND_MIDDLE;
    _uint           m_iFaceAnimIndex = { 0 };

    _bool           m_isMonster = { true };        // 데이터 파싱 시 필요한 정보


#ifdef _DEBUG
    _bool m_isObjectRender = { true };
#endif // _DEBUG

    // 림라이트 관련 정보
protected:
    _float m_isRimLight ;//
    _float2 m_fRimArmUV = { 0.5f , 0.7f };//0~1사이 [시작v,끝v]상의어께
    _float2 m_fRimTopUV = { 0.0f , 0.3f };//0~1사이 [시작v,끝v]상의

    _float2 m_fRimBotUV = { 0.9f, 1.0f };//0~1사이 [시작v,끝v]하의
    _float2 m_fRimPartsUV = { 0.0f, 1.0f };//0~1사이 [시작v,끝v]손,발


protected:
        int             m_iNaviRouteNum = { 0 }; //루트
        int             m_iNPCDirection = { 0 };

protected:
    virtual HRESULT Add_Components();
    virtual HRESULT Bind_ResourceData();
    virtual void Compute_Height();
    string ExtractString(string strName);

public:
    virtual void Free() override;
};
END
