#pragma once
#include "Base.h"

BEGIN(Engine)
class CBTNode_Manager :
    public CBase
{
private:
    CBTNode_Manager();
    virtual ~CBTNode_Manager() = default;

public:
    HRESULT Initialize(_uint iMaxLevelIndex);

public:
    HRESULT Add_BTNode_Prototype(_uint iLevelIndex, const wstring strBTNodeTag, class CBTNode* pBTNode);
    class CBTNode* Add_BTNode(_uint iLevelIndex, const wstring strBTNodeTag, void* pArg);
    void Clear(_uint iLevelIndex);

private:
    map<const wstring, class CBTNode*>* m_pPrototypes;

    _uint m_iMaxLevelIndex = { 0 };

private:
    class CBTNode* Find_BTNode_Prototype(_uint iLevelIndex, const wstring strBTNodeTag);

public:
    static CBTNode_Manager* Create(_uint iMaxLevelIndex);
    virtual void Free();
};
END
