#include "BaseAction.h"
#include "WorldState.h"
#include "IExamInterface.h"

#include <iostream>


GOAP::BaseAction::BaseAction(const std::string& _name, const int _cost)
    : m_Name(_name)
    , m_Cost(_cost)
    , m_Target(Elite::Vector2{})
    , m_AgentInfo(AgentInfo{})
    , m_pInterface(nullptr)
    , m_ActionTimeout(20.f)
    , m_ActionTimer(0.f)
    , m_pWorldState(nullptr)
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

//bool GOAP::Action::Execute(WorldState& ws, float frameTime)
//{
//    if (!m_IsRunning)
//    {
//        std::cout << "Now executing: " << m_Name << std::endl;
//        m_IsRunning = true;
//    }
//
//    if (true)
//    {
//        if (OperableOn(ws))
//        {
//            std::cout << "Finished executing: " << m_Name << std::endl;
//            ws = ActOn(ws);
//            m_IsDone = true;
//            return true;
//        }
//        
//        return false;
//    }
//
//    m_ActionTimer += frameTime;
//    return m_ActionTimer < m_ActionTimeout;
//}