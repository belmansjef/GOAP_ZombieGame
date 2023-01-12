#include "Action_GrabShotgun.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_GrabShotgun::Action_GrabShotgun()
	: BaseAction("Grab Shotgun", 5)
	, m_ShotgunGrabbed(false)
	, m_InventorySlot(1U)
{
	SetPrecondition("shotgun_aquired", true);
	SetEffect("shotgun_grabbed", true);
	SetEffect("shotgun_in_inventory", true);
}

bool GOAP::Action_GrabShotgun::IsDone()
{
	if (m_ShotgunGrabbed)
	{
		m_pWorldState->SetVariable("item_looted", true);
		m_pWorldState->SetVariable("shotgun_in_inventory", true);
	}
	return m_ShotgunGrabbed;
}

void GOAP::Action_GrabShotgun::Reset()
{
	BaseAction::Reset();
	m_ShotgunGrabbed = false;
}

int GOAP::Action_GrabShotgun::GetCost() const
{
	return static_cast<int>(m_pTarget->Location.Distance(m_AgentInfo.Position));
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_GrabShotgun::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Shotguns", m_pEntities) || m_pEntities == nullptr || m_pEntities->empty()) return false;

	m_AgentInfo = m_pInterface->Agent_GetInfo();
	m_pTarget = GetClosestEntity();
	return m_pTarget != nullptr;
}
#pragma warning( pop )

bool GOAP::Action_GrabShotgun::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo newShotgun;
	if (m_pInterface->Item_GetInfo(*m_pTarget, newShotgun))
	{
		if (newShotgun.Type != eItemType::SHOTGUN) return false;
		if (!m_pWorldState->GetVariable("shotgun_in_inventory"))
		{
			m_pInterface->Item_Grab(*m_pTarget, newShotgun);
			m_pInterface->Inventory_AddItem(m_InventorySlot, newShotgun);
		}
		else
		{
			ItemInfo equipedShotgun;
			m_pInterface->Inventory_GetItem(m_InventorySlot, equipedShotgun);
			// We have a pistol equiped, check if the new one has more ammo
			if (m_pInterface->Weapon_GetAmmo(newShotgun) >= m_pInterface->Weapon_GetAmmo(equipedShotgun))
			{
				// The new pistol has more amoe, destroy the current equiped and equip the new one
				m_pInterface->Inventory_RemoveItem(m_InventorySlot);
				m_pInterface->Item_Grab(*m_pTarget, newShotgun);
				m_pInterface->Inventory_AddItem(m_InventorySlot, newShotgun);
			}
			else
			{
				std::cout << "[Grab Shotgun]: Better shotgun in inventory, destroying ground item!" << std::endl;
				m_pInterface->Item_Destroy(*m_pTarget);
			}
		}
		// Remove item from general entity list
		std::vector<EntityInfo>* pEntities;
		pBlackboard->GetData("Entities", pEntities);
		pEntities->erase(std::find(pEntities->begin(), pEntities->end(), *m_pTarget));

		m_pEntities->erase(std::find(m_pEntities->begin(), m_pEntities->end(), *m_pTarget));
		m_pWorldState->SetVariable("shotgun_in_inventory", true);
		m_pWorldState->SetVariable("item_looted", true);
		return m_ShotgunGrabbed = true;
	}

	// Couldn't grab item, abort action
	return false;
}
