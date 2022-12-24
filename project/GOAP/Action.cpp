#include "Action.h"
#include "WorldState.h"

#include <iostream>

GOAP::Action::Action()
    : m_Cost(0)
{
}

GOAP::Action::Action(const std::string& _name, const int _cost, std::function<bool(IExamInterface* pInterface, SteeringPlugin_Output&)> _function)
    : m_Name(_name)
    , m_Cost(_cost)
    , m_Exec(_function)
{
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
    for (const auto& effect : m_ProceduralEffect)
    {
        tmp.SetVariable(effect.first, effect.second());
    }
    return tmp;
}

bool GOAP::Action::Execute(WorldState& ws, IExamInterface* pInterface, SteeringPlugin_Output& steering)
{
    if (!m_IsRunning)
    {
        std::cout << "Now executing: " << m_Name << std::endl;
        m_IsRunning = true;
    }

    if (m_Exec(pInterface, steering))
    {
        std::cout << "Finished executing: " << m_Name << std::endl;
        ws = ActOn(ws);
        return true;
    }
    
    return false;
}