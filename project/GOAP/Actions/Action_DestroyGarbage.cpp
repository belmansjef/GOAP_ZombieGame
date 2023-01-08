#include "Action_DestroyGarbage.h"
#include "../WorldState.h"

#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

GOAP::Action_DestroyGarbage::Action_DestroyGarbage()
	: BaseAction("Destroy Garbage", 5)
	, m_GarbageDestoryed(false)
{
	SetPrecondition("garbage_aquired", true);
	SetEffect("garbage_destroyed", true);
}

bool GOAP::Action_DestroyGarbage::IsDone()
{
	return m_GarbageDestoryed;
}

void GOAP::Action_DestroyGarbage::Reset()
{
	BaseAction::Reset();
	m_GarbageDestoryed = false;
}

#pragma warning( push )
#pragma warning( disable : 6011)
bool GOAP::Action_DestroyGarbage::IsValid(Elite::Blackboard* pBlackboard)
{
	if (!pBlackboard->GetData("Interface", m_pInterface) || m_pInterface == nullptr) return false;
	if (!pBlackboard->GetData("WorldState", m_pWorldState) || m_pWorldState == nullptr) return false;
	if (!pBlackboard->GetData("Garbage", m_pEntities) || m_pEntities == nullptr || m_pEntities->empty()) return false;

	m_AgentInfo = m_pInterface->Agent_GetInfo();
	m_pTarget = GetClosestEntity();
	return m_pTarget != nullptr;
}
#pragma warning( pop )

bool GOAP::Action_DestroyGarbage::Execute(Elite::Blackboard* pBlackboard)
{
	if (m_pInterface->Item_Destroy(*m_pTarget))
	{
		// Remove item from general entity list
		std::vector<EntityInfo>* pEntities;
		pBlackboard->GetData("Entities", pEntities);
		pEntities->erase(std::find(pEntities->begin(), pEntities->end(), *m_pTarget));

		m_pEntities->erase(std::find(m_pEntities->begin(), m_pEntities->end(), *m_pTarget));
		return m_GarbageDestoryed = true;
	}

	return false;
}
