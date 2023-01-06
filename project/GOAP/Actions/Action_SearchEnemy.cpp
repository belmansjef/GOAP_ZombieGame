#include "Action_SearchEnemy.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_SearchEnemy::Action_SearchEnemy()
	: BaseAction("Search Enemy", 5)
	, m_EnemyAquired(false)
	, m_pSteering(nullptr)
	, m_ActionTimer(0.f)
	, m_FrameTime(0.f)
{
	SetPrecondition("in_danger", true);
	SetPrecondition("enemy_aquired", false);
	SetEffect("enemy_aquired", true);
}

bool GOAP::Action_SearchEnemy::IsDone()
{
	if (m_EnemyAquired)
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
	}
	return m_EnemyAquired;
}

void GOAP::Action_SearchEnemy::Reset()
{
	BaseAction::Reset();
	m_EnemyAquired = false;
	m_FrameTime = 0.f;
	m_ActionTimer = 0.f;
}

bool GOAP::Action_SearchEnemy::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Steering", m_pSteering) || m_pSteering == nullptr) return false;

	return true;
}

bool GOAP::Action_SearchEnemy::Execute(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Enemies", m_Enemies)) return false;
	if (!pBlackboard->GetData("FrameTime", m_FrameTime)) return false;
	if (!m_Enemies.empty())
	{
		m_EnemyAquired = true;
		return true;
	}

	m_ActionTimer += m_FrameTime;
	if (m_ActionTimer >= m_ActionTimeout)
	{
		m_EnemyAquired = true;
		m_pWorldState->SetVariable("in_danger", false);
		std::cout << "Action [Search Enemy] timed-out!" << std::endl;
		return true;
	}	

	const AgentInfo agentInfo{ m_pInterface->Agent_GetInfo() };
	/*m_pSteering->LinearVelocity = Elite::OrientationToVector(agentInfo.Orientation);
	m_pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;*/

	m_pSteering->AutoOrient = false;
	m_pSteering->AngularVelocity = agentInfo.MaxAngularSpeed;

	return true;
}
