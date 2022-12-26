#pragma once
#include <ostream>
#include <string>
#include <map>
#include <functional>

#include "Data/EBlackboard.h"

namespace GOAP
{
	struct WorldState
	{
	public:
		WorldState(const std::string& _name = "", int _priority = 100);

		void SetVariable(const std::string& var_id, const bool value);
		bool GetVariable(const std::string& var_id) const;

		/// <summary>
		/// Does the passed WorldState meet the requirements of this WorldState?
		/// </summary>
		/// <param name="goal_state">The other WorldState to test if it's matched this goal</param>
		/// <returns>True if it meets this goal's state, false otherwise</returns>
		bool MeetsGoal(const WorldState& goal_state) const;

		/// <summary>
		/// How many of our state variables differ from the other WorldState?
		/// Used to get the Heuristic for A*
		/// </summary>
		/// <param name="goal_state">The WorldState to get the distance to</param>
		/// <returns>The number of state var difference between us and them</returns>
		int DistanceTo(const WorldState& goal_state) const;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const { return true; }

		bool operator==(const WorldState& other) const;
		bool operator!=(const WorldState& other) const;

		friend std::ostream& operator<<(std::ostream& out, const WorldState& ws);

		int priority; // Used if this is a goal state, the highest priorty goal will be used
		std::string name; // Human name of the state
		std::map<std::string, bool> vars; // Describes the world state and it's paramaters
	};

	inline std::ostream& operator<<(std::ostream& out, const WorldState& ws)
	{
		out << "WorldState { ";
		for (const auto& kvp : ws.vars)
		{
			out << kvp.second << " ";
		}
		out << "}";
		return out;
	}

	struct Goal_Wander final : WorldState
	{
	public:
		Goal_Wander() : WorldState("Wander", 0)
		{
			SetVariable("wandering", true);
		};
	};

	struct Goal_MoveToPistol final : WorldState
	{
	public:
		Goal_MoveToPistol() : WorldState("Move To Pistol", 100)
		{
			SetVariable("target_in_range", true);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};
}