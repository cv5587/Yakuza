#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIDamageMoney :
    public CUIScene
{
public:
    typedef struct tEffectTime {
        _float TimeDelta;
        _uint iMoney;
        _uint iTime;
        CUI_Object* pUI;
    }EFFECTTIME;    
protected:
    CUIDamageMoney();
    CUIDamageMoney(const CUIDamageMoney& rhs);
    virtual ~CUIDamageMoney() = default;

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

public://돈 , 배율을 매개변수로 받아서 계산해준다.
    void Add_MoneyEffect(_fmatrix World ,_uint iMoney , _uint iTime);
private:
    vector <EFFECTTIME*> m_PlayEffect;
    _float U = 0.0625f;
    _float V = 0.2f;
public:
    static CUIDamageMoney* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
