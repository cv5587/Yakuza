#include "Bounding.h"

CBounding::CBounding()
{
}

CBounding::~CBounding()
{
}

void CBounding::Tick(_fmatrix WorldMatrix)
{
}

#ifdef _DEBUG
HRESULT CBounding::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
    return S_OK;
}
#endif // _DEBUG

void CBounding::Free()
{
}
