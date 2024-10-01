#include "AI_RoadYOP.h"

#include "GameInstance.h"

#include "Sequance.h"
#include "Selector.h"
#include "LeafNode.h"

CAI_RoadYOP::CAI_RoadYOP()
	: CAI_RoadWalker{}
{
}

CAI_RoadYOP::CAI_RoadYOP(const CAI_RoadYOP& rhs)
	: CAI_RoadWalker{ rhs }
{
}

HRESULT CAI_RoadYOP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_RoadYOP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Root();

	return S_OK;
}

void CAI_RoadYOP::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	this->Execute();
}

CBTNode::NODE_STATE CAI_RoadYOP::Execute()
{
	return m_pRootNode->Execute();
}

void CAI_RoadYOP::Ready_Root()
{
	CSelector* pRoot = CSelector::Create();

#pragma region Coll
	CSequance* pCollSeq = CSequance::Create();
	// Add_children : 함수 넣어줌
	// check_coll : 함수, coll : 함수
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadYOP::Check_Coll, this)));
	pCollSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadYOP::Coll, this)));

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
	pWalkSeq->Add_Children(CLeafNode::Create(bind(&CAI_RoadYOP::Check_Walk, this)));

	CSelector* pWalkSelector = CSelector::Create();
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadYOP::Back, this)));
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadYOP::Turn, this)));
	pWalkSelector->Add_Children(CLeafNode::Create(bind(&CAI_RoadYOP::Walk, this)));
	pWalkSeq->Add_Children(pWalkSelector);
#pragma endregion


	pRoot->Add_Children(pCollSeq);
	pRoot->Add_Children(pWalkSeq);

	m_pRootNode = pRoot;
}

CAI_RoadYOP* CAI_RoadYOP::Create()
{
	CAI_RoadYOP* pInstance = new CAI_RoadYOP();

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CBTNode* CAI_RoadYOP::Clone(void* pArg)
{
	CAI_RoadYOP* pInstance = new CAI_RoadYOP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAI_RoadYOP::Free()
{
	__super::Free();
}
