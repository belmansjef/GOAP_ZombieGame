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
			out << "[" << kvp.first << " = " << kvp.second << "], ";
		}
		out << "}";
		return out;
	}


	struct Goal_FleePurgezone final : WorldState
	{
	public:
		Goal_FleePurgezone() : WorldState("Flee purgezone", 1000)
		{
			SetVariable("inside_purgezone", false);
		};

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_Heal final : WorldState
	{
	public:
		Goal_Heal() : WorldState("Heal", 950)
		{
			SetVariable("medkit_in_inventory", false);
			SetVariable("low_health", false);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_EliminateThreat final : WorldState
	{
	public:
		Goal_EliminateThreat() : WorldState("Eliminate Threat", 900)
		{
			SetVariable("in_danger", false);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_CollectShotgun final : WorldState
	{
	public:
		Goal_CollectShotgun() : WorldState("Collect Shotgun", 155)
		{
			SetVariable("shotgun_aquired", false);
			SetVariable("shotgun_in_inventory", true);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_CollectPistol final : WorldState
	{
	public:
		Goal_CollectPistol() : WorldState("Collect Pistol", 145)
		{
			SetVariable("pistol_aquired", false);
			SetVariable("pistol_in_inventory", true);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_CollectMedkit final : WorldState
	{
	public:
		Goal_CollectMedkit() : WorldState("Collect Medkit", 135)
		{
			SetVariable("medkit_aquired", false);
			SetVariable("medkit_in_inventory", true);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_CollectFood final : WorldState
	{
	public:
		Goal_CollectFood() : WorldState("Collect Food", 125)
		{
			SetVariable("food_in_inventory", true);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_EatFood final : WorldState
	{
	public:
		Goal_EatFood() : WorldState("Eat Food", 120)
		{
			SetVariable("food_in_inventory", false);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_DestroyGarbage final : WorldState
	{
	public:
		Goal_DestroyGarbage() : WorldState("Destroy Garbage", 115)
		{
			SetVariable("garbage_destroyed",	true);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};

	struct Goal_EnterHouse final : WorldState
	{
	public:
		Goal_EnterHouse() : WorldState("Enter House", 100)
		{
			SetVariable("target_in_range", true);
		}

		virtual bool IsValid(Elite::Blackboard* pBlackboard) const override;
	};


	struct Goal_Wander final : WorldState
	{
	public:
		Goal_Wander() : WorldState("Wander", 0)
		{
			SetVariable("wandering", true);
		};
	};
}