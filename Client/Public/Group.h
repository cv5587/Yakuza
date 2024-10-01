#pragma once
#include "UI_Object.h"

#ifdef _TOOL
#include "UITool_Defines.h"
#else
#include "Client_Defines.h"
#endif // _TOOL

BEGIN(Engine)
class CVIBuffer_Rect;
END

#ifdef _TOOL
BEGIN(UITool)
#else
BEGIN(Client)
#endif
class CGroup :
    public CUI_Object
{
private:
    CGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGroup(const CGroup& rhs);
    virtual ~CGroup() = default;

public:
    void Set_PartObject(CUI_Object* pObject) { m_PartObjects.emplace_back(pObject); }
    virtual void Set_isPlay(_bool isPlay)override;
public:
    vector<CUI_Object*> Get_PartObjects() { return m_PartObjects; }
    vector<CUI_Object*>* Get_pPartObjects() { return &m_PartObjects; }
    CUI_Object* Get_PartObject(_uint iIndex) { return m_PartObjects[iIndex]; }

    void Remove_PartObject(_uint iIndex);

public:
    virtual HRESULT Initialize_Prototype() override;
     HRESULT Initialize_Prototype(ifstream& in) ;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Show_Choice(_int Index);
    void Show_Off_All();                        //전체끄기
    void Show_On_All();                        //전체켜기
    void Show_On(_int Index);
    void Show_Off(_int Index);

private:
    vector<CUI_Object*> m_PartObjects;
    _float4x4 m_GroupWorld;
public:
    virtual HRESULT Save_binary(const string strDirectory)override;
    virtual HRESULT Save_Groupbinary(ofstream& out)override;
public:
    HRESULT Load_Groupbinary(ifstream& in);

public:
    virtual HRESULT Show_UI()override;
    virtual HRESULT Close_UI()override;
    virtual _bool Check_AnimFin() override;
    virtual _bool Click_Intersect(_int Index = 0) override;
public:
    static CGroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    static CGroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
