#pragma once
#include "Monster.h"

BEGIN(Client)
class CRushYakuza :
    public CMonster
{
public:
    typedef struct tMapMonsterObjDesc : public CGameObject::GAMEOBJECT_DESC
    {
        XMMATRIX		vStartPos;
        int				iLayer;
        wstring			wstrModelName;
        int				iShaderPass;
        int				iObjType;
        int				iObjPropertyType;
    }MONSTER_DESC;

private:
    CRushYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRushYakuza(const CRushYakuza& rhs);
    virtual ~CRushYakuza() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    int							Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc);

private:

    _bool m_isAnimLoop = { false };

    _float          m_fPrevSpeed = { 0.f };
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

    // MAP 배치관련
    int						m_iLayerNum;
    wstring					m_wstrModelName;
    int						m_iShaderPassNum = { 0 };
    int						m_iObjectType = { 0 };
    int						m_iObjectPropertyType = { 0 };


private:
    virtual HRESULT Add_Componenets() override;
    virtual HRESULT Bind_ResourceData() override;

private:
    void Synchronize_Root(const _float& fTimeDelta);
    void Change_Animation();

public:
    static CRushYakuza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
