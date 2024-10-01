#pragma once
#include "GameObject.h"

// 엔진 뭐 필요한게 없는듯?
BEGIN(Engine)
class CAnim;
END

BEGIN(Client)
class CQteManager :
    public CGameObject
{
public:
    enum QTE_TYPE {
        YONEDA, CARCHASE, QTE_TYPE_END
    };

    struct QTE_DESC {
        _uint iAnimIndex;
        _uint iCamAnimIndex;
        _uint iStartKeyFrameIndex;
        _uint iEndKeyFrameIndex;
        _uint iKeyFrames;
        _uint iSuccesStartFrame;
        _uint iSuccesEndFrame;
        _uint iFailedStartFrame;
        _uint iFailedEndFrame;
    };

    const _float QTE_TIME = 2.f;
    const _float QTE_TIME_SPEED = 0.3f;
    const _float QTE_TIME_SPEED_MAX = 0.7f;

private:
    CQteManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CQteManager(const CQteManager& rhs);
    virtual ~CQteManager() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_Animation(CAnim* pAnim, string strAnimName);
    void Set_Kuze(class CMonster* pMonster);

private:
    HRESULT Ready_SlowKeyFrame();

    void ResetVariables();

    void Skip_KeyFrame(QTE_DESC& Desc);
    void Cancle_KeyFrame();

public:
    void Slowing();
    _bool Check_QTE();
    void Check_QTE_Section();

private:
    CAnim* m_pPlayerAnimCom = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };

    _float m_fSlowSpeed = { QTE_TIME_SPEED_MAX };

    _float  m_fOutTime = { 0.f };
    _float  m_fPassTime = { 0.f };
    _bool   m_isSlowing = { false };

    _uint   m_iSuccess = { 0 };                     // 0: 암것도아님, 1: 성공, 2: 실패

    string m_strAnimationName;
    string m_strPlayingAnimName;

    _bool m_isKuze = { false };
    _uint m_iQteCount = { 0 };

    unordered_map<string, QTE_DESC> m_QTEs;
    QTE_DESC m_KuzeSecondQTE;

    class CMonster* m_pTargetMonster = { nullptr };

    _bool m_isQTEUI = { false };

public:
    static CQteManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END