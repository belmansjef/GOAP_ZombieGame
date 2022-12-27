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
bool GOAP::Goal_FleePurgezone::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    return ws->GetVariable("inside_purgezone");
}

bool GOAP::Goal_CollectShotgun::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    if (ws->GetVariable("shotgun_in_inventory")) return false;

    std::vector<ItemInfo>* shotguns{ nullptr };
    if (!pBlackboard->GetData("Shotguns", shotguns) || shotguns == nullptr || shotguns->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    if (!pBlackboard->ChangeData("Target", shotguns->back().Location)) return false;
    if (!pBlackboard->ChangeData("TargetItem", shotguns->back())) return false;
    if (!pBlackboard->ChangeData("InventorySlot", 1U)) return false;

    return true;
}

bool GOAP::Goal_CollectPistol::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    if (ws->GetVariable("pistol_in_inventory")) return false;

    std::vector<ItemInfo>* pistols{nullptr};
    if (!pBlackboard->GetData("Pistols", pistols) || pistols == nullptr || pistols->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    if (!pBlackboard->ChangeData("Target", pistols->back().Location)) return false;
    if (!pBlackboard->ChangeData("TargetItem", pistols->back())) return false;
    if (!pBlackboard->ChangeData("InventorySlot", 0U)) return false;

    return true;
}

bool GOAP::Goal_CollectMedkit::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    if (ws->GetVariable("medkit_in_inventory")) return false;

    std::vector<ItemInfo>* medkits{ nullptr };
    if (!pBlackboard->GetData("Meds", medkits) || medkits == nullptr || medkits->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    if (!pBlackboard->ChangeData("Target", medkits->back().Location)) return false;
    if (!pBlackboard->ChangeData("TargetItem", medkits->back())) return false;
    if (!pBlackboard->ChangeData("InventorySlot", 2U)) return false;

    return true;
}

bool GOAP::Goal_Heal::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    return (ws->GetVariable("medkit_in_inventory") && ws->GetVariable("low_health")) || ws->GetVariable("medkit_aquired");
}

bool GOAP::Goal_CollectFood::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    if (ws->GetVariable("food_in_inventory")) return false;

    std::vector<ItemInfo>* food{ nullptr };
    if (!pBlackboard->GetData("Food", food) || food == nullptr || food->empty()) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    if (!pBlackboard->ChangeData("Target", food->back().Location)) return false;
    if (!pBlackboard->ChangeData("TargetItem", food->back())) return false;
    if (!pBlackboard->ChangeData("InventorySlot", 3U)) return false;

    return true;
}


bool GOAP::Goal_EatFood::IsValid(Elite::Blackboard* pBlackboard) const
{
    WorldState* ws;
    if (!pBlackboard->GetData("WorldState", ws)) return false;
    return (ws->GetVariable("food_in_inventory") && ws->GetVariable("low_hunger")) || ws->GetVariable("food_aquired");
}

bool GOAP::Goal_EnterHouse::IsValid(Elite::Blackboard* pBlackboard) const
{
    std::vector<HouseInfo_Extended> houses;
    if (!pBlackboard->GetData("Houses", houses) || empty(houses)) return false;

    IExamInterface* pInterface;
    if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr) return false;

    float mostRecentDiscoverTime{INFINITY};
    for (auto& house : houses)
    {
        if (!house.HasRecentlyVisited() && mostRecentDiscoverTime > house.TimeSinceLastVisit)
        {
            mostRecentDiscoverTime = house.TimeSinceLastVisit;
            if (!pBlackboard->ChangeData("Target", house.Location)) return false;
        }
    }

    return mostRecentDiscoverTime < INFINITY;
}
#pragma endregion // Goals