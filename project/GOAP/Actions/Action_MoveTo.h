#pragma once
#include "../Action.h"

namespace GOAP
{
	class Action_MoveTo final : public Action
	{
	public:
		Action_MoveTo() = default;

		bool IsValid(Elite::Blackboard* pBlackboard)
		{
			return 
				pBlackboard->GetData("Target", m_Target)		&&
				pBlackboard->GetData("AgentInfo", m_AgentInfo)	&&
				pBlackboard->GetData("Steering", m_Steering);
		}
		virtual bool Execute(Elite::Blackboard* pBlackboard);

	private:
		SteeringPlugin_Output m_Steering;
	};
}