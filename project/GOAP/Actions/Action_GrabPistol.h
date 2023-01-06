#pragma once
#include "BaseAction.h"

class IExamInterface;

namespace GOAP
{
	class Action_GrabPistol final : public BaseAction
	{
	public:
		Action_GrabPistol();

		virtual bool IsDone() override;
		virtual void Reset() override;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_PistolGrabbed;
		UINT m_InventorySlot;
	};
}