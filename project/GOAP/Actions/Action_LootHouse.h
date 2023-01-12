#pragma once
#include "BaseAction.h"
namespace GOAP
{
	class Action_LootHouse final : public BaseAction
	{
	public:
		Action_LootHouse();
		~Action_LootHouse() { SAFE_DELETE(m_pSteering); SAFE_DELETE(m_pHouse); SAFE_DELETE(m_pHouses); }

		virtual bool IsDone() override;
		virtual void Reset() override;
		virtual int GetCost() const override;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_HouseLooted;

		SteeringPlugin_Output* m_pSteering;
		HouseInfo_Extended* m_pHouse;
		std::vector<HouseInfo_Extended>* m_pHouses;

		Elite::Vector2 GetNextCorner() const;
	};
}

