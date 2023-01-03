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
bool GOAP::Goal_ExploreWorld::IsValid(const WorldState& ws) const
{
    return !ws.GetVariable("is_world_explored");
}
#pragma endregion // Goals