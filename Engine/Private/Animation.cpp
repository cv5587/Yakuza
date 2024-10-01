#include "Animation.h"

#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration{ rhs.m_Duration }
	, m_TickPerSecond{ rhs.m_TickPerSecond }
	, m_CurrentPosition{ rhs.m_CurrentPosition }
	, m_isFinished{ rhs.m_isFinished }
	, m_iNumChannels{ rhs.m_iNumChannels }
	, m_CurrentKeyFrameIndices{ rhs.m_CurrentKeyFrameIndices }
	, m_Channels{ rhs.m_Channels }
	, m_CurrentChangePosition{rhs.m_CurrentChangePosition }
	, m_isRestart{rhs.m_isRestart }
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

const _float3* CAnimation::Get_LastKeyframe_Position(string strChannelName) const
{
	for (auto& pChannel : m_Channels)
	{
		if(pChannel->Get_Name() == strChannelName)
			return &pChannel->Get_Last_KeyFrame().vPosition;
	}

	return nullptr;
}

const _float3* CAnimation::Get_FirstKeyframe_Position(string strChannelName) const
{
	for (auto& pChannel : m_Channels)
	{
		if (pChannel->Get_Name() == strChannelName)
			return &pChannel->Get_First_KeyFrame().vPosition;
	}

	return nullptr;
}

const _float4* CAnimation::Get_LastKeyframe_Rotation(string strChannelName) const
{
	for (auto& pChannel : m_Channels)
	{
		if (pChannel->Get_Name() == strChannelName)
			return &pChannel->Get_Last_KeyFrame().vRotation;
	}

	return nullptr;
}

const _float4* CAnimation::Get_FirstKeyframe_Rotation(string strChannelName) const
{
	for (auto& pChannel : m_Channels)
	{
		if (pChannel->Get_Name() == strChannelName)
			return &pChannel->Get_First_KeyFrame().vRotation;
	}

	return nullptr;
}

HRESULT CAnimation::Initialize(const aiAnimation* pAnimation, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAnimation->mName.data);

	m_Duration = pAnimation->mDuration;
	m_TickPerSecond = pAnimation->mTicksPerSecond;

	m_iNumChannels = pAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	XMStoreFloat3(&m_vCenterMoveValue, XMVectorZero());
	XMStoreFloat4(&m_vCenterRotationValue, XMVectorZero());
	
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAnimation->mChannels[i], Bones);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}


	return S_OK;
}

HRESULT CAnimation::Initialize(const BAiAnimation* pAnimation, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAnimation->mName);

	m_Duration = pAnimation->mDuration;
	m_TickPerSecond = pAnimation->mTicksPerSecond;

	m_iNumChannels = pAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	XMStoreFloat3(&m_vCenterMoveValue, XMVectorZero());
	XMStoreFloat4(&m_vCenterRotationValue, XMVectorZero());

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAnimation->mChannels[i], Bones);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _bool isRoot, string strExcludeBoneName)
{
	m_CurrentPosition += m_TickPerSecond * fTimeDelta;
	m_isRestart = false;

	if (m_CurrentPosition >= m_Duration && !m_isFinished)
	{
		m_CurrentPosition = 0.0;
		m_isRestart = true;

		if(false == isLoop)
			m_isFinished = true;
	}

	if (false == m_isFinished)
	{
		_uint		iChannelIndex = { 0 };

		for (auto& pChannel : m_Channels)
		{
			if(Bones[pChannel->Get_BoneIndex()]->Get_Separation() == -1)
				pChannel->Update_TransformationMatrix(m_CurrentPosition, Bones, &m_CurrentKeyFrameIndices[iChannelIndex], &m_vCenterMoveValue, &m_vCenterRotationValue, isRoot, strExcludeBoneName);

			iChannelIndex++;
		}
	}
}

void CAnimation::Update_TransformationMatrix_Separation(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _int iAnimType)
{
	m_CurrentPosition += m_TickPerSecond * fTimeDelta;
	m_isRestart = false;

	if (m_CurrentPosition >= m_Duration && !m_isFinished)
	{
		m_CurrentPosition = 0.0;
		m_isRestart = true;

		if (false == isLoop)
			m_isFinished = true;
	}

	_uint		iChannelIndex = { 0 };

	for (auto& pChannel : m_Channels)
	{
		if (Bones[pChannel->Get_BoneIndex()]->Get_Separation() == iAnimType)
			pChannel->Update_TransformationMatrix(m_CurrentPosition, Bones, &m_CurrentKeyFrameIndices[iChannelIndex], &m_vCenterMoveValue, &m_vCenterRotationValue);

		iChannelIndex++;
	}
}

// 새로 입력받은 애니메이션에서 실행시킬 것이다
void CAnimation::Update_Change_Animation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pPrevAnimation, _double ChangeInterval, _bool isRoot)
{
	m_CurrentChangePosition += m_TickPerSecond * fTimeDelta;

	if (ChangeInterval <= m_CurrentChangePosition)
	{
		m_isChanged = true;
		XMStoreFloat3(&m_vCenterMoveValue, XMVectorZero());
		XMStoreFloat4(&m_vCenterRotationValue, XMVectorZero());
		return;
	}

	_uint		iChannelIndex = { 0 };

	for(auto& pDstChannel : m_Channels)
	{
		//pSrcChannel가 기존에 실행하고 있던 애니메이션이다
		for (auto& pSrcChannel : pPrevAnimation->m_Channels)
		{
			// 새로 실행할 애니메이션과 기존에 실행하고 있던 애니메이션의 뼈 인덱스가 같은 채널이 있다면
			// 선형보간이 필요한 상황이다.
			if (pDstChannel->Get_BoneIndex() == pSrcChannel->Get_BoneIndex())
			{
				// 새로 실행할 애님의 채널에서 부모를 꺼내와서 
				if (Bones[pDstChannel->Get_BoneIndex()]->Get_Separation() == -1)
					pDstChannel->Update_TransformationMatrix(m_CurrentChangePosition, Bones, pSrcChannel, pPrevAnimation->m_CurrentKeyFrameIndices[iChannelIndex], ChangeInterval, pPrevAnimation->Get_Finished(), &m_vCenterMoveValue, &m_vCenterRotationValue, isRoot);

				iChannelIndex++;
			}
		}
	}
}

void CAnimation::Update_Change_Animation_Separation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pPrevAnimation, _double ChangeInterval, _int iAnimType)
{
	m_CurrentChangePosition += m_TickPerSecond * fTimeDelta;

	if (ChangeInterval <= m_CurrentChangePosition)
	{
		m_isChanged = true;
		XMStoreFloat3(&m_vCenterMoveValue, XMVectorZero());
		XMStoreFloat4(&m_vCenterRotationValue, XMVectorZero());
		return;
	}

	_uint		iChannelIndex = { 0 };

	for (auto& pDstChannel : m_Channels)
	{
		//pSrcChannel가 기존에 실행하고 있던 애니메이션이다
		for (auto& pSrcChannel : pPrevAnimation->m_Channels)
		{
			// 새로 실행할 애니메이션과 기존에 실행하고 있던 애니메이션의 뼈 인덱스가 같은 채널이 있다면
			// 선형보간이 필요한 상황이다.
			if (pDstChannel->Get_BoneIndex() == pSrcChannel->Get_BoneIndex())
			{
				// 새로 실행할 애님의 채널에서 부모를 꺼내와서 
				if (Bones[pDstChannel->Get_BoneIndex()]->Get_Separation() == iAnimType)
					pDstChannel->Update_TransformationMatrix(m_CurrentChangePosition, Bones, pSrcChannel, pPrevAnimation->m_CurrentKeyFrameIndices[iChannelIndex], ChangeInterval, pPrevAnimation->Get_Finished(), &m_vCenterMoveValue, &m_vCenterRotationValue);
				iChannelIndex++;
			}
		}
	}
}

void CAnimation::Reset()
{
	m_CurrentPosition = 0.0;
	m_isFinished = false;
	m_CurrentChangePosition = 0.0;
	m_isChanged = false;

	XMStoreFloat3(&m_vCenterMoveValue, XMVectorZero());
	XMStoreFloat4(&m_vCenterRotationValue, XMVectorZero());

	ZeroMemory(&m_CurrentKeyFrameIndices.front(), sizeof(_uint) * m_iNumChannels);
}

void CAnimation::Update_KeyframeIndex()
{
	_uint		iChannelIndex = { 0 };

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_KeyFrame(m_CurrentPosition, &m_CurrentKeyFrameIndices[iChannelIndex++]);	
	}
}

CAnimation* CAnimation::Create(const aiAnimation* pAnimation, const vector<CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimation, Bones)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(const BAiAnimation* pAnimation, const vector<class CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimation, Bones)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
