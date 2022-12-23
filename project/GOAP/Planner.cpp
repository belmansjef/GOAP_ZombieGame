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

std::vector<GOAP::Action> GOAP::Planner::FormulatePlan(const WorldState& start, const WorldState& goal, const std::vector<Action>& actions)
{
    if (start.MeetsGoal(goal))
    {
        return std::vector<Action>();
    }

    // Feasible we'd re-use a planner, so clear out the prior results
    m_OpenList.clear();
    m_ClosedList.clear();

    Node starting_node{ start, 0, CalculateHeuristic(start, goal), 0, nullptr};

    m_OpenList.push_back(std::move(starting_node));

    while (m_OpenList.size() > 0)
    {
        // Look for Node with the lowest-F-score on the open list. Switch it to closed,
        // and hang onto it -- this is our latest node.
        Node& current(PopAndClose());

        // Is our current state the goal state? If so, we've found a path, yay.
        if (current.ws.MeetsGoal(goal))
        {
            std::vector<Action> plan;
            do
            {
                plan.push_back(*current.action);

                // Search node on open list
                auto it = std::find_if(begin(m_OpenList), end(m_OpenList), [&](const Node& n) { return n.id == current.parent_id; });

                // Node is not on the open list, search on closed list
                if (it == end(m_OpenList))
                {
                    it = std::find_if(begin(m_ClosedList), end(m_ClosedList), [&](const Node& n) { return n.id == current.parent_id; });
                }
                current = *it;
            } while (current.parent_id != 0);

            return plan;
        }

        // Check each node REACHABLE from current -- in other words, where can we go from here?
        for (const auto& potential_action : actions)
        {
            if (potential_action.OperableOn(current.ws))
            {
                WorldState outcome = potential_action.ActOn(current.ws);

                // Skip if already closed
                if (IsMemberOfClosed(outcome)) continue;

                // Look for a Node with this WorldState on the open list.
                auto p_outcome_node = IsMemberOfOpen(outcome);
                if (p_outcome_node == end(m_OpenList))
                { 
                    // not a member of open list
                    // Make a new node, with current as its parent, recording G & H
                    Node newNode(outcome, current.g + potential_action.GetCost(), CalculateHeuristic(outcome, goal), current.id, &potential_action);
                    // Add it to the open list (maintaining sort-order therein)
                    AddToOpenList(std::move(newNode));
                }
                else { // already a member of the open list
                    // check if the current G is better than the recorded G
                    if (current.g + potential_action.GetCost() < p_outcome_node->g)
                    {
                        //std::cout << "My path to " << p_outcome_node->ws_ << " using " << potential_action.name() << " (combined cost " << current.g_ + potential_action.cost() << ") is better than existing (cost " <<  p_outcome_node->g_ << "\n";
                        p_outcome_node->parent_id = current.id;                  // make current its parent
                        p_outcome_node->g = current.g + potential_action.GetCost(); // recalc G & H
                        p_outcome_node->h = CalculateHeuristic(outcome, goal);
                        p_outcome_node->action = &potential_action;

                        // resort open list to account for the new F
                        // sorting likely invalidates the p_outcome_node iterator, but we don't need it anymore
                        std::sort(begin(m_OpenList), end(m_OpenList));
                    }
                }
            }
        }
    }

    // If there's nothing left to evaluate, then we have no possible path left
    throw std::runtime_error("A* planner could not find a path from start to goal");
}