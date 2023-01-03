#include "WorldState.h"
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

#pragma region BaseWorldState
GOAP::WorldState::WorldState(const std::string& _name, int _priority)
    : priority(_priority)
    , name(_name)
{
}

void GOAP::WorldState::SetVariable(const std::string& var_id, const bool value)
{
    vars[var_id] = value;
}

bool GOAP::WorldState::GetVariable(const std::string& var_id) const
{
    return vars.at(var_id);
}

bool GOAP::WorldState::operator==(const WorldState& other) const
{
    return vars == other.vars;
}

bool GOAP::WorldState::operator!=(const WorldState& other) const
{
    return !(vars == other.vars);
}

bool GOAP::WorldState::MeetsGoal(const WorldState& goal_state) const
{
    for (const auto& kv : goal_state.vars)
    {
        try
        {
            if (vars.at(kv.first) != kv.second)
            {
                return false;
            }
        } 
        catch (const std::out_of_range&)
        {
            return false;
        }
    }
    return true;
}

int GOAP::WorldState::DistanceTo(const WorldState& goal_state) const
{
    int result{};

    for (const auto& kv : goal_state.vars)
    {
        auto it = vars.find(kv.first);
        if (it == end(vars) || it->second != kv.second)
        {
            ++result;
        }
    }

    return result;
}
#pragma endregion // BaseWorldState

#pragma region Goals
bool GOAP::Goal_MoveToPistol::IsValid(Elite::Blackboard* pBlackboard) const
{
    std::vector<EntityInfo> pEntities;
    if (!pBlackboard->GetData("Entities", pEntities) || empty(pEntities)) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

<<<<<<< Updated upstream
    auto it = std::find_if(begin(pEntities), end(pEntities), [&](EntityInfo entity)
        {
            ItemInfo item;
            return (pInterface->Item_GetInfo(entity, item) && item.Type == eItemType::PISTOL);
        });
=======
    return true;
}

bool GOAP::Goal_Heal::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    return (ws->GetVariable("medkit_in_inventory") && ws->GetVariable("low_health"));
}

bool GOAP::Goal_EatFood::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    return (ws->GetVariable("food_in_inventory") && ws->GetVariable("low_hunger"));
}

bool GOAP::Goal_EliminateThreat::IsValid(Elite::Blackboard* pBlackboard) const
{
    std::vector<EnemyInfo> enemiesInFOV;
    if (!pBlackboard->GetData("EnemyEntities", enemiesInFOV)) return false;

    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;

    if (!ws->GetVariable("pistol_in_inventory") && !ws->GetVariable("shotgun_in_inventory")) return false;

    if (ws->GetVariable("enemy_aquired"))
    {
        pBlackboard->ChangeData("Target", enemiesInFOV.back().Location);
        return true;
    }
    else if (ws->GetVariable("in_danger"))
    {
        AgentInfo agentInfo;
        pBlackboard->GetData("AgentInfo", agentInfo);
        pBlackboard->ChangeData("Target", agentInfo.Position - agentInfo.LinearVelocity);
        return true;
    }

    return false;
}

bool GOAP::Goal_SearchHouse::IsValid(Elite::Blackboard* pBlackboard) const
{
    std::vector<HouseInfo_Extended>* houses;
    if (!pBlackboard->GetData("Houses", houses) || houses->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    AgentInfo agentInfo;
    if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;
    
    for (const auto& house : *houses)
    {
        if (!house.HasRecentlyVisited()) return true;
    }
    return false;
}

bool GOAP::Goal_ExploreWorld::IsValid(Elite::Blackboard* pBlackboard) const
{
    CellSpace worldGrid;
    if (!pBlackboard->GetData("CellSpace", worldGrid)) return false;

    AgentInfo agentInfo;
    if (!pBlackboard->GetData("AgentInfo", agentInfo)) return false;
    if (!pBlackboard->ChangeData("Target", worldGrid.GetNextCellExpandingSquare(agentInfo.Position).center)) return false;
    
    return true;
}

#pragma endregion // Goals
>>>>>>> Stashed changes

    return it != end(pEntities);
}
#pragma endregion // Goals