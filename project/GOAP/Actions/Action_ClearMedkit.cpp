#include "Action_ClearMedkit.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_ClearMedkit::Action_ClearMedkit()
    : BaseAction("Clear Medkit", 5)
    , m_MedkitCleared(false)
{
    SetPrecondition("medkit_aquired", true);
    SetPrecondition("medkit_limit_reached", true);
    SetEffect("ground_cleared", true);
}

bool GOAP::Action_ClearMedkit::IsDone()
{
    return m_MedkitCleared;
}

void GOAP::Action_ClearMedkit::Reset()
{
    BaseAction::Reset();
    m_MedkitCleared = false;
}

int GOAP::Action_ClearMedkit::GetCost() const
{
    return static_cast<int>(m_pTarget->Location.Distance(m_AgentInfo.Position));
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_ClearMedkit::IsValid(Elite::Blackboard* pBlackboard)
{
    if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
    if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
    if (!pBlackboard->GetData("Medkits", m_pEntities) || m_pEntities == nullptr || m_pEntities->empty()) return false;

    m_AgentInfo = m_pInterface->Agent_GetInfo();
    m_pTarget = GetClosestEntity();
    return m_pTarget != nullptr;
}
#pragma warning( pop )

bool GOAP::Action_ClearMedkit::Execute(Elite::Blackboard* pBlackboard)
{
    if (m_pInterface->Item_Destroy(*m_pTarget))
    {
        // Remove item from general entity list
        std::vector<EntityInfo>* pEntities;
        pBlackboard->GetData("Entities", pEntities);
        pEntities->erase(std::find(pEntities->begin(), pEntities->end(), *m_pTarget));

        m_pEntities->erase(std::find(m_pEntities->begin(), m_pEntities->end(), *m_pTarget));
        return m_MedkitCleared = true;
    }

    // Couldn't destroy item, abort action
    return false;
}