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
		const float m_AngleError{ 15.f };

		ItemInfo m_TargetItem;
		WorldState* m_pWorldState;
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
		const float m_AngleError{ 15.f };

		ItemInfo m_TargetItem;
		WorldState* m_pWorldState;
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
		const float m_AngleError{ 15.f };

		ItemInfo m_TargetItem;
		WorldState* m_pWorldState;
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
		const float m_AngleError{ 15.f };

		ItemInfo m_TargetItem;
		WorldState* m_pWorldState;
		std::vector<ItemInfo>* m_pMeds;
		SteeringPlugin_Output* m_pSteering;
	};

	class Action_EatFood final : public BaseAction
	{
	public:
		Action_EatFood();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	private:
		WorldState* m_pWorldState;
	};

	class Action_Heal final : public BaseAction
	{
	public:
		Action_Heal();

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;
	private:
		WorldState* m_pWorldState;
	};
}