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
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_MoveTo final : public BaseAction
	{
	public:
		Action_MoveTo();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
	 	SteeringPlugin_Output* m_pSteering;
	};

	class Action_GrabFood final : public BaseAction
	{
	public:
		Action_GrabFood();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 0.5f };

		ItemInfo m_TargetItem;
		std::vector<ItemInfo>* m_pFood;
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_GrabPistol final : public BaseAction
	{
	public:
		Action_GrabPistol();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 0.5f };

		ItemInfo m_TargetItem;
		std::vector<ItemInfo>* m_pPistols;
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_GrabShotgun final : public BaseAction
	{
	public:
		Action_GrabShotgun();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 0.5f };

		ItemInfo m_TargetItem;
		std::vector<ItemInfo>* m_pShotguns;
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_GrabMedkit final : public BaseAction
	{
	public:
		Action_GrabMedkit();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 0.5f };

		ItemInfo m_TargetItem;
		std::vector<ItemInfo>* m_pMeds;
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_EatFood final : public BaseAction
	{
	public:
		Action_EatFood();

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

	class Action_SearchEnemy final : public BaseAction
	{
	public:
		Action_SearchEnemy();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		std::vector<EnemyInfo*> m_pEnemies;
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_KillEnemy_Pistol final : public BaseAction
	{
	public:
		Action_KillEnemy_Pistol();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 0.05f };

		UINT m_InventorySlot;
		std::vector<EnemyInfo*> m_pEnemies;
		SteeringPlugin_Output* m_pSteering;
		
	};

	class Action_KillEnemy_Shotgun final : public BaseAction
	{
	public:
		Action_KillEnemy_Shotgun();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		const float m_AngleError{ 2.f };

		UINT m_InventorySlot;
		std::vector<EnemyInfo*> m_pEnemies;
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_FleeToSafety final : public BaseAction
	{
	public:
		Action_FleeToSafety();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		SteeringPlugin_Output* m_pSteering;
		std::vector<HouseInfo_Extended>* m_pHouses;
	};
}