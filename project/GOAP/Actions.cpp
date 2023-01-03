#include "../stdafx.h"
#include "Actions.h"
#include "IExamInterface.h"

GOAP::Action_MoveTo::Action_MoveTo()
	: BaseAction::BaseAction("Move To", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	SetEffect("target_in_range", true);
}

bool GOAP::Action_MoveTo::IsValid(Elite::Blackboard* pBlackboard)
{
	return
		pBlackboard->GetData("Target", m_Target)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Steering", m_pSteering);
}

bool GOAP::Action_MoveTo::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_Target) <= 0.5f)
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		return true;
	}

	m_pSteering->AutoOrient = true;
<<<<<<< Updated upstream
	m_pSteering->LinearVelocity = m_Target - m_AgentInfo.Position;
	m_pSteering->LinearVelocity.Normalize();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

=======
	m_pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(m_Target) - m_AgentInfo.Position).GetNormalized();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	if (m_AgentInfo.Stamina > 4.f && m_pWorldState->GetVariable("in_danger"))
		m_pSteering->RunMode = true;
	else if (m_AgentInfo.Stamina == 0.f)
		m_pSteering->RunMode = false;

	return false;
}

GOAP::Action_FaceTarget::Action_FaceTarget()
	: BaseAction("Face Target", 5)
{
	SetPrecondition("target_in_range", true);
	SetEffect("is_facing_target", true);
}

bool GOAP::Action_FaceTarget::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Target", m_Target);
}

bool GOAP::Action_FaceTarget::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = false;
	const Elite::Vector2 dir_vector = (m_Target - m_AgentInfo.Position).GetNormalized();
	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		return true;
	}

	m_pSteering->AngularVelocity = m_AgentInfo.MaxAngularSpeed;
	if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
	{
		m_pSteering->AngularVelocity = -m_AgentInfo.MaxAngularSpeed;
	}

	return false;
}

GOAP::Action_GrabPistol::Action_GrabPistol()
	: BaseAction("Grab Pistol", 5, 4.f)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("is_facing_target", true);
	SetPrecondition("pistol_aquired", true);
	SetPrecondition("pistol_in_inventory", false);
	SetEffect("pistol_aquired", false);
	SetEffect("pistol_in_inventory", true);
}

bool GOAP::Action_GrabPistol::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("PistolEntities", m_Entities)
		&& pBlackboard->GetData("PistolPositions", m_EntityPositions)
		&& !m_Entities.empty();
}

bool GOAP::Action_GrabPistol::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo item;
	if (m_pInterface->Item_Grab(m_Entities.back(), item))
	{
		m_pInterface->Inventory_AddItem(0U, item);
		m_EntityPositions->pop_back();
		*m_pWorldState = ActOn(*m_pWorldState);
		return true;
	}
	return false;
}

GOAP::Action_GrabShotgun::Action_GrabShotgun()
	: BaseAction("Grab Shotgun", 5, 4.f)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("is_facing_target", true);
	SetPrecondition("shotgun_aquired", true);
	SetPrecondition("shotgun_in_inventory", false);
	SetEffect("shotgun_aquired", false);
	SetEffect("shotgun_in_inventory", true);
}

bool GOAP::Action_GrabShotgun::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("ShotgunEntities", m_Entities)
		&& pBlackboard->GetData("ShotgunPositions", m_EntityPositions)
		&& !m_Entities.empty();
}

bool GOAP::Action_GrabShotgun::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo item;
	if (m_pInterface->Item_Grab(m_Entities.back(), item))
	{
		m_pInterface->Inventory_AddItem(1U, item);
		m_EntityPositions->pop_back();
		*m_pWorldState = ActOn(*m_pWorldState);
		return true;
	}
	return false;
}

GOAP::Action_GrabMedkit::Action_GrabMedkit()
	: BaseAction("Grab Medkit", 5, 4.f)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("is_facing_target", true);
	SetPrecondition("medkit_aquired", true);
	SetPrecondition("medkit_in_inventory", false);
	SetEffect("medkit_aquired", false);
	SetEffect("medkit_in_inventory", true);
}

bool GOAP::Action_GrabMedkit::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("MedEntities", m_Entities)
		&& pBlackboard->GetData("MedPositions", m_EntityPositions)
		&& !m_Entities.empty();
}

bool GOAP::Action_GrabMedkit::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo item;
	if (m_pInterface->Item_Grab(m_Entities.back(), item))
	{
		m_pInterface->Inventory_AddItem(2U, item);
		m_EntityPositions->pop_back();
		*m_pWorldState = ActOn(*m_pWorldState);
		return true;
	}
	return false;
}

GOAP::Action_GrabFood::Action_GrabFood()
	: BaseAction("Grab Food", 5, 4.f)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("is_facing_target", true);
	SetPrecondition("food_aquired", true);
	SetPrecondition("food_inventory_full", false);
	SetEffect("food_aquired", false);
}

bool GOAP::Action_GrabFood::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("FoodEntities", m_Entities)
		&& pBlackboard->GetData("FoodPositions", m_EntityPositions)
		&& !m_Entities.empty();
}

bool GOAP::Action_GrabFood::Execute(Elite::Blackboard* pBlackboard)
{
	ItemInfo item;
	if (m_pInterface->Item_Grab(m_Entities.back(), item))
	{
		if (!m_pWorldState->GetVariable("food_in_inventory"))
		{
			m_pInterface->Inventory_AddItem(3U, item);
			m_pWorldState->SetVariable("food_in_inventory", true);
		}
		else
		{
			m_pInterface->Inventory_AddItem(4U, item);
			m_pWorldState->SetVariable("food_inventory_full", true);
		}

		m_EntityPositions->pop_back();
		*m_pWorldState = ActOn(*m_pWorldState);
		return true;
	}
	return false;
}

GOAP::Action_DropPistol::Action_DropPistol()
	: BaseAction("Drop Pistol", 5)
{
	SetPrecondition("pistol_in_inventory", true);
	SetEffect("pistol_in_inventory", false);
}

bool GOAP::Action_DropPistol::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_DropPistol::Execute(Elite::Blackboard* pBlackboard)
{
	m_pInterface->Inventory_RemoveItem(0u);
	*m_pWorldState = ActOn(*m_pWorldState);
	return true;
}

GOAP::Action_DropShotgun::Action_DropShotgun()
	:BaseAction("Drop Shotgun", 5)
{
	SetPrecondition("shotgun_in_inventory", true);
	SetEffect("shotgun_in_inventory", false);
}

bool GOAP::Action_DropShotgun::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_DropShotgun::Execute(Elite::Blackboard* pBlackboard)
{
	m_pInterface->Inventory_RemoveItem(1u);
	*m_pWorldState = ActOn(*m_pWorldState);
	return true;
}

GOAP::Action_DropMedkit::Action_DropMedkit()
	: BaseAction("Drop Medkit", 5)
{
	SetPrecondition("medkit_in_inventory", true);
	SetEffect("medkit_in_inventory", false);
}

bool GOAP::Action_DropMedkit::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_DropMedkit::Execute(Elite::Blackboard* pBlackboard)
{
	m_pInterface->Inventory_RemoveItem(2u);
	*m_pWorldState = ActOn(*m_pWorldState);
	return true;
}

GOAP::Action_DropFood::Action_DropFood()
	: BaseAction("Drop Food", 5)
{
	SetPrecondition("food_in_inventory", true);
	SetEffect("food_inventory_full", false);
}

bool GOAP::Action_DropFood::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_DropFood::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pWorldState->GetVariable("food_inventory_full"))
	{
		m_pInterface->Inventory_RemoveItem(4u);
	}
	else
	{
		m_pInterface->Inventory_RemoveItem(3u);
	}
	*m_pWorldState = ActOn(*m_pWorldState);
	return true;
}

GOAP::Action_EatFood::Action_EatFood()
	: BaseAction("Eat Food", 1)
{
	SetPrecondition("food_in_inventory", true);
	SetEffect("low_hunger", false);
	SetEffect("food_inventory_full", false);
}

bool GOAP::Action_EatFood::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_EatFood::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pWorldState->GetVariable("food_inventory_full"))
	{
		if (m_pInterface->Inventory_UseItem(4U))
		{
			m_pInterface->Inventory_RemoveItem(4U);
			*m_pWorldState = ActOn(*m_pWorldState);
			return true;
		}
	}
	else if (m_pInterface->Inventory_UseItem(3U))
	{
		m_pInterface->Inventory_RemoveItem(3U);
		m_pWorldState->SetVariable("food_in_inventory", false);
		*m_pWorldState = ActOn(*m_pWorldState);
		return true;
	}
	return false;
}

GOAP::Action_Heal::Action_Heal()
	: BaseAction("Heal", 1)
{
	SetPrecondition("medkit_in_inventory", true);
	SetEffect("medkit_in_inventory", false);
	SetEffect("low_health", false);
}

bool GOAP::Action_Heal::IsValid(Elite::Blackboard* pBlackboard)
{
	return pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_Heal::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Inventory_UseItem(2U))
	{
		m_pInterface->Inventory_RemoveItem(2U);
		*m_pWorldState = ActOn(*m_pWorldState);
		return true;
	}
	return false;
}

GOAP::Action_DestroyGarbage::Action_DestroyGarbage()
	: BaseAction("Destroy Garbage", 5, 4.f)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("is_facing_target", true);
	SetPrecondition("garbage_aquired", true);
	SetEffect("garbage_aquired", false);
	SetEffect("garbage_destroyed", true);
}

bool GOAP::Action_DestroyGarbage::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("GarbageEntities", m_Entities)
		&& pBlackboard->GetData("GarbagePositions", m_EntityPositions)
		&& !m_Entities.empty();
}

bool GOAP::Action_DestroyGarbage::Execute(Elite::Blackboard* pBlackboard)
{
	m_pInterface->Item_Destroy(m_Entities.back());
	m_EntityPositions->pop_back();
	return true;
}

GOAP::Action_KillEnemy_Pistol::Action_KillEnemy_Pistol()
	: BaseAction("Kill Enemy Pistol", 5)
	, m_InventorySlot(0U)
{
	SetPrecondition("enemy_aquired", true);
	SetPrecondition("pistol_in_inventory", true);
	SetEffect("enemy_aquired", false);
	SetEffect("in_danger", false);
}

bool GOAP::Action_KillEnemy_Pistol::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("EnemyEntities", m_Enemies)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& !m_Enemies.empty();
}

bool GOAP::Action_KillEnemy_Pistol::Execute(Elite::Blackboard* pBlackboard)
{
	// Move away from enemy
	m_pSteering->LinearVelocity = m_AgentInfo.Position - m_pInterface->NavMesh_GetClosestPathPoint(m_Enemies.back().Location);
	m_pSteering->LinearVelocity.Normalize();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	// Look towards enemy
	m_pSteering->AutoOrient = false;
	const Elite::Vector2 dir_vector = (m_Enemies.back().Location - m_AgentInfo.Position).GetNormalized();
	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	m_LastShotTimer += m_FrameTime;
	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		if (m_LastShotTimer < m_ShotCooldown) return false;

		m_LastShotTimer = 0.f;
		if (!m_pInterface->Inventory_UseItem(m_InventorySlot))
		{
			m_pInterface->Inventory_RemoveItem(m_InventorySlot);
			m_pWorldState->SetVariable("pistol_in_inventory", false);
			return true;
		}

		if (m_Enemies.back().Health <= 1.f)
		{
			*m_pWorldState = ActOn(*m_pWorldState);
			return true;
		}
	}

	m_pSteering->AngularVelocity = m_AgentInfo.MaxAngularSpeed;
	if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
	{
		m_pSteering->AngularVelocity = -m_AgentInfo.MaxAngularSpeed;
	}

	return false;
}

GOAP::Action_KillEnemy_Shotgun::Action_KillEnemy_Shotgun()
	: BaseAction("Kill Enemy Shotgun", 3)
	, m_InventorySlot(1U)
{
	SetPrecondition("enemy_aquired", true);
	SetPrecondition("shotgun_in_inventory", true);
	SetEffect("enemy_aquired", false);
	SetEffect("in_danger", false);
}

bool GOAP::Action_KillEnemy_Shotgun::IsValid(Elite::Blackboard* pBlackboard)
{
	return BaseAction::IsValid(pBlackboard)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("EnemyEntities", m_Enemies)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& !m_Enemies.empty();
}

bool GOAP::Action_KillEnemy_Shotgun::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = false;

	// Move away from enemy
	m_pSteering->LinearVelocity = m_AgentInfo.Position - m_pInterface->NavMesh_GetClosestPathPoint(m_Enemies.back().Location);
	m_pSteering->LinearVelocity.Normalize();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	const Elite::Vector2 dir_vector = (m_Enemies.back().Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	m_LastShotTimer += m_FrameTime;
	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		if (m_LastShotTimer < m_ShotCooldown) return false;

		m_LastShotTimer = 0.f;
		if (!m_pInterface->Inventory_UseItem(m_InventorySlot))
		{
			m_pInterface->Inventory_RemoveItem(m_InventorySlot);
			m_pWorldState->SetVariable("shotgun_in_inventory", false);
			return true;
		}
	
		if (m_Enemies.back().Health <= 1.f)
		{
			*m_pWorldState = ActOn(*m_pWorldState);
			return true;
		}
	}

	m_pSteering->AngularVelocity = m_AgentInfo.MaxAngularSpeed;
	if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
	{
		m_pSteering->AngularVelocity = -m_AgentInfo.MaxAngularSpeed;
	}
	return false;
}

GOAP::Action_FleeToSafety::Action_FleeToSafety()
	: BaseAction("Flee To Safety", 20, 10.f)
{
	SetPrecondition("in_danger", true);
	SetEffect("in_danger", false);
}

bool GOAP::Action_FleeToSafety::IsValid(Elite::Blackboard* pBlackboard)
{
	pBlackboard->GetData("WorldState", m_pWorldState);
	if (!BaseAction::IsValid(pBlackboard))
	{
		m_pWorldState->SetVariable("in_danger", false);
		return false;
	}

	return BaseAction::IsValid(pBlackboard)
		&& m_ActionMoveTo.IsValid(pBlackboard)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("Target", m_Target);
}

bool GOAP::Action_FleeToSafety::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pWorldState->GetVariable("enemy_aquired"))
	{
		m_ActionMoveTo.SetTarget((m_Target - m_AgentInfo.Position).GetNormalized() * 300.f);
		m_pInterface->Draw_Circle((m_Target - m_AgentInfo.Position).GetNormalized() * 300.f, 2.f, { 1.f, 0.f, 0.f });
	}
	else
	{
		m_ActionMoveTo.SetTarget((m_AgentInfo.Position - m_Target).GetNormalized() * 300.f);
		m_pInterface->Draw_Circle((m_Target - m_AgentInfo.Position).GetNormalized() * 300.f, 2.f, { 1.f, 0.f, 0.f });
	}	

	if (m_ActionMoveTo.Execute(pBlackboard))
	{
		*m_pWorldState = ActOn(*m_pWorldState);
		return true;
	}
>>>>>>> Stashed changes
	return false;
}

GOAP::Action_Wander::Action_Wander()
	: BaseAction::BaseAction("Wander", 0)
	, m_pSteering(new SteeringPlugin_Output())
	, m_pInterface(nullptr)
{
	SetEffect("wandering", true);
}

bool GOAP::Action_Wander::IsValid(Elite::Blackboard* pBlackboard)
{
	return
		pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("Interface", m_pInterface);
}

bool GOAP::Action_Wander::Execute(Elite::Blackboard* pBlackboard)
{
	m_WanderAngle += Elite::randomFloat(-45.f, 45.f);
	const Elite::Vector2 circle_center = m_AgentInfo.Position + (m_AgentInfo.LinearVelocity.GetNormalized() * 10.f);
	const Elite::Vector2 desired_location = m_pInterface->NavMesh_GetClosestPathPoint({ cosf(m_WanderAngle) * 10.f + circle_center.x, sinf(m_WanderAngle) * 10.f + circle_center.y });

	m_pSteering->AutoOrient = true;
	m_pSteering->LinearVelocity = desired_location - m_pInterface->Agent_GetInfo().Position;
	m_pSteering->LinearVelocity.Normalize();
	m_pSteering->LinearVelocity *= m_pInterface->Agent_GetInfo().MaxLinearSpeed;

	return false;
}
