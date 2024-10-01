#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pBoneNode, _int iParentIndex)
{
	strcpy_s(m_szName, pBoneNode->mName.data);
	m_iParentBoneIndex = iParentIndex;

	memcpy(&m_TransformationMatrix, &pBoneNode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	
	return S_OK;
}

HRESULT CBone::Initialize(const BAiNode* pBoneNode)
{
	strcpy_s(m_szName, pBoneNode->mName);
	m_iParentBoneIndex = pBoneNode->mParentBoneIndex;

	memcpy(&m_TransformationMatrix, &pBoneNode->mTransformationMatrix, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
	}
}

CBone* CBone::Create(const aiNode* pBoneNode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pBoneNode, iParentIndex)))
	{
		MSG_BOX("Failed To Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Create(const BAiNode* pBoneNode)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pBoneNode)))
	{
		MSG_BOX("Failed To Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}
