#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUIManager :
    public CBase
{
    DECLARE_SINGLETON(CUIManager)
public:
    enum INFO{HP, SKILLGAUAGE , EAGLEEYE , CARHP , INFO_END};

    // 스킬창 용 이넘
    enum KRS_SKILL_TYPE { FLY_KICK, KICK_COMBO, PUNCH, DOUBLE_FINISH, FINISHBLOW, POWER, WALL_ATTACK, ATTACK_DEEPDOWN};
    enum KRH_SKILL_TYPE { RISE_TORNADO, SWAY_BLOW, CANCLE_WEABBING, WEABBING_TO_SWAY, RUSH_COMBO_7, RUSH_COMBO_8 };
    enum KRC_SKILL_TYPE { GUARD_RUSH, LARRIOTT, RUSH_HUG, WEAPON_RUSH, CRASH_HAMMER };

private:
    CUIManager();
    virtual ~CUIManager() = default;

#ifdef _DEBUG
public:
    void Set_Render(_bool isRender) { m_isRender = isRender; }

public:
    _bool Get_Render() { return m_isRender; }
#endif // _DEBUG

public:
    _bool isInvenClose() { return m_isClose; }
    _bool isOpen(wstring strName);

public: // 튜토리얼 용
    _bool isShowTutorialUI(_uint iUIType);
    _bool isCloseTutorialUIAnim();
    _bool isTutorialStartEnd();
    void Set_TutorialState(_uint iType);
    void Set_TutorialText(wstring strText);

public: // 카체이스 용
    HRESULT Add_Target(_uint iIndex, class CCarChase_Monster* pMonster, _bool isBullet = false);
    HRESULT Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP);
    HRESULT Remove_Target(_uint iIndex);
    class CCarChase_Monster* Get_Target();

public: //타이틀 용
    /*
    * 타이틀 목록
    0 출구로 가라
    1 약손 쾌감으로 가라
    2 마코토를 찾아라
    3 도망쳐라
    4 에가시라 히로야
    5 쿠제 다이사쿠
    6 수상한 남자
    7 수상한 남자들
    8 폭주족
    9 챔피언 거리 주민
    10 호위하라
    11 거리의 양아치
    12 야쿠자
    13 날라리
    14 니시키야마를 찾아라
    15 여자를 데리고 도망쳐라
    16 도지마조 조직원
    17 동흥 크레디트의 남자들
    18 날라리들
    */
    void Set_TitleStart(_bool isStart);
    _bool isTitleStart();
    void Start_Title(_int iIndex);
    _bool isTitleEnd();
    _bool isBattleStart();

public: //페이드인페이드아웃
    _bool isFindFinished();

public:
    //자신이 만들 ui씬은 미리 할당(수동)
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) ;

    HRESULT Add_Data(const wstring strSceneName, const wstring strProtoName);

    void Open_Scene(const wstring strSceneName);
    void Close_Scene(const wstring strSceneName = TEXT(""));

    void Click();
    HRESULT Tick(const _float& fTimeDelta);
    HRESULT Late_Tick(const _float& fTimeDelta);

    _bool IsOpend() {
        return (0 < m_PlayScene.size());
    }

    //임시 토크매니저 확인용 제작
public:
    void Start_Talk(_uint iScriptIndex);
    _int Get_CurrentPage();
    _bool isTalkFinished();
    void Change_TutorialUI(_uint iUIType);
    _bool Check_Scene(wstring SceneName);

public: // 스토리 UI
    void Close_Image();

    //카라오케 점수 전달용
public:
    void Set_Score(_uint iScore);

    //파이트 스코어 (돈)
public:
    void Set_Money(_uint Money) { m_Money = Money; }
    _uint Get_Money() { return m_Money; }
    void Set_AlwayUI(_bool isTrue) { m_isAlways = isTrue; }

    //스타일 교체용(lifeUI)
public:
    void Change_Style(_uint iStyle);

/* 스킬창 용 */
public:
    _bool* Get_Skill_Holligan();
    _bool* Get_Skill_Rush();
    _bool* Get_Skill_Destoryer();

private:

    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    map <wstring ,class CUIScene*> m_AllScene;//모든 씬데이터 보관(폴더이름 ,파일)
    vector< class CUIScene*> m_PlayScene ;//사용하는 씬만 담아 상호 작용(팝업용)창
    vector<class CUIScene*> m_AlwaysUI;//항상 떠있는 ui (피통 스킬상태 소지품)
    class CInventoryManager* m_pInventory = {nullptr};

#ifdef _DEBUG
private:
    _bool m_isRender = { true };
#endif // _DEBUG

private:
    _bool m_isClose = { true };
    wstring m_iCurScene = { TEXT("") };

private:
    _uint m_Money = { 0 };

private:
    _bool m_isAlways = { true };

public:
    void Fade_In();
    void Fade_Out();
    _bool Fade_Finished();

 public://QTE
    void PressKey();
    void Set_QTEKey(wstring strKey);
    _bool QTE_Finished();

public://DamgeEffecdt
    void Add_MoneyEffect(_fmatrix world, _uint iMoney, _uint iTime);

public:
   CUIScene* Find_Scene(wstring strSceneName);      

public:
    virtual void Free();
};
END
