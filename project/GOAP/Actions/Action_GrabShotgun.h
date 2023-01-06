#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_GrabShotgun final : public BaseAction
	{
	public:
		Action_GrabShotgun();

		virtual bool IsDone() override;
		virtual void Reset() override;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_ShotgunGrabbed;
		UINT m_InventorySlot;
	};
}