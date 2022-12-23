#include "Action.h"
#include "WorldState.h"

#include <cassert>

GOAP::Action::Action()
    : m_Cost(0)
{
}

GOAP::Action::Action(const std::string& _name, const int _cost)
    : Action() 
{
    m_Name = _name;
    m_Cost = _cost;
}

bool GOAP::Action::OperableOn(const WorldState& ws) const
{
    for (const auto& preconditions : m_Preconditions)
    {
        try
        {
            if (ws.vars.at(preconditions.first) != preconditions.second)
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

GOAP::WorldState GOAP::Action::ActOn(const WorldState& ws) const
{
    GOAP::WorldState tmp(ws);
    for (const auto& effect : m_Effects)
    {
        tmp.SetVariable(effect.first, effect.second);
    }
    return tmp;
}