#include "WorldState.h"

GOAP::WorldState::WorldState(const std::string& _name) 
    : priority(0)
    , name(_name)
{
}

void GOAP::WorldState::SetVariable(const int var_id, const bool value)
{
    vars[var_id] = value;
}

bool GOAP::WorldState::GetVariable(const int var_id) const
{
    return vars.at(var_id);
}


bool GOAP::WorldState::operator==(const WorldState& other) const
{
    return (vars == other.vars);
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
        auto itr = vars.find(kv.first);
        if (itr == end(vars) || itr->second != kv.second)
        {
            ++result;
        }
    }

    return result;
}

float GOAP::WorldState::UpdatePriority()
{
    return exec(priority);
}
