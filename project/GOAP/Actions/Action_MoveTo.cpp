#include "../../stdafx.h"
#include "Action_MoveTo.h"

bool GOAP::Action_MoveTo::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_AgentInfo.Position.DistanceSquared(m_Target) <= 0.5f)
	{
		m_Steering.LinearVelocity = Elite::ZeroVector2;
		return true;
	}

	m_Steering.AutoOrient = true;
	m_Steering.LinearVelocity = m_Target - m_AgentInfo.Position;
	m_Steering.LinearVelocity.Normalize();
	m_Steering.LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	return false;
}
