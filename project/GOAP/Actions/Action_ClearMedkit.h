#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_ClearMedkit final : public BaseAction
	{
	public:
		Action_ClearMedkit();

		virtual bool IsDone() override;
		virtual void Reset() override;
		virtual int GetCost() const override;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_MedkitCleared;
	};
}

