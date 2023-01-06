#include "Action_FleePurgezone.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_FleePurgezone::Action_FleePurgezone()
    : BaseAction("Flee Purgezone", 5)
    , m_FleedPurgezone(false)
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
    if (!pBlackboard->GetData("Target", target)) return false;

    EntityInfo* ei = new EntityInfo;
    ei->Location = target;
    m_pTarget = ei;

    return m_pTarget != nullptr;
}

bool GOAP::Action_FleePurgezone::Execute(Elite::Blackboard* pBlackboard)
{
    return m_FleedPurgezone = true;
}
