#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "../Data/EBlackboard.h"
#include "Exam_HelperStructs.h"
#include "../../stdafx.h"

class IExamInterface;

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
		virtual bool OperableOn(const WorldState& ws) const;

		/// <summary>
		/// Act on the given WorldState.
		/// DOES NOT check if the WorldState is eligible, will simply act on it.
		/// </summary>
		/// <param name="ws">The WorldState to act on.</param>
		/// <returns>A copy of the supplied WorldState, with the effects applied.</returns>
		WorldState ActOn(const WorldState& ws) const;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) { return true; }
		virtual bool Execute(Elite::Blackboard* m_pBlackboard) { return true; }

		virtual void Reset() { m_pTarget = nullptr; m_InRange = false; }
		virtual bool RequiresInRange() { return true; }
		void SetInRange(bool inRange) { m_InRange = inRange; }

		virtual void SetPrecondition(const std::string& key, const bool value) { m_Preconditions[key] = value; }
		virtual void SetEffect(const std::string& key, const bool value) { m_Effects[key] = value; }

		virtual int GetCost() const { return m_Cost; }
		std::string GetName() const { return m_Name; }
		EntityInfo* GetTarget() const { return m_pTarget; }
		bool IsInRange() const { return m_InRange; }
		virtual bool IsDone() { return true; }

	protected:
		int m_Cost;
		bool m_InRange;
		std::string m_Name;
		EntityInfo* m_pTarget;
		WorldState* m_pWorldState;
		IExamInterface* m_pInterface;
		AgentInfo m_AgentInfo;
		std::vector<EntityInfo>* m_pEntities;
		
		// Preconditions are predicates that must be satisfied
		// before this action can be taken.
		std::unordered_map<std::string, bool> m_Preconditions;

		// Effects are the result of this action being taken.
		// They most likely change certain vars in the current WorldState.
		std::unordered_map<std::string, bool> m_Effects;

		EntityInfo* GetClosestEntity() const;
	};
}