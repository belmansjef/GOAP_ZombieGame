#include "Action_GrabFood.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_GrabFood::Action_GrabFood()
	: BaseAction("Grab Food", 5)
	, m_FoodGrabbed(false)
	, m_FirstInventorySlot(3U)
	, m_SecondInventorySlot(4U)
{
	SetPrecondition("food_aquired", true);
	SetEffect("food_in_inventory", true);
	SetEffect("food_grabbed", true);
}

bool GOAP::Action_GrabFood::IsDone()
{
	return m_FoodGrabbed;
}

void GOAP::Action_GrabFood::Reset()
{
	BaseAction::Reset();
	m_FoodGrabbed = false;
}

int GOAP::Action_GrabFood::GetCost() const
{
	return static_cast<int>(m_pTarget->Location.Distance(m_AgentInfo.Position));
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_GrabFood::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Food", m_pEntities) || m_pEntities == nullptr || m_pEntities->empty()) return false;

	m_AgentInfo = m_pInterface->Agent_GetInfo();
	m_pTarget = GetClosestEntity();
	return m_pTarget != nullptr || !m_pWorldState->GetVariable("all_houses_looted");
}
#pragma warning( pop )

bool GOAP::Action_GrabFood::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo newFood;
	if (m_pInterface->Item_GetInfo(*m_pTarget, newFood))
	{
		if (newFood.Type != eItemType::FOOD) return false;
		// If there's no food in the inventory, place it in the first food slot
		if (!m_pWorldState->GetVariable("food_in_inventory"))
		{
			m_pInterface->Item_Grab(*m_pTarget, newFood);
			m_pInterface->Inventory_AddItem(m_FirstInventorySlot, newFood);
			m_pWorldState->SetVariable("food_in_inventory", true);
		}
		// If there is food in the first slot, put it in the second
		else
		{
			// Food inventory is full, check if the new food is better
			if (m_pWorldState->GetVariable("food_inventory_full"))
			{
				ItemInfo equipedFood;
				m_pInterface->Inventory_GetItem(m_FirstInventorySlot, equipedFood);
				if (m_pInterface->Food_GetEnergy(newFood) >= m_pInterface->Food_GetEnergy(equipedFood))
				{
					m_pInterface->Inventory_UseItem(m_FirstInventorySlot);
					m_pInterface->Inventory_RemoveItem(m_FirstInventorySlot);
					m_pInterface->Item_Grab(*m_pTarget, newFood);
					m_pInterface->Inventory_AddItem(m_FirstInventorySlot, newFood);
				}
				else
				{
					m_pInterface->Inventory_GetItem(m_SecondInventorySlot, equipedFood);
					if (m_pInterface->Food_GetEnergy(newFood) >= m_pInterface->Food_GetEnergy(equipedFood))
					{
						m_pInterface->Inventory_UseItem(m_SecondInventorySlot);
						m_pInterface->Inventory_RemoveItem(m_SecondInventorySlot);
						m_pInterface->Item_Grab(*m_pTarget, newFood);
						m_pInterface->Inventory_AddItem(m_SecondInventorySlot, newFood);
					}
					else
					{
						std::cout << "[Grab Food]: Better food in inventory, destroying ground item!" << std::endl;
						m_pInterface->Item_Destroy(*m_pTarget);
					}
				}
			}
			// Food inventory is not full yet, add new food to inventory
			else
			{
				m_pInterface->Item_Grab(*m_pTarget, newFood);
				m_pInterface->Inventory_AddItem(m_SecondInventorySlot, newFood);
				m_pWorldState->SetVariable("food_inventory_full", true);
			}
		}

		// Remove item from general entity list
		std::vector<EntityInfo>* pEntities;
		pBlackboard->GetData("Entities", pEntities);
		pEntities->erase(std::find(pEntities->begin(), pEntities->end(), *m_pTarget));

		m_pEntities->erase(std::find(m_pEntities->begin(), m_pEntities->end(), *m_pTarget));
		m_pWorldState->SetVariable("item_looted", true);
		return m_FoodGrabbed = true;
	}

	// Couldn't grab item, abort action
	return false;
}
