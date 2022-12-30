#include "../stdafx.h"
#include "Actions.h"
#include "IExamInterface.h"
#include "WorldState.h"

GOAP::Action_Wander::Action_Wander()
	: BaseAction("Wander", 5)
	, m_pSteering(new SteeringPlugin_Output())
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

GOAP::Action_MoveTo::Action_MoveTo()
	: BaseAction("Move To", 5)
	, m_pSteering(new SteeringPlugin_Output())
{
	SetEffect("target_in_range", true);
}

bool GOAP::Action_MoveTo::IsValid(Elite::Blackboard* pBlackboard)
{
	return
		pBlackboard->GetData("Target", m_Target)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_MoveTo::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_Target) <= (m_AgentInfo.GrabRange * m_AgentInfo.GrabRange))
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		return true;
	}

	m_pSteering->AutoOrient = true;
	m_pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(m_Target) - m_AgentInfo.Position).GetNormalized();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	if (m_pWorldState->GetVariable("inside_purgezone"))
	{
		if (m_pInterface->Agent_GetInfo().Stamina > 4.f)
			m_pSteering->RunMode = true;
		else if (m_pInterface->Agent_GetInfo().Stamina == 0.f)
			m_pSteering->RunMode = false;
	}
	else
		m_pSteering->RunMode = false;


	return false;
}

GOAP::Action_GrabFood::Action_GrabFood()
	: BaseAction("Grab Food", 5)
	, m_pSteering(nullptr)
	, m_pFood(nullptr)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("food_aquired", true);
	SetPrecondition("food_in_inventory", false);
	SetEffect("food_in_inventory", true);
}

bool GOAP::Action_GrabFood::IsValid(Elite::Blackboard* pBlackboard)
{
	return
		pBlackboard->GetData("TargetItem", m_TargetItem)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Food", m_pFood);
}

bool GOAP::Action_GrabFood::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = false;

	const Elite::Vector2 dir_vector = (m_pFood->back().Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		if (m_pInterface->Item_Grab({}, m_TargetItem) && m_TargetItem.Type == eItemType::FOOD)
		{
			m_pInterface->Inventory_AddItem(3U, m_TargetItem);
			m_pFood->pop_back();
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

GOAP::Action_GrabPistol::Action_GrabPistol()
	: BaseAction("Grab Pistol", 5)
	, m_pSteering(nullptr)
	, m_pPistols(nullptr)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("pistol_aquired", true);
	SetEffect("pistol_aquired", false);
	SetEffect("pistol_in_inventory", true);
}

bool GOAP::Action_GrabPistol::IsValid(Elite::Blackboard* pBlackboard)
{
	return
		pBlackboard->GetData("TargetItem", m_TargetItem)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Pistols", m_pPistols);
}

bool GOAP::Action_GrabPistol::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = false;

	const Elite::Vector2 dir_vector = (m_pPistols->back().Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		if (m_pInterface->Item_Grab({}, m_TargetItem) && m_TargetItem.Type == eItemType::PISTOL)
		{
			m_pInterface->Inventory_AddItem(0U, m_TargetItem);
			m_pPistols->pop_back();
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

GOAP::Action_GrabShotgun::Action_GrabShotgun()
	: BaseAction("Grab Shotgun", 5)
	, m_pSteering(nullptr)
	, m_pShotguns(nullptr)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("shotgun_aquired", true);
	SetEffect("shotgun_aquired", false);
	SetEffect("shotgun_in_inventory", true);
}

bool GOAP::Action_GrabShotgun::IsValid(Elite::Blackboard* pBlackboard)
{
	return
		pBlackboard->GetData("TargetItem", m_TargetItem)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Shotguns", m_pShotguns);
}

bool GOAP::Action_GrabShotgun::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = false;

	const Elite::Vector2 dir_vector = (m_pShotguns->back().Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		if (m_pInterface->Item_Grab({}, m_TargetItem) && m_TargetItem.Type == eItemType::SHOTGUN)
		{
			m_pInterface->Inventory_AddItem(1U, m_TargetItem);
			m_pShotguns->pop_back();
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

GOAP::Action_GrabMedkit::Action_GrabMedkit()
	: BaseAction("Grab Medkit", 5)
	, m_pSteering(nullptr)
	, m_pMeds(nullptr)
{
	SetPrecondition("target_in_range", true);
	SetPrecondition("medkit_aquired", true);
	SetEffect("medkit_aquired", false);
	SetEffect("medkit_in_inventory", true);
}

bool GOAP::Action_GrabMedkit::IsValid(Elite::Blackboard* pBlackboard)
{
	return
		pBlackboard->GetData("TargetItem", m_TargetItem)
		&& pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Meds", m_pMeds);
}

bool GOAP::Action_GrabMedkit::Execute(Elite::Blackboard* pBlackboard)
{
	m_pSteering->AutoOrient = false;

	const Elite::Vector2 dir_vector = (m_pMeds->back().Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		if (m_pInterface->Item_Grab({}, m_TargetItem) && m_TargetItem.Type == eItemType::MEDKIT)
		{
			m_pInterface->Inventory_AddItem(2U, m_TargetItem);
			m_pMeds->pop_back();
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

GOAP::Action_EatFood::Action_EatFood()
	: BaseAction("Eat Food", 1)
{
	SetPrecondition("food_in_inventory", true);
	SetEffect("food_in_inventory", false);
	SetEffect("low_hunger", false);
}

bool GOAP::Action_EatFood::IsValid(Elite::Blackboard* pBlackboard)
{
	return pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState);
}

bool GOAP::Action_EatFood::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Inventory_UseItem(3U))
	{
		m_pInterface->Inventory_RemoveItem(3U);			
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

GOAP::Action_KillEnemy_Pistol::Action_KillEnemy_Pistol()
	: BaseAction("Kill Enemy Pistol", 5)
	, m_pSteering(nullptr)
	, m_InventorySlot(0U)
{
	SetPrecondition("enemy_aquired", true);
	SetPrecondition("pistol_in_inventory", true);
	SetEffect("enemy_aquired", false);
	SetEffect("in_danger", false);
}

bool GOAP::Action_KillEnemy_Pistol::IsValid(Elite::Blackboard* pBlackboard)
{
	pBlackboard->GetData("Interface", m_pInterface);
	ItemInfo weapon;
	if (!m_pInterface->Inventory_GetItem(m_InventorySlot, weapon)) return false;
	return 
		pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Enemies", m_Enemies)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& m_pInterface->Weapon_GetAmmo(weapon) != 0
		&& !m_Enemies.empty();
}

bool GOAP::Action_KillEnemy_Pistol::Execute(Elite::Blackboard* pBlackboard)
{
	float frameTime;
	pBlackboard->GetData("FrameTime", frameTime);

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


	m_LastShotTimer += frameTime;
	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		if (m_LastShotTimer < m_ShotCooldown) return false;

		m_LastShotTimer = 0.f;
		m_pInterface->Inventory_UseItem(m_InventorySlot);
		if (m_Enemies.back().Health <= 1.f)
		{
			*m_pWorldState = ActOn(*m_pWorldState);
			return true;
		}
	}

	m_pSteering->AngularVelocity = 1.f;
	if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
	{
		m_pSteering->AngularVelocity = -1.f;
	}

	return false;
}

GOAP::Action_KillEnemy_Shotgun::Action_KillEnemy_Shotgun()
	: BaseAction("Kill Enemy Shotgun", 3)
	, m_pSteering(nullptr)
	, m_InventorySlot(1U)
{
	SetPrecondition("enemy_aquired", true);
	SetPrecondition("shotgun_in_inventory", true);
	SetEffect("enemy_aquired", false);
	SetEffect("in_danger", false);
}

bool GOAP::Action_KillEnemy_Shotgun::IsValid(Elite::Blackboard* pBlackboard)
{
	pBlackboard->GetData("Interface", m_pInterface);
	ItemInfo weapon;
	// if (!m_pInterface->Inventory_GetItem(m_InventorySlot, weapon)) return false;

	return 
		pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Enemies", m_Enemies)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		// && m_pInterface->Weapon_GetAmmo(weapon) != 0
		&& !m_Enemies.empty();
}

bool GOAP::Action_KillEnemy_Shotgun::Execute(Elite::Blackboard* pBlackboard)
{
	float frameTime;
	pBlackboard->GetData("FrameTime", frameTime);

	m_pSteering->AutoOrient = false;
	m_pSteering->LinearVelocity = Elite::ZeroVector2;

	const Elite::Vector2 dir_vector = (m_Enemies.back().Location - m_AgentInfo.Position).GetNormalized();

	const float target_angle = atan2f(dir_vector.y, dir_vector.x);
	const float agent_angle = m_AgentInfo.Orientation;
	const float delta_angle = target_angle - agent_angle;

	m_LastShotTimer += frameTime;
	if (abs(delta_angle) <= m_AngleError)
	{
		m_pSteering->AngularVelocity = 0.f;
		if (m_LastShotTimer < m_ShotCooldown) return false;

		m_LastShotTimer = 0.f;
		m_pInterface->Inventory_UseItem(m_InventorySlot);
		if (m_Enemies.back().Health <= 1.f)
		{
			*m_pWorldState = ActOn(*m_pWorldState);
			return true;
		}
	}

	m_pSteering->AngularVelocity = 1.f;
	if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
	{
		m_pSteering->AngularVelocity = -1.f;
	}

	return false;
}

GOAP::Action_FleeToSafety::Action_FleeToSafety()
	: BaseAction("Flee To Safety", 20)
	, m_pSteering(nullptr)
	, m_pHouses(nullptr)
{
	SetPrecondition("enemy_aquired", true);
	SetPrecondition("house_aquired", true);
	SetEffect("enemy_aquired", false);
	SetEffect("in_danger", false);
}

bool GOAP::Action_FleeToSafety::IsValid(Elite::Blackboard* pBlackboard)
{
	return pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("Houses", m_pHouses);
}

bool GOAP::Action_FleeToSafety::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_pHouses->back().Location) <= 10.f)
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		*m_pWorldState = ActOn(*m_pWorldState);
		return true;
	}

	m_pSteering->AutoOrient = true;
	m_pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(m_pHouses->back().Location) - m_AgentInfo.Position).GetNormalized();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	if (m_pInterface->Agent_GetInfo().Stamina > 4.f)
		m_pSteering->RunMode = true;
	else if(m_pInterface->Agent_GetInfo().Stamina == 0.f)
		m_pSteering->RunMode = false;

	return false;
}

GOAP::Action_SearchEnemy::Action_SearchEnemy()
	:BaseAction("Search Enemy", 1, 3.f)
	, m_pSteering(nullptr)
{
	SetPrecondition("enemy_aquired", false);
	SetEffect("enemy_aquired", true);
}

bool GOAP::Action_SearchEnemy::IsValid(Elite::Blackboard* pBlackboard)
{
	return pBlackboard->GetData("Interface", m_pInterface)
		&& pBlackboard->GetData("WorldState", m_pWorldState)
		&& pBlackboard->GetData("Enemies", m_Enemies)
		&& pBlackboard->GetData("AgentInfo", m_AgentInfo)
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("FrameTime", m_FrameTime)
		&& m_ActionTimer < m_ActionTimeout;
}

bool GOAP::Action_SearchEnemy::Execute(Elite::Blackboard* pBlackboard)
{
	if (!m_Enemies.empty())
	{
		m_pSteering->AngularVelocity = 0.f;
		return true;
	}

	m_ActionTimer += m_FrameTime;

	m_pSteering->AutoOrient = false;
	m_pSteering->LinearVelocity = Elite::ZeroVector2;
	m_pSteering->AngularVelocity = m_AgentInfo.MaxAngularSpeed;
	return false;
}