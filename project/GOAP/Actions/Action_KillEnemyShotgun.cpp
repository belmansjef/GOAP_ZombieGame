#include "Action_KillEnemyShotgun.h"
#include "../WorldState.h"
#include "../../MathHelpers.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_KillEnemyShotgun::Action_KillEnemyShotgun()
    : BaseAction("Kill Enemy Shotgun", 2)
    , m_EnemyKilled(false)
    , m_pSteering(nullptr)
    , m_InventorySlot(1U)
    , m_FrameTime(0.f)
    , m_LastShotTime(0.f)
{
    SetPrecondition("shotgun_in_inventory", true);
    SetPrecondition("enemy_aquired", true);
    SetEffect("enemy_aquired", false);
    SetEffect("in_danger", false);
}

bool GOAP::Action_KillEnemyShotgun::IsDone()
{
    if (m_EnemyKilled)
    {
        m_pSteering->LinearVelocity = Elite::ZeroVector2;
        m_pSteering->AngularVelocity = 0.f;
        m_pWorldState->SetVariable("in_danger", false);
        m_pWorldState->SetVariable("enemy_aquired", false);
    }
    return m_EnemyKilled;
}

void GOAP::Action_KillEnemyShotgun::Reset()
{
    BaseAction::Reset();
    m_EnemyKilled = false;
}

bool GOAP::Action_KillEnemyShotgun::IsValid(Elite::Blackboard* pBlackboard)
{
    if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
    if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
    if (!pBlackboard->GetData("Steering", m_pSteering) || m_pSteering == nullptr) return false;
    if (!pBlackboard->GetData("Enemies", m_Enemies)) return false;

    return true;
}

bool GOAP::Action_KillEnemyShotgun::Execute(Elite::Blackboard* pBlackboard)
{
    if (!pBlackboard->GetData("Enemies", m_Enemies) || m_Enemies.empty()) return false;
    if (!pBlackboard->GetData("FrameTime", m_FrameTime) || m_Enemies.empty()) return false;
    if (m_Enemies.empty())
    {
        m_EnemyKilled = true;
    }

    m_LastShotTime += m_FrameTime;

    const AgentInfo& agentInfo{ m_pInterface->Agent_GetInfo() };
    const Elite::Vector2 dirVector = (m_Enemies.back().Location - agentInfo.Position).GetNormalized();
    const float desiredOrientation{ Math::WrapOrientation(Elite::VectorToOrientation(dirVector)) };
    const float currentOrientation{ Math::WrapOrientation(agentInfo.Orientation) };
    float orientationDifference{ desiredOrientation - currentOrientation };
    
    // Wrap orientation to smallest angle
    if (abs(orientationDifference) > 180.f)
    {
        orientationDifference = -Math::GetSign(orientationDifference) * (360.f - abs(orientationDifference));
    }

    m_pSteering->AutoOrient = false;
    m_pSteering->LinearVelocity = (agentInfo.Position - m_pInterface->NavMesh_GetClosestPathPoint(m_Enemies.back().Location)).GetNormalized();
    m_pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;

    // Looking at target
    if (abs(orientationDifference) <= m_AngleError)
    {
        m_pSteering->AutoOrient = true;
        m_pSteering->RunMode = false;
        m_pSteering->AngularVelocity = 0.f;

        if (m_LastShotTime < m_ShotDelay) return true;
        m_pWorldState->SetVariable("in_danger", false);

        m_LastShotTime = 0.f;
        if (!m_pInterface->Inventory_UseItem(m_InventorySlot))
        {
            m_pInterface->Inventory_RemoveItem(m_InventorySlot);
            m_pWorldState->SetVariable("shotgun_in_inventory", false);
            return false;
        }
        return true;
    }

    m_pSteering->AngularVelocity += orientationDifference;
    return true;
}