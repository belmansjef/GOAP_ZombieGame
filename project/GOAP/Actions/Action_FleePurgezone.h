#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_FleePurgezone final : public BaseAction
	{
	public:
		Action_FleePurgezone();
		~Action_FleePurgezone() { SAFE_DELETE(m_pSteering); }


		virtual bool IsDone() override;
		virtual void Reset() override;
		virtual bool RequiresInRange() override { return false; }

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_FleedPurgezone;
		SteeringPlugin_Output* m_pSteering;
	};
}
