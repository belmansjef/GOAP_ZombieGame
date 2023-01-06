#include "Action_Eat.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_Eat::Action_Eat()
	: BaseAction("Eat", 5)
	, m_Ate(false)
	, m_FirstInventorySlot(3U)
	, m_SecondInventorySlot(4U)
{
	SetPrecondition("food_in_inventory", true);
	SetEffect("low_energy", false);
}

bool GOAP::Action_Eat::IsDone()
{
	return m_Ate;
}

void GOAP::Action_Eat::Reset()
{
	BaseAction::Reset();
	m_Ate = false;
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_Eat::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;

	return m_pWorldState->GetVariable("food_in_inventory") || m_pWorldState->GetVariable("food_aquired");
}
#pragma warning( pop ) 

bool GOAP::Action_Eat::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pWorldState->GetVariable("food_inventory_full"))
	{
		if(!m_pInterface->Inventory_UseItem(m_SecondInventorySlot)) return false;
		m_pWorldState->SetVariable("food_inventory_full", false);
		m_Ate = true;
		return m_pInterface->Inventory_RemoveItem(m_SecondInventorySlot);
	}
	else if (m_pWorldState->GetVariable("food_in_inventory"))
	{
		if (!m_pInterface->Inventory_UseItem(m_FirstInventorySlot)) return false;
		m_pWorldState->SetVariable("food_in_inventory", false);
		m_Ate = true;
		return m_pInterface->Inventory_RemoveItem(m_FirstInventorySlot);
	}

	return false;
}
