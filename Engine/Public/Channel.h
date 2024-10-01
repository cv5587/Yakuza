#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	HRESULT Initialize(const BAiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void	Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float3* fCenterMoveValue, _float4* fCenterRotationValue, _bool isRoot = true, string strExcludeBoneName = "");
	void	Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, CChannel* pSrcChannel, _uint PrevKeyFrameIndex, _double ChangeInterval, _bool isFinished, _float3* fCenterMoveValue, _float4* fCenterRotationValue, _bool isRoot = true);
	void	Update_KeyFrame(_double CurrentPosition, _uint* pCurrentKeyFrameIndex);

public:
	const _uint& Get_BoneIndex() const { return m_iBoneIndex; }
	const KEYFRAME& Get_First_KeyFrame() const { return m_KeyFrames.front(); }
	const KEYFRAME& Get_Last_KeyFrame() const { return m_KeyFrames.back(); }
	const vector<KEYFRAME>& Get_KeyFrames() const { return m_KeyFrames; }

	const _char* Get_Name() const {
		return m_szName;
	}

private:
	_char				m_szName[MAX_PATH] = { "" };		// ª¿ ¿Ã∏ß
	_uint				m_iBoneIndex = { 0 };				// ¿¸√º ª¿¡ﬂ ¿Ã∏ß¿Ã ¿œƒ°«œ¥¬ ª¿¿« ¿Œµ¶Ω∫

	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;
	
public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	static CChannel* Create(const BAiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END