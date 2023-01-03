#pragma once

#include "BaseAction.h"
#include "Node.h"
#include "WorldState.h"

#include <ostream>
#include <unordered_map>
#include <vector>

class Blackboard;

namespace GOAP
{
	class Planner final
	{
	public:
		Planner() = default;

		// Usefull for debugging
		void PrintOpenList() const;
		void PrintClosedList() const;

		std::vector<GOAP::BaseAction*> FormulatePlan(const WorldState& start, const WorldState& goal, std::vector<BaseAction*>& actions, Elite::Blackboard* pBlackboard);

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