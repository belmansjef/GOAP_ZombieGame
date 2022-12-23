#pragma once

#include "Action.h"
#include "Node.h"
#include "WorldState.h"

#include <ostream>
#include <unordered_map>
#include <vector>

namespace GOAP
{
	class Planner final
	{
	public:
		Planner() = default;

		// Usefull for debugging
		void PrintOpenList() const;
		void PrintClosedList() const;

		std::vector<GOAP::Action> FormulatePlan(const WorldState& start, const WorldState& goal, const std::vector<Action>& actions);

	private:
		std::vector<Node> m_OpenList{}; // A* open list
		std::vector<Node> m_ClosedList{}; // A* closed list

		bool IsMemberOfClosed(const WorldState& ws) const;
		std::vector<GOAP::Node>::iterator IsMemberOfOpen(const WorldState& ws);

		Node& PopAndClose();
		void AddToOpenList(Node&& n);

		int CalculateHeuristic(const WorldState& now, const WorldState& goal) const;
	};
}