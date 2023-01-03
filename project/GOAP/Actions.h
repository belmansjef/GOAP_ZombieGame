#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_GotoClosestCell final : public BaseAction
	{
	public:
		Action_GotoClosestCell();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
		virtual bool IsDone() override;
	};
}