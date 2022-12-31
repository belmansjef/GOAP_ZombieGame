#include "BaseAction.h"
#include "WorldState.h"
#include "IExamInterface.h"

#include <iostream>


GOAP::BaseAction::BaseAction(const std::string& _name, const int _cost, float actionTimeout)
    : m_Name(_name)
    , m_Cost(_cost)
    , m_Target(Elite::Vector2{})
    , m_AgentInfo(AgentInfo{})
    , m_pInterface(nullptr)
    , m_ActionTimeout(actionTimeout)
    , m_ActionTimer(0.f)
    , m_pWorldState(nullptr)
    , m_FrameTime(0.f)
{
}

GOAP::BaseAction::~BaseAction()
{
}

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
        } catch (const std::out_of_range&)
        {
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