#include "BaseAction.h"
#include "WorldState.h"
#include "IExamInterface.h"

#include <iostream>


GOAP::BaseAction::BaseAction(const std::string& _name, const int _cost)
    : m_Name(_name)
    , m_Cost(_cost)
    , m_InRange(false)
    , m_pTarget(new EntityInfo())
{
}

GOAP::BaseAction::~BaseAction()
{
}

/// <summary>
/// Checks if this action can fullfill any of the WorldState goals
/// </summary>
/// <param name="ws">The WorldState to check if we satisfy</param>
/// <returns></returns>
bool GOAP::BaseAction::OperableOn(const WorldState& ws) const
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
        // We have more preconditions then the goal WorldState has goals
        catch (const std::out_of_range&)
        {
            std::cout << "Goal WorldState has less goals than action [" << m_Name << "] has preconditions!" << std::endl;
            return false;
        }
    }
    return true;
}

GOAP::WorldState GOAP::BaseAction::ActOn(const WorldState& ws) const
{
    GOAP::WorldState tmp{ ws };
    for (const auto& effect : m_Effects)
    {
        tmp.SetVariable(effect.first, effect.second);
    }
    return tmp;
}