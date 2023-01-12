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
	SetEffect("pistol_aquired", true);
	SetEffect("pistol_in_inventory", true);
	SetEffect("shotgun_aquired", true);
	SetEffect("shotgun_in_inventory", true);
	SetEffect("medkit_aquired", true);
	SetEffect("medkit_in_inventory", true);
	SetEffect("food_aquired", true);
	SetEffect("food_in_inventory", true);
	SetEffect("garbage_aquired", true);
}

bool GOAP::Action_LootHouse::IsDone()
{
	if (m_HouseLooted)
	{
		m_pHouse->itemsLootedSinceLastVisit = 0;
		m_pHouse->NextCornerHouseSearch = Corner::BottomLeft;
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
	}

	return m_HouseLooted;
}

void GOAP::Action_LootHouse::Reset()
{
	SAFE_DELETE(m_pTarget);
	BaseAction::Reset();
	m_HouseLooted = false;
	m_pHouse = nullptr;
}

int GOAP::Action_LootHouse::GetCost() const
{
	// Magic number +10 to give preference to grabbing an item inside the house instead of continueing to search the house
	return static_cast<int>(m_pHouse->Location.Distance(m_AgentInfo.Position)) + 10;
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_LootHouse::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Steering", m_pSteering) || m_pSteering == nullptr) return false;
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Houses", m_pHouses) || m_pHouses == nullptr || m_pHouses->empty()) return false;

	m_AgentInfo = m_pInterface->Agent_GetInfo();
	float closestDist{ FLT_MAX };
	for (auto& house : *m_pHouses)
	{
		const float dist{ house.Location.DistanceSquared(m_AgentInfo.Position) };
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
		std::cout << "House size: " << m_pHouse->Size << std::endl;
	}

	return m_pTarget != nullptr;
}
#pragma warning( pop ) 

bool GOAP::Action_LootHouse::Execute(Elite::Blackboard* pBlackboard)
{
	// Get next corner
	m_AgentInfo = m_pInterface->Agent_GetInfo();
	const Elite::Vector2 nextCorner{ GetNextCorner() };

	// Move towards next corner
	m_pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(nextCorner) - m_AgentInfo.Position).GetNormalized();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	// Check if we're at the next corner
	if (m_AgentInfo.Position.DistanceSquared(nextCorner) <= m_AgentInfo.GrabRange * m_AgentInfo.GrabRange)
	{
		switch (m_pHouse->NextCornerHouseSearch)
		{
		case Corner::BottomLeft:
			m_pHouse->NextCornerHouseSearch = Corner::TopLeft;
			break;
		case Corner::TopLeft:
			if (m_pHouse->Size.x < 40.f || m_pHouse->Size.y < 40.f)
			{
				m_pHouse->NextCornerHouseSearch = Corner::BottomLeft;
				m_HouseLooted = true;
				return true;
			}
			m_pHouse->NextCornerHouseSearch = Corner::BottomRight;
			break;
		case Corner::BottomRight:
			m_pHouse->NextCornerHouseSearch = Corner::TopRight;
			break;
		case Corner::TopRight:
			m_pHouse->NextCornerHouseSearch = Corner::BottomLeft;
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
	const std::vector<Elite::Vector2> corners{ m_pHouse->GetCorners() };
	const std::vector<Elite::Vector2> midPoints{ m_pHouse->GetMidPoints() };

	if (m_pHouse->Size.x >= 40.f && m_pHouse->Size.y >= 40.f)
	{
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
	}
	else if(m_pHouse->Size.x < 40.f)
	{
		switch (m_pHouse->NextCornerHouseSearch)
		{
		case Corner::BottomLeft:
			return midPoints[1];
		case Corner::TopLeft:
			return midPoints[3];
		}
	}
	else if (m_pHouse->Size.y < 40.f)
	{
		switch (m_pHouse->NextCornerHouseSearch)
		{
		case Corner::BottomLeft:
			return midPoints[0];
		case Corner::TopLeft:
			return midPoints[2];
		}
	}

	return corners[0];
}