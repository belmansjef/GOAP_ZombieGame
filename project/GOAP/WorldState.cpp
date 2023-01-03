#include "WorldState.h"
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"
#include "../SpacePartitioning.h"

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
bool GOAP::Goal_FleePurgezone::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    return ws->GetVariable("inside_purgezone");
}

bool GOAP::Goal_CollectShotgun::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    if (ws->GetVariable("shotgun_in_inventory")) return false;

    std::vector<Elite::Vector2>* shotguns{ nullptr };
    if (!pBlackboard->GetData("ShotgunPositions", shotguns) || shotguns == nullptr || shotguns->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    if (!pBlackboard->ChangeData("Target", shotguns->back())) return false;
    if (!pBlackboard->ChangeData("InventorySlot", 1U)) return false;

    return true;
}

bool GOAP::Goal_CollectPistol::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    if (ws->GetVariable("pistol_in_inventory")) return false;

    std::vector<Elite::Vector2>* pistols{ nullptr };
    if (!pBlackboard->GetData("PistolPositions", pistols) || pistols == nullptr || pistols->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    if (!pBlackboard->ChangeData("Target", pistols->back())) return false;
    if (!pBlackboard->ChangeData("InventorySlot", 0U)) return false;

    return true;
}

bool GOAP::Goal_CollectMedkit::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    if (ws->GetVariable("medkit_in_inventory")) return false;

    std::vector<Elite::Vector2>* medkits{ nullptr };
    if (!pBlackboard->GetData("MedPositions", medkits) || medkits == nullptr || medkits->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    if (!pBlackboard->ChangeData("Target", medkits->back())) return false;
    if (!pBlackboard->ChangeData("InventorySlot", 2U)) return false;

    return true;
}


bool GOAP::Goal_CollectFood::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    if (ws->GetVariable("food_inventory_full")) return false;

    std::vector<Elite::Vector2>* food{ nullptr };
    if (!pBlackboard->GetData("FoodPositions", food) || food == nullptr || food->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    if (!pBlackboard->ChangeData("Target", food->back())) return false;
    if (!pBlackboard->ChangeData("InventorySlot", 3U)) return false;

    return true;
}

bool GOAP::Goal_DestroyGarbage::IsValid(Elite::Blackboard* pBlackboard) const
{
    std::vector<Elite::Vector2>* garbage{ nullptr };
    if (!pBlackboard->GetData("GarbagePositions", garbage) || garbage == nullptr || garbage->empty()) return false;
    if (!pBlackboard->ChangeData("Target", garbage->back())) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

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
    if (!pBlackboard->ChangeData("Target", worldGrid.GetNextCellExpandingSquare().center)) return false;
    
    return true;
}

#pragma endregion // Goals

bool GOAP::Goal_SearchArea::IsValid(Elite::Blackboard* pBlackboard) const
{
    return false;
}
