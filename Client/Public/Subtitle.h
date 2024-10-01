#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Client)
class CSubtitle :
    public CGameObject
{
public:
    typedef struct tSubtitleDesc {
        _uint iScriptChapter;
    }SUBTITLE_DESC;

private:
    CSubtitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSubtitle(const CSubtitle& rhs);
    virtual ~CSubtitle() = default;

public:
    _bool isFinished() { return m_isFinished; }
    
public:
    void Start_Subscript() { m_isStart = true;  }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;
    
private:
    class CScriptManager* m_pScriptManager = { nullptr };

private:
    wstring m_strScript = { TEXT("") };
    _float							m_fX, m_fY, m_fSizeX, m_fSizeY;
    _float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
    _uint m_iScriptIndex = { 0 };
    _uint m_iScriptChapter = { 0 };

    _float m_fDuration = { 3.f };
    _float m_fScriptTime = { 0.f };

    _bool m_isStart = { false };
    _bool m_isFinished = { false };

public:
    static CSubtitle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END

