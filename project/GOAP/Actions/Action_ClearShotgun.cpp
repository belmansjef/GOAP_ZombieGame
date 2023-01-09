#include "Action_ClearShotgun.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_ClearShotgun::Action_ClearShotgun()
    : BaseAction("Clear Shotgun", 5)
    , m_ShotgunCleared(false)
{
    SetPrecondition("shotgun_aquired", true);
    SetPrecondition("shotgun_limit_reached", true);
    SetEffect("ground_cleared", true);
}

bool GOAP::Action_ClearShotgun::IsDone()
{
    return m_ShotgunCleared;
}

void GOAP::Action_ClearShotgun::Reset()
{
    BaseAction::Reset();
    m_ShotgunCleared = false;
}

int GOAP::Action_ClearShotgun::GetCost() const
{
    return static_cast<int>(m_pTarget->Location.Distance(m_AgentInfo.Position));
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_ClearShotgun::IsValid(Elite::Blackboard* pBlackboard)
{
    if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
    if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
    if (!pBlackboard->GetData("Shotguns", m_pEntities) || m_pEntities == nullptr || m_pEntities->empty()) return false;

    m_AgentInfo = m_pInterface->Agent_GetInfo();
    m_pTarget = GetClosestEntity();
    return m_pTarget != nullptr;
}
#pragma warning( pop )

bool GOAP::Action_ClearShotgun::Execute(Elite::Blackboard* pBlackboard)
{
    if (m_pInterface->Item_Destroy(*m_pTarget))
    {
        // Remove item from general entity list
        std::vector<EntityInfo>* pEntities;
        pBlackboard->GetData("Entities", pEntities);
        pEntities->erase(std::find(pEntities->begin(), pEntities->end(), *m_pTarget));

        m_pEntities->erase(std::find(m_pEntities->begin(), m_pEntities->end(), *m_pTarget));
        m_pWorldState->SetVariable("item_looted", true);
        return m_ShotgunCleared = true;
    }

    // Couldn't destroy item, abort action
    return false;
}