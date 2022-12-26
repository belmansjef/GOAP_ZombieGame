#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Data/EBlackboard.h"
#include "Exam_HelperStructs.h"

namespace GOAP
{
	struct WorldState;

	class Action
	{
	public:
		Action(const std::string& _name, const int _cost);
		virtual ~Action() = default;

		/// <summary>
		/// Can this action operate on the given WorldState?
		/// I.E. Are the preconditions met in the WorldState?
		/// </summary>
		/// <param name="ws">The WorldState to check if this action can operate on,</param>
		/// <returns>True if the action can operate on the WorldState, false otherwise,</returns>
		bool OperableOn(Elite::Blackboard* pBlackboard) const;

		/// <summary>
		/// Act on the given WorldState.
		/// DOES NOT check if the WorldState is eligible, will simply act on it.
		/// </summary>
		/// <param name="ws">The WorldState to act on.</param>
		/// <returns>A copy of the supplied WorldState, with the effects applied.</returns>
		WorldState ActOn(Elite::Blackboard* pBlackboard) const;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) { return true; }
		virtual bool Execute(Elite::Blackboard* pBlackboard) { return true; }

		void SetPrecondition(const int key, const bool value) { m_Preconditions[key] = value; }
		void SetEffect(const int key, const bool value) { m_Effects[key] = value; }

		void SetActionTimeout(float timeout){ m_ActionTimeout = timeout; }

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
		std::unordered_map<int, bool> m_Preconditions;

		// Effects are the result of this action being taken.
		// They most likely change certain vars in the current WorldState.
		std::unordered_map<int, bool> m_Effects;

		float m_ActionTimeout;
		float m_ActionTimer;
	};
}