#include "Action_SearchArea.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_SearchArea::Action_SearchArea()
	: BaseAction("Search Area", 5)
	, m_AreaSearched(false)
	, m_pHouse(nullptr)
	, m_pHouses(nullptr)
	, m_pSteering(nullptr)
{
	SetPrecondition("all_houses_looted", true);
	SetEffect("all_areas_searched", true);
	SetEffect("house_aquired", true);
}

bool GOAP::Action_SearchArea::IsDone()
{
	if (m_AreaSearched)
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
		m_pSteering->RunMode = false;
		m_pHouse->AreaSearched = true;
		m_pWorldState->SetVariable("all_areas_searched", true);
		return true;
	}
	return false;
}

void GOAP::Action_SearchArea::Reset()
{
	SAFE_DELETE(m_pTarget);
	BaseAction::Reset();
	m_AreaSearched = false;
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_SearchArea::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Steering", m_pSteering) || m_pSteering == nullptr) return false;
	m_pSteering->RunMode = false;
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Houses", m_pHouses) || m_pHouses == nullptr || m_pHouses->empty()) return false;
	
	m_AgentInfo = m_pInterface->Agent_GetInfo();
	HouseInfo_Extended* pHouse{ nullptr };
	float closestDist{ FLT_MAX };
	for (auto& house : *m_pHouses)
	{
		const float dist{ house.Location.DistanceSquared(m_AgentInfo.Position) };
		if (!house.AreaSearched && dist < closestDist)
		{
			pHouse = &house;
			closestDist = dist;
		}
	}
	if (m_pHouse != nullptr && pHouse != nullptr && pHouse != m_pHouse && m_pHouse->HasRecentlyVisited())
	{
		m_pHouse->AreaSearched = true;
	}
	m_pHouse = pHouse;


	if (m_pHouse != nullptr)
	{
		EntityInfo* ei = new EntityInfo;
		ei->Location = GetNextCorner();
		m_pTarget = ei;
	}
	
	return m_pTarget != nullptr;
}
#pragma warning( pop )

bool GOAP::Action_SearchArea::Execute(Elite::Blackboard* pBlackboard)
{
	// Get next corner
	m_AgentInfo = m_pInterface->Agent_GetInfo();
	Elite::Vector2 nextCorner{ GetNextCorner() };

	// Move towards next corner
	m_pSteering->LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(nextCorner) - m_AgentInfo.Position).GetNormalized();
	m_pSteering->LinearVelocity *= m_AgentInfo.MaxLinearSpeed;

	if (m_AgentInfo.Stamina >= 5.f)
		m_pSteering->RunMode = true;
	else if (m_AgentInfo.Stamina == 0.f)
		m_pSteering->RunMode = false;

	// Check if we're at the next corner
	if (m_AgentInfo.Position.DistanceSquared(nextCorner) <= (m_AgentInfo.GrabRange * m_AgentInfo.GrabRange) + 20.f)
	{
		switch (m_pHouse->NextCornerAreaSearch)
		{
		case Corner::TopRight:
			m_pHouse->NextCornerAreaSearch = Corner::BottomRight;
			break;
		case Corner::BottomRight:
			m_pHouse->NextCornerAreaSearch = Corner::BottomLeft;
			break;
		case Corner::BottomLeft:
			m_pHouse->NextCornerAreaSearch = Corner::TopLeft;
			break;
		case Corner::TopLeft:
			m_pHouse->NextCornerAreaSearch = Corner::TopRight;
			m_AreaSearched = true;
			break;
		}
	}

	// Debug drawing
	m_pInterface->Draw_SolidCircle(nextCorner, 1.f, { 0.f, 0.f }, { 0.f, 1.f, 0.f });

	return true;
}

Elite::Vector2 GOAP::Action_SearchArea::GetNextCorner() const
{
	switch (m_pHouse->NextCornerAreaSearch)
	{
	case Corner::TopLeft:
		return { m_pHouse->Location.x - m_pHouse->Size.x / 2.f - 8.5f, m_pHouse->Location.y + m_pHouse->Size.y / 2.f + 8.5f };
	case Corner::TopRight:
		return { m_pHouse->Location + m_pHouse->Size / 2.f + Elite::Vector2{8.5f, 8.5f} };
	case Corner::BottomRight:
		return { m_pHouse->Location.x + m_pHouse->Size.x / 2.f + 8.5f, m_pHouse->Location.y - m_pHouse->Size.y / 2.f - 8.5f };
	case Corner::BottomLeft:
		return { m_pHouse->Location - m_pHouse->Size / 2.f - Elite::Vector2{8.5f, 8.5f} };
	}

	std::cout << "Defaulting" << std::endl;
	return m_pHouse->Location;


}