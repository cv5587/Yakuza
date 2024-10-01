#pragma once
#include "Base.h"

BEGIN(Engine)
class CRandomManager :
    public CBase
{
private:
    CRandomManager();
    virtual ~CRandomManager() = default;

public:
    HRESULT Initialize();

    template<typename T>
    T Get_Random(T Min, T Max);


private:
    random_device				m_RandomDevice;
    mt19937_64					m_RandomNumber;

public:
    static CRandomManager* Create();
    virtual void Free() override;
};
END

template<typename T>
inline T CRandomManager::Get_Random(T Min, T Max)
{
    m_RandomNumber = mt19937_64(m_RandomDevice());

    using Type = typename conditional<is_integral<T>::value,
        std::uniform_int_distribution<T>, std::uniform_real_distribution<T>>::type;

    Type Rand(Min, Max);

    return Rand(m_RandomNumber);
}