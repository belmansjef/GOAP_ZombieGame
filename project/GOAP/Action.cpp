#include "Action.h"
#include "WorldState.h"

#include <iostream>

GOAP::Action::Action(const std::string& _name, const int _cost)
    : m_Name(_name)
    , m_Cost(_cost)
    , m_ActionTimeout(10.f)
    , m_ActionTimer(0.f)
{
}

bool GOAP::Action::OperableOn(Elite::Blackboard* pBlackboard) const
{
    GOAP::WorldState ws{};
    if (!pBlackboard->GetData("WorldState", ws)) return false;
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

GOAP::WorldState GOAP::Action::ActOn(Elite::Blackboard* pBlackboard) const
{
    GOAP::WorldState tmp{};
    if (!pBlackboard->GetData("WorldState", tmp)) return tmp;
    
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