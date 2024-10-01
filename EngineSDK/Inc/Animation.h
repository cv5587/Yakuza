#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

	_bool Get_Changed() const {
		return m_isChanged;
	}

	_bool Get_Restrat() const {
		return m_isRestart;
	}

	const _char* Get_AnimName() const {
		return m_szName;
	}

	const _double* Get_Duration() const {
		return &m_Duration;
	}

	const _double* Get_CurrentPosition() const {
		return &m_CurrentPosition;
	}

	const _float3* Get_CenterMoveValue() const {
		return &m_vCenterMoveValue;
	}

	const _float4* Get_CenterRotationValue() const {
		return &m_vCenterRotationValue;
	}

	const vector<class CChannel*>& Get_Channels() const {
		return m_Channels;
	}

	const vector<_uint>* Get_CurrentKeyFrameIndices() const {
		return &m_CurrentKeyFrameIndices;
	}

	const _float3* Get_LastKeyframe_Position(string strChannelName) const;
	const _float3* Get_FirstKeyframe_Position(string strChannelName) const;

	const _float4* Get_LastKeyframe_Rotation(string strChannelName) const;
	const _float4* Get_FirstKeyframe_Rotation(string strChannelName) const;


public:
	void Set_CurrentPosition(_double Position)	{
		m_CurrentPosition = Position;
	}

public:
	HRESULT Initialize(const aiAnimation* pAnimation, const vector<class CBone*>& Bones);
	HRESULT Initialize(const BAiAnimation* pAnimation, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _bool isRoot = true, string strExcludeBoneName = "");
	void Update_TransformationMatrix_Separation(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _int iAnimType);
	void Update_Change_Animation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pPrevAnimation, _double ChangeInterval, _bool isRoot = true);
	void Update_Change_Animation_Separation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pPrevAnimation, _double ChangeInterval, _int iAnimType);
	void Reset();

	void Update_KeyframeIndex();

private:
	_char					m_szName[MAX_PATH] = { "" };
	_double					m_Duration = { 0.0 };
	_double					m_TickPerSecond = { 0.0 };
	_double					m_CurrentPosition = { 0.0 };
	_double					m_CurrentChangePosition = { 0.0 };
	_bool					m_isFinished = { false };
	_bool					m_isChanged = { true };
	_bool					m_isRestart = { false };

	_float3					m_vCenterMoveValue;
	_float4					m_vCenterRotationValue;

	_uint					m_iNumChannels = { 0 };
	vector<_uint>			m_CurrentKeyFrameIndices;
	vector<class CChannel*>	m_Channels;

public:
	static CAnimation* Create(const aiAnimation* pAnimation, const vector<class CBone*>& Bones);
	static CAnimation* Create(const BAiAnimation* pAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END