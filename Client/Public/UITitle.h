#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUITitle :
    public CUIScene
{
    //EVENTUI  0 번쨰 -> 이펙트
    //EVENTUI 1 번째 ->타이틀
public:
    enum TITLE_IMG {
        
        TITLE_IMG_END
    };
protected:
    CUITitle();
    CUITitle(const CUITitle& rhs);
    virtual ~CUITitle() = default;

public:
    _bool isStart() { return m_isStart; }
    _bool isEnd() { return m_isEnd; }
    _bool isBattleStart() { return m_isBattleStart; }

public:
    void Set_Start(_bool isStart) { m_isStart = isStart; }

public:
    virtual HRESULT Show_Scene()override;//ui 애님 준비(초기화/열때 정방향 진행)
    virtual HRESULT Close_Scene()override;//ui 애님 준비(초기화/닫을떄 반대로진행)

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

public:
    //띄울 이미지 선택하는 함수
    void Start_Title(_uint Index);

private:
    _float m_iCurrentTime = { 0.f };
    _bool m_isStart = false;
    _bool m_isEnd = { false };
    _bool m_isBattleStart = { false };

public:
    static CUITitle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
