#pragma once
#include "Base.h"
#include "Client_Defines.h"
BEGIN(Engine)

END

BEGIN(Client)
class CItemData :
    public CBase
{
public:
    typedef struct tItemDesc {
        wstring szItemName;
        wstring TextureName;
        _uint iHealAmount;
        _uint iStaminaAmount;
    }ITEMDESC;
private:
    CItemData();
    CItemData(const CItemData& rhs);
    virtual ~CItemData() = default;
public:
    HRESULT Initialize() ;
private:
    wstring m_szItemName = { TEXT("") };//아이템이름
    wstring m_TextureName={TEXT("")};//클론할 컴포넌트 이름
    _uint m_iHealAmount = { 0 };//힐량
    _uint m_iStaminaAmount = { 0 };//스테미나 회복량
public:
    static CItemData* Create();
    virtual void Free();
};
END