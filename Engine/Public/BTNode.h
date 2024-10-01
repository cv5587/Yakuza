#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CBTNode abstract : public CBase
{
public:
	enum NODE_STATE { SUCCESS, FAIL, RUNNING };

public:
	CBTNode();
	CBTNode(void* pArg);
	virtual ~CBTNode() = default;

public:
	virtual NODE_STATE Execute() = 0;

protected:
	_bool m_isClone = { false };

public:
	virtual CBTNode* Clone(void* pArg);
	virtual void Free() override;
};
END
