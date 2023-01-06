#include "Action_GoToClosestCell.h"
#include "../../SpacePartitioning.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_GotoClosestCell::Action_GotoClosestCell()
	: BaseAction("Go To Closest Cell", 10)
{
	// TODO: Add dynamic cost based on distance to closest cell
	SetEffect("has_explored", true);
}

bool GOAP::Action_GotoClosestCell::IsDone()
{
	return IsInRange();
}

void GOAP::Action_GotoClosestCell::Reset()
{
	SAFE_DELETE(m_pTarget);
	BaseAction::Reset();
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_GotoClosestCell::IsValid(Elite::Blackboard* pBlackboard)
{
	CellSpace worldGrid;
	if (!pBlackboard->GetData("CellSpace", worldGrid)) return false;
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;

	EntityInfo* target = new EntityInfo();
	target->Location = worldGrid.GetClosestCellOnPath(m_pInterface->Agent_GetInfo().Position).center;
	m_pTarget = target;
	
	return m_pTarget != nullptr;
}
#pragma warning( pop ) 

bool GOAP::Action_GotoClosestCell::Execute(Elite::Blackboard* pBlackboard)
{
	return true;
}