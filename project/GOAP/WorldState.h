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

		virtual bool IsValid(const WorldState& ws) const { return true; }

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
		Goal_FleePurgezone() : WorldState("Flee Purgezone", 1000)
		{
			SetVariable("inside_purgezone", false);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_EliminateThreat final : WorldState
	{
	public:
		Goal_EliminateThreat() : WorldState("Eliminate Threat", 110)
		{
			SetVariable("in_danger", false);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_ReplenishHealth final : WorldState
	{
	public:
		Goal_ReplenishHealth() : WorldState("Replenish Health", 100)
		{
			SetVariable("low_health", false);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_ReplenishEnergy final : WorldState
	{
	public:
		Goal_ReplenishEnergy() : WorldState("Replenish Energy", 90)
		{
			SetVariable("low_energy", false);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_LootPistol final : WorldState
	{
	public:
		Goal_LootPistol() : WorldState("Loot Pistol", 60)
		{
			SetVariable("pistol_in_inventory", true);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_LootShotgun final : WorldState
	{
	public:
		Goal_LootShotgun() : WorldState("Loot Shotgun", 50)
		{
			SetVariable("shotgun_in_inventory", true);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_LootFood final : WorldState
	{
	public:
		Goal_LootFood() : WorldState("Loot Food", 40)
		{
			SetVariable("food_inventory_full", true);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_LootMedkit final : WorldState
	{
	public:
		Goal_LootMedkit() : WorldState("Loot Medkit", 30)
		{
			SetVariable("medkit_in_inventory", true);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_ClearGarbage final : WorldState
	{
	public:
		Goal_ClearGarbage() : WorldState("Clear Garbage", 25)
		{
			SetVariable("garbage_destroyed", true);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_LootHouse final : WorldState
	{
	public:
		Goal_LootHouse() : WorldState("Loot House", 20)
		{
			SetVariable("house_looted", true);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_SearchArea final : WorldState
	{
	public:
		Goal_SearchArea() : WorldState("Search Area", 10)
		{
			SetVariable("all_areas_searched", true);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};

	struct Goal_ExploreWorld final : WorldState
	{
	public:
		Goal_ExploreWorld() : WorldState("Explore World", 0)
		{
			SetVariable("has_explored", true);
		};
		virtual bool IsValid(const WorldState& ws) const override;
	};
}