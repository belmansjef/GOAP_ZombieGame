#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_Eat final : public BaseAction
	{
	public:
		Action_Eat();

		virtual bool IsDone() override;
		virtual void Reset() override;
		virtual bool RequiresInRange() override { return false; }

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_Ate;
		UINT m_FirstInventorySlot, m_SecondInventorySlot;
	};
}