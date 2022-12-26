#include "Action.h"
#include "WorldState.h"

#include <iostream>

GOAP::Action::Action()
    : m_Cost(0)
    , m_ActionTimeout(10.f)
    , m_ActionTimer(0.f)
{
}

GOAP::Action::Action(const std::string& _name, const int _cost, std::function<bool()> _execFunction)
    : m_Name(_name)
    , m_Cost(_cost)
    , m_ExecutionFunction(_execFunction)
    , m_ActionTimeout(10.f)
    , m_ActionTimer(0.f)
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
    return tmp;
}

bool GOAP::Action::Execute(WorldState& ws, float frameTime)
{
    if (!m_IsRunning)
    {
        std::cout << "Now executing: " << m_Name << std::endl;
        m_IsRunning = true;
    }

    if (m_ExecutionFunction())
    {
        std::cout << "Finished executing: " << m_Name << std::endl;
        if (OperableOn(ws))
        {
            ws = ActOn(ws);
            m_IsDone = true;
            return true;
        }
        
        return false;
    }

    m_ActionTimer += frameTime;
    return m_ActionTimer < m_ActionTimeout;
}

int GOAP::Action::GetCost() const
{
    if (m_CostFunction)
    {
        return m_CostFunction(m_Cost);
    }
    return m_Cost;
}
