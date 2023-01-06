#include "Action_GrabMedkit.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_GrabMedkit::Action_GrabMedkit()
	: BaseAction("Grab Medkit", 5)
	, m_MedkitGrabbed(false)
	, m_InventorySlot(2U)
{
	SetPrecondition("medkit_aquired", true);
	SetPrecondition("medkit_in_inventory", false);
	SetEffect("medkit_in_inventory", true);
}

bool GOAP::Action_GrabMedkit::IsDone()
{
	return m_MedkitGrabbed;
}

void GOAP::Action_GrabMedkit::Reset()
{
	BaseAction::Reset();
	m_MedkitGrabbed = false;
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_GrabMedkit::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Medkits", m_pEntities) || m_pEntities == nullptr || m_pEntities->empty()) return false;
	
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

bool GOAP::Action_GrabMedkit::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo item;
	if (m_pInterface->Item_Grab(*m_pTarget, item))
	{
		if (item.Type != eItemType::MEDKIT) return false;
		m_pInterface->Inventory_AddItem(m_InventorySlot, item);

		// Remove item from general entity list
		std::vector<EntityInfo>* pEntities;
		pBlackboard->GetData("Entities", pEntities);
		pEntities->erase(std::find(pEntities->begin(), pEntities->end(), *m_pTarget));

		m_pEntities->erase(std::find(m_pEntities->begin(), m_pEntities->end(), *m_pTarget));
		m_pWorldState->SetVariable("medkit_in_inventory", true);
		return m_MedkitGrabbed = true;
	}

	// Couldn't grab item, abort action
	return false;
}
