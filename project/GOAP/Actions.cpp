#include "../stdafx.h"
#include "Actions.h"
#include "IExamInterface.h"
#include "../SpacePartitioning.h"
#include "WorldState.h"

GOAP::Action_GotoClosestCell::Action_GotoClosestCell()
	: BaseAction("Go To Closest Cell", 10)
{
	// TODO: Add dynamic cost based on distance to closest cell
	SetEffect("has_explored", true);
}

bool GOAP::Action_GotoClosestCell::IsValid(Elite::Blackboard* pBlackboard)
{
	CellSpace worldGrid;
	if (!pBlackboard->GetData("CellSpace", worldGrid)) return false;

	EntityInfo* cellInfo = new EntityInfo();
	cellInfo->Location = worldGrid.GetNextCellExpandingSquare().center;

	m_pTarget = cellInfo;
	return true;
}
	
bool GOAP::Action_GotoClosestCell::Execute(Elite::Blackboard* pBlackboard)
{
	return true;
}

bool GOAP::Action_GotoClosestCell::IsDone()
{
	return IsInRange();
}
