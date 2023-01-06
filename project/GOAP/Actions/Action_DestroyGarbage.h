#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_DestroyGarbage final : public BaseAction
	{
	public:
		Action_DestroyGarbage();

		virtual bool IsDone() override;
		virtual void Reset() override;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_GarbageDestoryed;
	};
}