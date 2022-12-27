#include "../stdafx.h"
#include "Actions.h"
#include "IExamInterface.h"
#include "WorldState.h"

GOAP::Action_Wander::Action_Wander()
	: BaseAction::BaseAction("Wander", 5)
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
		&& pBlackboard->GetData("Steering", m_pSteering)
		&& pBlackboard->GetData("Interface", m_pInterface);
}

bool GOAP::Action_MoveTo::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_Target) <= 5.f)
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		return true;
	}

	m_pSteering->AutoOrient = true;
	m_pSteering->LinearVelocity = m_pInterface->NavMesh_GetClosestPathPoint(m_Target) - m_AgentInfo.Position;
	m_pSteering->LinearVelocity.Normalize();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	/*if (m_pInterface->Agent_GetInfo().Stamina > 4.f)
	{
		m_pSteering->RunMode = true;
	}
	else if(m_pInterface->Agent_GetInfo().Stamina == 0.f)
	{
		m_pSteering->RunMode = false;
	}*/

	return false;
}

GOAP::Action_GrabFood::Action_GrabFood()
	: BaseAction::BaseAction("Grab Food", 5)
	, m_pSteering(nullptr)
	, m_pWorldState(nullptr)
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

	const Elite::Vector2 dir_vector = (m_Target - m_AgentInfo.Position).GetNormalized();

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
	: BaseAction::BaseAction("Grab Pistol", 5)
	, m_pSteering(nullptr)
	, m_pWorldState(nullptr)
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

	const Elite::Vector2 dir_vector = (m_Target - m_AgentInfo.Position).GetNormalized();

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
	: BaseAction::BaseAction("Grab Shotgun", 5)
	, m_pSteering(nullptr)
	, m_pWorldState(nullptr)
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

	const Elite::Vector2 dir_vector = (m_Target - m_AgentInfo.Position).GetNormalized();

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
	: BaseAction::BaseAction("Grab Medkit", 5)
	, m_pSteering(nullptr)
	, m_pWorldState(nullptr)
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

	const Elite::Vector2 dir_vector = (m_Target - m_AgentInfo.Position).GetNormalized();

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
	: BaseAction::BaseAction("Eat Food", 1)
	, m_pWorldState(nullptr)
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
	: BaseAction::BaseAction("Heal", 1)
	, m_pWorldState(nullptr)
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