#include "AI_RoadNML.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

CAI_RoadNML::CAI_RoadNML()
	: CAI_RoadWalker{}
{
}

CAI_RoadNML::CAI_RoadNML(const CAI_RoadNML& rhs)
	: CAI_RoadWalker { rhs }
{
}

HRESULT CAI_RoadNML::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadNML::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Root();

	return S_OK;
}

void CAI_RoadNML::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_RoadNML::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_RoadNML::Ready_Root()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Coll
	CSequance* pCollSeq = CSequance::Create();
	// Add_children : 함수 넣어줌
	// check_coll : 함수, coll : 함수
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Check_Coll, this)));
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Coll, this)));

	//check_coll 성공하면 coll로 넘어감
	//sequence, select는 vector를 가짐, 0~마지막까지 순서대로 실행시킴
	//sequcence에 check_coll, coll을 넣어놧음
	//check_coll -> coll 넘어갓음(success)해서 그럼 coll이 만약 success되면
	//pCollSeq : CAI_RoadNML::Coll의 success를 받음
	//execute를 보기
	//pCollSeq이 success, fail, running 리턴되면
	// pCollSeq success 반환 : 

#pragma endregion

#pragma region Walk
	CSequance* pWalkSeq = CSequance::Create();
	pWalkSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Check_Walk, this)));

	CSelector* pWalkSelector = CSelector::Create();
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Back, this)));
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Turn, this)));
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadNML::Walk, this)));
	pWalkSeq->Add_Children(pWalkSelector);
#pragma endregion

	
	pRoot->Add_Children(pCollSeq);
	pRoot->Add_Children(pWalkSeq);

	m_pRootNode = pRoot;
}

CAI_RoadNML* CAI_RoadNML::Create()
{
	CAI_RoadNML* pInstance = new CAI_RoadNML();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_RoadNML::Clone(void* pArg)
{
	CAI_RoadNML* pInstance = new CAI_RoadNML(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_RoadNML::Free()
{
	__super::Free();
}
