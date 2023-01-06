#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_Heal final : public BaseAction
	{
	public:
		Action_Heal();

		virtual bool IsDone() override;
		virtual void Reset() override;
		virtual bool RequiresInRange() override { return false; }

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_Healed;
		UINT m_InventorySlot;
	};
}