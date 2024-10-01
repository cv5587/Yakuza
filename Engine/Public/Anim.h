#pragma once
#include "Component.h"
#include "Engine_Defines.h"

/*
캐릭터 애니메이션를 모아두는 컴포넌트
모델 play 함수에서 해당 컴포넌트를 받아서 사용할 수 있도록 한다.
*/
BEGIN(Engine)
class ENGINE_DLL CAnim :
    public CComponent
{
private:
    CAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAnim(const CAnim& rhs);
    virtual ~CAnim() = default;

public:
    vector<class CAnimation*> Get_Animations() { return m_Animations; }
    _uint Get_NumAnimation() { return m_iAnimations; }
    _uint Get_AnimationIndex(const _char* pName);
    string Get_AnimationName(_uint iIndex);
    class CAnimation* Get_CurrentAnimation();

public:
    void Set_CurrentAnimIndex(_uint iIndex) { m_iCurrentIndex = iIndex; }
    void Set_PrevAnimIndex(_uint iIndex) { m_iPrevIndex = iIndex; }
    
public: // 애니메이션 관련
    void Reset_Animation();
    void Reset_Animation(_uint iAnimIndex);
    _bool Get_AnimFinished();
    _bool Get_LoopAnimFinished();
    const _double* Get_AnimDuration();
    const _double* Get_AnimDuration(_uint iAnimIndex);
    const _double* Get_AnimPosition();
    const _double* Get_AnimPosition(_uint iAnimIndex);
    const _float4* Get_AnimationCenterRotation();
    const _float3* Get_AnimationCenterMove();
    const vector<_uint>* Get_CurrentKeyFrameIndices(string strAnimationName = "");
    const vector<_uint>* Get_CurrentKeyFrameIndices(_uint iAnimIndex);
    const _float3* Get_LastKeyframe_Position(_uint iAnimIndex, string strChannelName);
    const _float3* Get_FirstKeyframe_Position(_uint iAnimIndex, string strChannelName);
    const _float4* Get_LastKeyframe_Rotation(_uint iAnimIndex, string strChannelName);
    const _float4* Get_FirstKeyframe_Rotation(_uint iAnimIndex, string strChannelName);

    _bool Get_AnimFinished() const;
    _bool Get_AnimChanged() const;
    _bool Get_AnimRestart(_bool isLoop) const;
    _bool Get_AnimLerp(_float ChangeInterval) const;
    _uint Get_CurrentAnimIndex() { return m_iCurrentIndex; }
    _uint Get_PrevAnimIndex() { return m_iPrevIndex; }

public:
    virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, _bool isSave);
    virtual HRESULT Initialize(void* pArg);

public:
    HRESULT Export_Animation(const _char* pModelFilePath);
    HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);

private:
    const aiScene* m_pAIScene = { nullptr };
    Assimp::Importer			m_Importer;

    _uint m_iAnimations = { 0 };
    vector<class CBone*> m_Bones;
    vector<class CAnimation*> m_Animations;

    _uint m_iCurrentIndex = { 0 };
    _uint m_iPrevIndex = { 0 };

private:
    HRESULT Save_File(const _char* pModelFilePath, vector<class CBone*> Bones);
    HRESULT Load_File(const _char* pModelFilePath);
    
public:
    static CAnim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _bool isSave);
    virtual CComponent* Clone(void* pArg);
    virtual void Free();  
};
END
