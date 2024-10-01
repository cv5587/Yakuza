#pragma once
#include "UI_Texture.h" 

#ifdef _TOOL
BEGIN(UITool)
#else
BEGIN(Client)
#endif // _TOOL
class CUI_Effect :
    public CUI_Texture
{
public:
    typedef struct tUIEffectDesc :public UI_TEXTURE_DESC {
        _float3 vLifeTime;
        _float  fSpeed;
        _bool isUVAnim;

    }UI_EFFECT_DESC;

private:
    CUI_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Effect(const CUI_Effect& rhs);
    virtual ~CUI_Effect() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
     HRESULT Initialize_Prototype(ifstream& in) ;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
    void Set_LifeTime(_float3 vLifeTime) { m_vLifeTime = vLifeTime; }
    void Set_isUVAnim(_bool isUVAnim) { m_isUVAnim = isUVAnim; }

    _float Get_Speed() { return m_fSpeed; }
    _float3 Get_LifeTime() { return m_vLifeTime; }
    _bool Get_isUVAnim() { return m_isUVAnim; }

private:
    //이펙트시가 시작되면 시작시간 부터 끝시간까지만 렌더 되는 구조
    //끝나면 자동소멸
    //
    _float3 m_vLifeTime = { 0.f, 0.f, 1.f };//현재 시간 ,시작시간, 종료시간
    _float m_fSpeed = { 1.f };//이펙트 진행 속도
    _bool m_isUVAnim = { false };
private:
    virtual HRESULT Bind_ResourceData() override;
public:
    virtual HRESULT Save_binary(const string strDirectory)override;
    virtual HRESULT Save_Groupbinary(ofstream& out)override;
    virtual HRESULT Load_binary(ifstream& in)override;
public:
    static CUI_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    static CUI_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
