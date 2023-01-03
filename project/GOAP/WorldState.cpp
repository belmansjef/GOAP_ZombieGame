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

    for (const auto& kv : goal_state.vars)
    {
        auto it = vars.find(kv.first);
        if (it == end(vars) || it->second != kv.second)
        {
            ++result;
        }
    }

    return result;
}
#pragma endregion // BaseWorldState

#pragma region Goals
bool GOAP::Goal_MoveToPistol::IsValid(Elite::Blackboard* pBlackboard) const
{
    std::vector<EntityInfo> pEntities;
    if (!pBlackboard->GetData("Entities", pEntities) || empty(pEntities)) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    auto it = std::find_if(begin(pEntities), end(pEntities), [&](EntityInfo entity)
        {
            ItemInfo item;
            return (pInterface->Item_GetInfo(entity, item) && item.Type == eItemType::PISTOL);
        });

    return it != end(pEntities);
}
#pragma endregion // Goals