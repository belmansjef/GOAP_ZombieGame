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
	SetEffect("pistol_grabbed", true);
	SetEffect("pistol_in_inventory", true);
}

bool GOAP::Action_GrabPistol::IsDone()
{
	if (m_PistolGrabbed)
	{
		m_pWorldState->SetVariable("item_looted", true);
		m_pWorldState->SetVariable("pistol_in_inventory", true);
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
	ItemInfo newPistol;
	if (m_pInterface->Item_GetInfo(*m_pTarget, newPistol))
	{
		if (newPistol.Type != eItemType::PISTOL) return false;
		if (!m_pWorldState->GetVariable("pistol_in_inventory"))
		{
			m_pInterface->Item_Grab(*m_pTarget, newPistol);
			m_pInterface->Inventory_AddItem(m_InventorySlot, newPistol);
		}
		else
		{
			ItemInfo equipedPistol;
			m_pInterface->Inventory_GetItem(m_InventorySlot, equipedPistol);
			// We have a pistol equiped, check if the new one has more ammo
			if (m_pInterface->Weapon_GetAmmo(newPistol) >= m_pInterface->Weapon_GetAmmo(equipedPistol))
			{
				// The new pistol has more amoe, destroy the current equiped and equip the new one
				m_pInterface->Inventory_RemoveItem(m_InventorySlot);
				m_pInterface->Item_Grab(*m_pTarget, newPistol);
				m_pInterface->Inventory_AddItem(m_InventorySlot, newPistol);
			}
			else
			{
				std::cout << "[Grab Pistol]: Better pistol in inventory, destroying ground item!" << std::endl;
				m_pInterface->Item_Destroy(*m_pTarget);
			}
		}
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