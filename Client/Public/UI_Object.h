#pragma once
#include "GameObject.h"

#ifdef _TOOL
#include "UITool_Defines.h"
#else
#include "Client_Defines.h"
#endif // _TOOL

#ifdef _TOOL
BEGIN(UITool)
#else
BEGIN(Client)
#endif
class CUI_Object abstract :
    public CGameObject
{
public:
    typedef struct tUIObjectDesc : public CGameObject::GAMEOBJECT_DESC {
        string strName;
        _uint iTypeIndex;
        _bool isLoad = { false };
        _bool isEvent;
        _bool isScreen;
    } UI_OBJECT_DESC;

    enum TEXTURE_TYPE { TYPE_IMG, TYPE_BTN, TYPE_TEXT, TYPE_GROUP, TYPE_UIEFFECT, TEXTURE_TYPE_END };

protected:
    CUI_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Object(const CUI_Object& rhs);
    virtual ~CUI_Object() = default;

public:
    void Set_Name(string strName) { m_strName = strName; }
    void Set_Event(_bool isEvent) { m_isEvent = isEvent; }
    void Set_isScreen(_bool isScreen) { m_isScreen = isScreen; }
    virtual void Set_isPlay(_bool isPlay) { m_isPlay = isPlay; }
public:
    string Get_Name() { return m_strName; }
    _bool Get_Event() { return m_isEvent; }
    _bool Get_isScreen() { return m_isScreen; }
    _int Get_TypeIndex() { return m_iTypeIndex; }
    _bool Get_isPlay() { return m_isPlay; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual HRESULT Save_binary(const string strDirectory);
    virtual HRESULT Save_Groupbinary(ofstream& out);
    virtual HRESULT  Load_binary(ifstream& in);

public:
    virtual HRESULT Show_UI();
    virtual HRESULT Close_UI();
    virtual _bool Check_AnimFin();
    virtual _bool Click_Intersect(_int Index=0);
protected:
    string m_strName = { "" }; // 저장될 이름
    _uint m_iTypeIndex = { 0 }; // 오브젝트 타입
    _bool m_isFinAnim = { false };
    _bool m_isEvent = { false };
    _bool m_isScreen = { false };
    _bool m_isPlay = { true };
public:
    virtual void Free();
};
END
