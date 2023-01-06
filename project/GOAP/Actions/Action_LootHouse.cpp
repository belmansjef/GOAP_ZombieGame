#include "Action_LootHouse.h"

#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_LootHouse::Action_LootHouse()
	: BaseAction("Loot House", 10)
	, m_HouseLooted(false)
	, m_pHouse(nullptr)
	, m_pHouses(nullptr)
	, m_pSteering(nullptr)
{
	SetPrecondition("house_aquired", true);
	SetEffect("house_looted", true);
}

bool GOAP::Action_LootHouse::IsDone()
{
	if (m_HouseLooted)
	{
		m_pHouse->TimeSinceLastVisit = 0.f;
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		return true;
	}
	return false;
}

void GOAP::Action_LootHouse::Reset()
{
	SAFE_DELETE(m_pTarget);
	BaseAction::Reset();
	m_HouseLooted = false;
	m_pHouse = nullptr;
}


#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_LootHouse::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Steering", m_pSteering) || m_pSteering == nullptr) return false;
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Houses", m_pHouses) || m_pHouses == nullptr || m_pHouses->empty()) return false;

	AgentInfo agentInfo{ m_pInterface->Agent_GetInfo() };
	float closestDist{ FLT_MAX };
	for (auto& house : *m_pHouses)
	{
		const float dist{ house.Location.DistanceSquared(agentInfo.Position) };
		if (!house.HasRecentlyVisited() && dist < closestDist)
		{
			m_pHouse = &house;
			closestDist = dist;
		}
	}

	if (m_pHouse != nullptr)
	{
		EntityInfo* ei = new EntityInfo;
		ei->Location = GetNextCorner();
		m_pTarget = ei;
	}

	return m_pTarget != nullptr;
}
#pragma warning( pop ) 

bool GOAP::Action_LootHouse::Execute(Elite::Blackboard* pBlackboard)
{
	// Get next corner
	const AgentInfo agentInfo{ m_pInterface->Agent_GetInfo() };
	const Elite::Vector2 nextCorner{ GetNextCorner() };

	// Move towards next corner
	m_pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(nextCorner) - agentInfo.Position).GetNormalized();
	m_pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;

	// Check if we're at the next corner
	if (agentInfo.Position.DistanceSquared(nextCorner) <= agentInfo.GrabRange * agentInfo.GrabRange)
	{
		switch (m_pHouse->NextCornerHouseSearch)
		{
		case Corner::BottomLeft:
			m_pHouse->NextCornerHouseSearch = Corner::TopLeft;
			break;
		case Corner::TopLeft:
			m_pHouse->NextCornerHouseSearch = Corner::BottomRight;
			if (m_pHouse->Size.y > 40.f && m_pHouse->Size.x < 20.f)
				m_HouseLooted = true;
			break;
		case Corner::BottomRight:
			m_pHouse->NextCornerHouseSearch = Corner::TopRight;
			break;
		case Corner::TopRight:
			m_pHouse->NextCornerHouseSearch = Corner::TopLeft;
			m_HouseLooted = true;
			break;
		}
	}

	// Debug drawing
	m_pInterface->Draw_SolidCircle(nextCorner, 1.f, { 0.f, 0.f }, { 0.f, 1.f, 0.f });
	
	return true;
}

Elite::Vector2 GOAP::Action_LootHouse::GetNextCorner() const
{	
	std::vector<Elite::Vector2> corners{ m_pHouse->GetCorners() };
	switch (m_pHouse->NextCornerHouseSearch)
	{
	case Corner::BottomLeft:
		return corners[0];
	case Corner::TopLeft:
		return corners[1];
	case Corner::BottomRight:
		return corners[3];
	case Corner::TopRight:
		return corners[2];
	}

	return corners[0];
}
