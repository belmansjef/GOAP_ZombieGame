#include "Action_GrabPistol.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"


GOAP::Action_GrabPistol::Action_GrabPistol()
	: BaseAction("Grab Pistol", 5)
	, m_PistolGrabbed(false)
	, m_InventorySlot(0U)
{
	SetPrecondition("pistol_aquired", true);
	SetPrecondition("pistol_in_inventory", false);
	SetEffect("pistol_aquired", false);
	SetEffect("pistol_in_inventory", true);
}

bool GOAP::Action_GrabPistol::IsDone()
{
	if (m_PistolGrabbed)
	{
		m_pWorldState->SetVariable("item_looted", true);
	}
	return m_PistolGrabbed;
}

void GOAP::Action_GrabPistol::Reset()
{
	BaseAction::Reset();
	m_PistolGrabbed = false;
}

int GOAP::Action_GrabPistol::GetCost() const
{
	return static_cast<int>(m_pTarget->Location.Distance(m_AgentInfo.Position));
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_GrabPistol::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Pistols", m_pEntities) || m_pEntities == nullptr || m_pEntities->empty()) return false;

	m_AgentInfo = m_pInterface->Agent_GetInfo();
	m_pTarget = GetClosestEntity();
	return m_pTarget != nullptr;
}
#pragma warning( pop )

bool GOAP::Action_GrabPistol::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo item;
	if (m_pInterface->Item_Grab(*m_pTarget, item))
	{
		if (item.Type != eItemType::PISTOL) return false;
		m_pInterface->Inventory_AddItem(m_InventorySlot, item);
		// Remove item from general entity list
		std::vector<EntityInfo>* pEntities;
		pBlackboard->GetData("Entities", pEntities);
		pEntities->erase(std::find(pEntities->begin(), pEntities->end(), *m_pTarget));
		

		m_pEntities->erase(std::find(m_pEntities->begin(), m_pEntities->end(), *m_pTarget));
		m_pWorldState->SetVariable("pistol_in_inventory", true);
		m_pWorldState->SetVariable("item_looted", true);
		return m_PistolGrabbed = true;
	}
	
	// Couldn't grab item, abort action
	return false;
}