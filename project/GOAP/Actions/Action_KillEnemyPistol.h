#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_KillEnemyPistol final : public BaseAction
	{
	public:
		Action_KillEnemyPistol();
		~Action_KillEnemyPistol() { SAFE_DELETE(m_pSteering); }

		virtual bool IsDone() override;
		virtual void Reset() override;
		virtual bool RequiresInRange() override { return false; }

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 1.f };
		const float m_ShotDelay{ 0.01f };
		float m_LastShotTime;
		float m_FrameTime;

		bool m_EnemyKilled;
		UINT m_InventorySlot;
		SteeringPlugin_Output* m_pSteering;
		std::vector<EnemyInfo> m_Enemies;
	};
}