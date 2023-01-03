#pragma once
#include "BaseAction.h"

namespace GOAP
{
	class Action_Wander final : public BaseAction
	{
	public:
		Action_Wander();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		float m_WanderAngle{};
	};

	class Action_MoveTo : public BaseAction
	{
	public:
		Action_MoveTo();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_FaceTarget final : public BaseAction
	{
	public:
		Action_FaceTarget();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 0.1f };
	};

	class Action_GrabPistol final : public BaseAction
	{
	public:
		Action_GrabPistol();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_GrabShotgun final : public BaseAction
	{
	public:
		Action_GrabShotgun();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_GrabMedkit final : public BaseAction
	{
	public:
		Action_GrabMedkit();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_GrabFood final : public BaseAction
	{
	public:
		Action_GrabFood();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	};

	class Action_DropPistol final : public BaseAction
	{
	public:
		Action_DropPistol();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_DropShotgun final : public BaseAction
	{
	public:
		Action_DropShotgun();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_DropMedkit final : public BaseAction
	{
	public:
		Action_DropMedkit();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_DropFood final : public BaseAction
	{
	public:
		Action_DropFood();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	};

	class Action_Heal final : public BaseAction
	{
	public:
		Action_Heal();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_EatFood final : public BaseAction
	{
	public:
		Action_EatFood();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_DestroyGarbage final : public BaseAction
	{
	public:
		Action_DestroyGarbage();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_SearchEnemy final : public BaseAction
	{
	public:
		Action_SearchEnemy();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		std::vector<EnemyInfo> m_Enemies;
		Action_FaceTarget m_ActionFaceTarget;
	};

	class Action_KillEnemy_Pistol final : public BaseAction
	{
	public:
		Action_KillEnemy_Pistol();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 0.05f };
		const float m_ShotCooldown{ 0.05f };
		float m_LastShotTimer{};

		UINT m_InventorySlot;
		std::vector<EnemyInfo> m_Enemies;
	};

	class Action_KillEnemy_Shotgun final : public BaseAction
	{
	public:
		Action_KillEnemy_Shotgun();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 0.5f };
		const float m_ShotCooldown{ 0.05f };
		float m_LastShotTimer{};

		UINT m_InventorySlot;
		std::vector<EnemyInfo> m_Enemies;
	};

	class Action_FleeToSafety final : public BaseAction
	{
	public:
		Action_FleeToSafety();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	private:
		Action_MoveTo m_ActionMoveTo;
	};

	class Action_FleePurgezone final : public BaseAction
	{
	public:
		Action_FleePurgezone();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	};

	class Action_SearchHouse final : public BaseAction
	{
	public:
		Action_SearchHouse();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	private:
		enum class Corner{ BottomLeft, BottomRight, TopLeft, TopRight};
		Corner m_NextCorner{ Corner::BottomLeft };
		Action_MoveTo m_ActionMoveTo;
		std::vector<HouseInfo_Extended>* m_pHouses;
	};

	class Action_SearchArea final : public BaseAction
	{
	public:
		Action_SearchArea();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	private:
		Action_MoveTo m_ActionMoveTo;
		std::vector<HouseInfo_Extended>* m_pHouses;
		CellSpace m_WorldGrid;
	};
}