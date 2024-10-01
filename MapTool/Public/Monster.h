#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CAnim;
END

BEGIN(Client)
class CMonster abstract :
    public CLandObject
{
public:
    enum MONSTER_STATE
    {
        IDLE,
        SHIFT_F,
        SHIFT_L,
        SHIFT_R,
        SHIFT_B,
        SHIFT_FR,
        SHIFT_FL,
        SHIFT_BR,
        SHIFT_BL,
        CMD_1,
        CMD_2,
        CMD_3,
        CMD_4,
        CMD_5,
        PUNCH,
        ANGRY_START,
        ANGRY_CHOP,
        ANGRY_KICK,
        DEATH,
        MONSTER_STATE_END
    };

protected:
    CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMonster(const CMonster& rhs);
    virtual ~CMonster() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CCollider* m_pColliderCom = { nullptr };
    CAnim* m_pAnimCom = { nullptr };

    class CCharacterData* m_pData = { nullptr }; // 캐릭터 데이터 파일 입출력

protected:
    _uint m_iState = { 0 };

private:
    virtual HRESULT Add_Componenets() override;
    virtual HRESULT Bind_ResourceData() override;

public:
    virtual void Free() override;
};
END
