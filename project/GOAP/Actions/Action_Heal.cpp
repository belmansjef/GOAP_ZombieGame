#include "Action_Heal.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_Heal::Action_Heal()
	: BaseAction("Heal", 5)
	, m_Healed(false)
	, m_InventorySlot(2U)
{
	SetPrecondition("medkit_in_inventory", true);
	SetEffect("low_health", false);
}

bool GOAP::Action_Heal::IsDone()
{
	m_pWorldState->SetVariable("low_health", !m_Healed);
	m_pWorldState->SetVariable("medkit_in_inventory", !m_Healed);
	return m_Healed;
}

void GOAP::Action_Heal::Reset()
{
	BaseAction::Reset();
	m_Healed = false;
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_Heal::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;

	return m_pWorldState->GetVariable("medkit_in_inventory") || m_pWorldState->GetVariable("medkit_aquired");
}
#pragma warning( pop ) 

bool GOAP::Action_Heal::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Inventory_UseItem(m_InventorySlot))
	{
		m_Healed = true;
		return m_pInterface->Inventory_RemoveItem(m_InventorySlot);
	}

	return false;
}
