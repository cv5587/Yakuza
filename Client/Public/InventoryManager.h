#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)
class CInventoryManager :
    public CBase
{
private:
    CInventoryManager();
    CInventoryManager(const CInventoryManager& rhs);
    virtual ~CInventoryManager() = default;

public:
     HRESULT Initialize() ;
     HRESULT Tick(const _float& fTimeDelta) ;
     HRESULT Late_Tick(const _float& fTimeDelta) ;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    vector<class CItemData*> m_Items;

public:
    static CInventoryManager* Create();
    virtual void Free();
};
END