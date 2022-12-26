#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Data/EBlackboard.h"
#include "Exam_HelperStructs.h"

namespace GOAP
{
	struct WorldState;

	class BaseAction
	{
	public:
		BaseAction() = default;
		BaseAction(const std::string& _name, const int _cost);
		virtual ~BaseAction();

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

		virtual bool IsValid(Elite::Blackboard* m_pBlackboard) { return true; }
		virtual bool Execute(Elite::Blackboard* m_pBlackboard) { return true; }

		virtual void SetPrecondition(const std::string& key, const bool value) { m_Preconditions[key] = value; }
		virtual void SetEffect(const std::string& key, const bool value) { m_Effects[key] = value; }
		virtual void SetActionTimeout(float timeout){ m_ActionTimeout = timeout; }

		virtual int GetCost() const { return m_Cost; }
		std::string GetName() const { return m_Name; }
		bool GetIsDone() const { return m_IsDone; }

	protected:
		bool m_IsRunning{false};
		bool m_IsDone{false};
		std::string m_Name;	// Human name of the action
		int m_Cost;			// The base cost of this action
		Elite::Vector2 m_Target;
		AgentInfo m_AgentInfo;
		

		// Preconditions are predicates that must be satisfied
		// before this action can be taken.
		std::unordered_map<std::string, bool> m_Preconditions;

		// Effects are the result of this action being taken.
		// They most likely change certain vars in the current WorldState.
		std::unordered_map<std::string, bool> m_Effects;

		float m_ActionTimeout;
		float m_ActionTimer;
	};
}