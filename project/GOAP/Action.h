#pragma once

#include <string>
#include <unordered_map>
#include <functional>

namespace GOAP
{
	struct WorldState;

	class Action final
	{
	public:
		Action();
		Action(const std::string& _name, const int _cost, std::function<bool()> _execFunction);

		/// <summary>
		/// Can this action operate on the given WorldState?
		/// I.E. Are the preconditions met in the WorldState?
		/// </summary>
		/// <param name="ws">The WorldState to check if this action can operate on,</param>
		/// <returns>True if the action can operate on the WorldState, false otherwise,</returns>
		bool OperableOn(const WorldState& ws) const;

		/// <summary>
		/// Act on the given WorldState.
		/// DOES NOT check if the WorldState is eligible, will simply act on it.
		/// </summary>
		/// <param name="ws">The WorldState to act on.</param>
		/// <returns>A copy of the supplied WorldState, with the effects applied.</returns>
		WorldState ActOn(const WorldState& ws) const;

		bool Execute(WorldState& ws, float frameTime);

		void SetPrecondition(const int key, const bool value) { m_Preconditions[key] = value; }
		void SetEffect(const int key, const bool value) { m_Effects[key] = value; }

		void SetActionTimeout(float timeout){ m_ActionTimeout = timeout; }

		void SetCostFunction(std::function<int(int)> func) { m_CostFunction = func; }

		int GetCost() const;
		std::string GetName() const { return m_Name; }
		bool GetIsDone() const { return m_IsDone; }

	private:
		bool m_IsRunning{false};
		bool m_IsDone{false};
		std::string m_Name;	// Human name of the action
		int m_Cost;			// The base cost of this action

		// Preconditions are predicates that must be satisfied
		// before this action can be taken.
		std::unordered_map<int, bool> m_Preconditions;

		// Effects are the result of this action being taken.
		// They most likely change certain vars in the current WorldState.
		std::unordered_map<int, bool> m_Effects;
		std::function<bool()> m_ExecutionFunction;
		std::function<int(int)> m_CostFunction;

		float m_ActionTimeout;
		float m_ActionTimer;
	};
}