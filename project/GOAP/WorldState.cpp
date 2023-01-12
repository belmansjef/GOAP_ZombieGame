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

    // Loop through each variable in the goal WorldState we want to achieve
    for (const auto& kv : goal_state.vars)
    {
        auto it = vars.find(kv.first);
        if (it == end(vars) || it->second != kv.second)
        {
            // This WorldState does not have or doesn't match with the goal WorldState variable
            ++result;
        }
    }

    return result;
}
#pragma endregion // BaseWorldState

#pragma region Goals

bool GOAP::Goal_FleePurgezone::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("inside_purgezone");
}

bool GOAP::Goal_EliminateThreat::IsValid(const WorldState& ws) const
{
    return (ws.GetVariable("enemy_aquired") || ws.GetVariable("in_danger")) &&
        (ws.GetVariable("pistol_in_inventory") || ws.GetVariable("shotgun_in_inventory"));
}

bool GOAP::Goal_ReplenishEnergy::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("low_energy") && ws.GetVariable("food_in_inventory");
}

bool GOAP::Goal_ReplenishHealth::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("low_health") && ws.GetVariable("medkit_in_inventory");
}

bool GOAP::Goal_LootFood::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("food_aquired");
}

bool GOAP::Goal_LootMedkit::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("medkit_aquired");
}

bool GOAP::Goal_LootPistol::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("pistol_aquired");
}

bool GOAP::Goal_LootShotgun::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("shotgun_aquired");
}

bool GOAP::Goal_ClearGarbage::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("garbage_aquired");
}

bool GOAP::Goal_LootHouse::IsValid(const WorldState& ws) const
{
    return !ws.GetVariable("all_houses_looted");
}

bool GOAP::Goal_SearchArea::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("all_houses_looted") && !ws.GetVariable("all_areas_searched");
}

bool GOAP::Goal_ExploreWorld::IsValid(const WorldState& ws) const
{
    return !ws.GetVariable("is_world_explored");
}

#pragma endregion // Goals