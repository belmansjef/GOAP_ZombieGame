#pragma once
#include "BaseAction.h"

class IExamInterface;

namespace GOAP
{
	class Action_MoveTo final : public BaseAction
	{
	public:
		Action_MoveTo();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
	 	SteeringPlugin_Output* m_pSteering;
	};

	class Action_Wander final : public BaseAction
	{
	public:
		Action_Wander();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		float m_WanderAngle{};
		SteeringPlugin_Output* m_pSteering;
		IExamInterface* m_pInterface;
	};
}