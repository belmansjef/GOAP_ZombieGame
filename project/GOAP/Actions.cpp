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
	m_pSteering->LinearVelocity = m_Target - m_AgentInfo.Position;
	m_pSteering->LinearVelocity.Normalize();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

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
