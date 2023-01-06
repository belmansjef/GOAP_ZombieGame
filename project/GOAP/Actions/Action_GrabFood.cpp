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
	SetEffect("food_inventory_full", true);
	SetEffect("food_in_inventory", true);
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

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_GrabFood::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Food", m_pEntities) || m_pEntities == nullptr || m_pEntities->empty()) return false;

	AgentInfo agentInfo{ m_pInterface->Agent_GetInfo() };
	float closestDist{ FLT_MAX };
	EntityInfo* closestEntity{ nullptr };
	for (auto& entity : *m_pEntities)
	{
		const float dist{ entity.Location.DistanceSquared(agentInfo.Position) };
		if (dist < closestDist)
		{
			closestEntity = &entity;
			closestDist = dist;
		}
	}

	if (closestEntity != nullptr)
	{
		m_pTarget = closestEntity;
	}

	return m_pTarget != nullptr;
}
#pragma warning( pop )

bool GOAP::Action_GrabFood::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo item;
	if (m_pInterface->Item_Grab(*m_pTarget, item))
	{
		// If there's no food in the inventory, place it in the first food slot
		if (!m_pWorldState->GetVariable("food_in_inventory"))
		{
			if (item.Type != eItemType::FOOD) return false;
			m_pInterface->Inventory_AddItem(m_FirstInventorySlot, item);
			m_pWorldState->SetVariable("food_in_inventory", true);
		}
		// If there is food in the first slot, put it in the second
		else
		{
			if (item.Type != eItemType::FOOD) return false;
			m_pInterface->Inventory_AddItem(m_SecondInventorySlot, item);
			m_pWorldState->SetVariable("food_inventory_full", true);
		}

		// Remove item from general entity list
		std::vector<EntityInfo>* pEntities;
		pBlackboard->GetData("Entities", pEntities);
		pEntities->erase(std::find(pEntities->begin(), pEntities->end(), *m_pTarget));

		m_pEntities->erase(std::find(m_pEntities->begin(), m_pEntities->end(), *m_pTarget));
		return m_FoodGrabbed = true;
	}

	// Couldn't grab item, abort action
	return false;
}
