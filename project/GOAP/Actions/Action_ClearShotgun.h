#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_ClearShotgun final : public BaseAction
	{
	public:
		Action_ClearShotgun();

		virtual bool IsDone() override;
		virtual void Reset() override;
		virtual int GetCost() const override;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_ShotgunCleared;
	};
}

