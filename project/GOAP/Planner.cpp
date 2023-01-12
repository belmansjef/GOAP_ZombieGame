#include "Planner.h"

#include <algorithm>
#include <cassert>
#include <iostream>


int GOAP::Planner::CalculateHeuristic(const WorldState& now, const WorldState& goal) const
{
    return now.DistanceTo(goal);
}

void GOAP::Planner::AddToOpenList(Node&& n)
{
    // insert maintaining sort order
    auto it = std::lower_bound(begin(m_OpenList), end(m_OpenList), n);
    m_OpenList.emplace(it, std::move(n));
}

GOAP::Node& GOAP::Planner::PopAndClose()
{
    assert(!m_OpenList.empty());
    m_ClosedList.push_back(std::move(m_OpenList.front()));
    m_OpenList.erase(m_OpenList.begin());

    return m_ClosedList.back();
}

bool GOAP::Planner::IsMemberOfClosed(const WorldState& ws) const
{
    return std::find_if(begin(m_ClosedList), end(m_ClosedList), [&](const Node& n) { return n.ws == ws; }) != end(m_ClosedList);
}

std::vector<GOAP::Node>::iterator GOAP::Planner::IsMemberOfOpen(const WorldState& ws)
{
    return std::find_if(begin(m_OpenList), end(m_OpenList), [&](const Node& n) { return n.ws == ws; });
}

void GOAP::Planner::PrintOpenList() const
{
    for (const auto& n : m_OpenList)
    {
        std::cout << n << "\n";
    }
}

void GOAP::Planner::PrintClosedList() const
{
    for (const auto& n : m_ClosedList)
    {
        std::cout << n << "\n";
    }
}

std::vector<GOAP::BaseAction*> GOAP::Planner::FormulatePlan(const WorldState& start, const WorldState& goal, std::vector<BaseAction*>& actions, Elite::Blackboard* pBlackboard)
{
    if (start.MeetsGoal(goal))
    {
        std::cout << "Goal [" << goal.name << "] already meets current WorldState!" << std::endl;
        return std::vector<BaseAction*>();
    }

    // Get all actions that are valid at time of planning
    std::vector<BaseAction*> usable_actions;
    for (auto& action : actions)
    {
        action->Reset();
        if (action->IsValid(pBlackboard))
        {
            usable_actions.push_back(action);
        }
    }

    if (usable_actions.empty())
    {
        std::cout << "No usable actions to formulate plan!" << std::endl;
        return std::vector<BaseAction*>();
    }

    // We re-use the planner, so clear out the lists filled with previous results
    m_OpenList.clear();
    m_ClosedList.clear();

    // Create the first node and push it to the openlist, the list of yet to be checked actions
    Node starting_node{ start, 0, CalculateHeuristic(start, goal), 0, nullptr};
    m_OpenList.push_back(std::move(starting_node));

    while (m_OpenList.size() > 0)
    {
        // Get the node with the lowest f-cost, put it on the closed list
        // and get a reference to it
        Node& current(PopAndClose());

        // Does the current state meet the goal's state?
        if (current.ws.MeetsGoal(goal))
        {
            std::vector<BaseAction*> plan;
            do
            {
                plan.emplace_back(current.action);
                
                // Search parent node on open list
                auto it = std::find_if(begin(m_OpenList), end(m_OpenList), [&](const Node& n) { return n.id == current.parent_id; });

                // Parent node is not on the open list, search on closed list
                if (it == end(m_OpenList))
                {
                    it = std::find_if(begin(m_ClosedList), end(m_ClosedList), [&](const Node& n) { return n.id == current.parent_id; });
                }
                current = *it;
            } while (current.parent_id != 0); // Walk back through the action sequence until we get to the start node

            return plan;
        }

        // We have not met our goal state yet.
        // Check each node REACHABLE from the current node -- in other words, which action can we perform from here?
        for (const auto potential_action : usable_actions)
        {
            // This action gets us closer to our goal state, this means we can use it to formulate our plan
            if (potential_action->OperableOn(current.ws))
            {
                WorldState outcome = potential_action->ActOn(current.ws);

                // Skip if already closed -- in other words, we've already got an action that gets us here
                if (IsMemberOfClosed(outcome)) continue;

                // std::cout << "action [" << potential_action->GetName() << "] costs " << potential_action->GetCost() << std::endl;

                // Look for a Node with this WorldState on the open list.
                auto potential_outcome_node = IsMemberOfOpen(outcome);
                if (potential_outcome_node == end(m_OpenList))
                { 
                    // Not a member of open list
                    // Make a new node, with current node as its parent, recording G & H
                    Node newNode(outcome, current.g + potential_action->GetCost(), CalculateHeuristic(outcome, goal), current.id, potential_action);
                    // Add it to the open list (maintaining sort-order therein)
                    AddToOpenList(std::move(newNode));
                }
                else
                {
                    // already a member of the open list
                    // check if the current G is better than the recorded G
                    if (current.g + potential_action->GetCost() < potential_outcome_node->g)
                    {    
                        potential_outcome_node->parent_id = current.id; // make current its parent
                        potential_outcome_node->g = current.g + potential_action->GetCost(); // recalc G & H
                        potential_outcome_node->h = CalculateHeuristic(outcome, goal);
                        potential_outcome_node->action = potential_action;

                        // resort open list to account for the new f-cost
                        std::sort(begin(m_OpenList), end(m_OpenList));
                    }
                }
            }
        }
    }

    // If there's nothing left to evaluate, then we have no possible path left
    std::cout << "Could not formulate a plan!" << std::endl;
    return std::vector<BaseAction*>();
}