#pragma once
#include "Base.h"
BEGIN(Engine)
class CCalculator :
    public CBase
{
private:
    CCalculator();
    virtual ~CCalculator() = default;

public:
    HRESULT Initialize();

public:
    _float Lerp(_float Start, _float End, _float Factor);
    _uint Lerp(_uint Start, _uint End, _float Factor);

    _bool Find_String(const string& text, const string& pattern);

private:
    vector<_int> Compute_LPS(const std::string& pattern);

public:
    static CCalculator* Create();
    virtual void Free() override;
};

END