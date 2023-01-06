#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_SearchEnemy final : public BaseAction
	{
	public:
		Action_SearchEnemy();

		virtual bool IsDone() override;
		virtual void Reset() override;
		virtual bool RequiresInRange() override { return false; }

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_EnemyAquired;
		const float m_ActionTimeout{ 3.f };
		float m_ActionTimer;
		float m_FrameTime;
		SteeringPlugin_Output* m_pSteering;
		std::vector<EnemyInfo> m_Enemies;
	};
}