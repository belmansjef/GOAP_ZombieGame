#pragma once
#include "BaseAction.h"
#include "../../SpacePartitioning.h"

namespace GOAP
{
	class Action_SearchArea : public BaseAction
	{
	public:
		Action_SearchArea();

		virtual bool IsDone() override;
		virtual void Reset() override;

		virtual bool IsValid(Elite::Blackboard* pBlackboard) override;
		virtual bool Execute(Elite::Blackboard* pBlackboard) override;

	private:
		bool m_AreaSearched;
		SteeringPlugin_Output* m_pSteering;
		HouseInfo_Extended* m_pHouse;
		std::vector<HouseInfo_Extended>* m_pHouses;

		Elite::Vector2 GetNextCorner() const;
	};
}