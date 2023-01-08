#include "Action_FleePurgezone.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_FleePurgezone::Action_FleePurgezone()
    : BaseAction("Flee Purgezone", 5)
    , m_FleedPurgezone(false)
    , m_pSteering(nullptr)
{
    SetPrecondition("inside_purgezone", true);
    SetEffect("inside_purgezone", false);
}

bool GOAP::Action_FleePurgezone::IsDone()
{
    if (m_FleedPurgezone)
    {
        m_pWorldState->SetVariable("inside_purgezone", false);
        m_pWorldState->SetVariable("in_danger", false);
    }
   
    return m_FleedPurgezone;
}

void GOAP::Action_FleePurgezone::Reset()
{
    SAFE_DELETE(m_pTarget);
    BaseAction::Reset();
    m_FleedPurgezone = false;
}

bool GOAP::Action_FleePurgezone::IsValid(Elite::Blackboard* pBlackboard)
{
    Elite::Vector2 target;
    if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
    if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
    if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
    if (!pBlackboard->GetData("Steering", m_pSteering) || m_pWorldState == nullptr) return false;
    if (!pBlackboard->GetData("Target", target)) return false;

    return true;
}

bool GOAP::Action_FleePurgezone::Execute(Elite::Blackboard* pBlackboard)
{
    Elite::Vector2 target;
    if (!pBlackboard->GetData("Target", target)) return false;
    m_AgentInfo = m_pInterface->Agent_GetInfo();

    if (m_AgentInfo.Stamina >= 3.f)
        m_pSteering->RunMode = true;
    else if (m_AgentInfo.Stamina == 0.f)
        m_pSteering->RunMode = false;

    m_pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(target) - m_AgentInfo.Position).GetNormalized();
    m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;
    m_FleedPurgezone = (m_AgentInfo.Position.DistanceSquared(target) <= m_AgentInfo.GrabRange * m_AgentInfo.GrabRange + 10.f);

    return true;
}
