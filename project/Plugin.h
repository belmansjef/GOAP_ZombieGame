#pragma once
#include <queue>

#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"

// GOAP includes
#include "GOAP/WorldState.h"
#include "GOAP/Planner.h"
#include "FSM.h"

// Space Partitioning
#include "SpacePartitioning.h"

class IBaseInterface;
class IExamInterface;
class BaseAction;

class Plugin final : public IExamPlugin
{
public:
	Plugin() {};
	virtual ~Plugin() {};

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;
	void GetNewHousesInFOV();
	void GetEntitiesInFOV();
	std::vector<EnemyInfo> GetEnemiesInFOV();

	Elite::Vector2 m_Target = {};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose

	// World Info
	const size_t m_GroundItemLimit{ 4 };
	Elite::Vector2 m_WorldCenter;
	Elite::Vector2 m_WorldDimensions;
	std::vector<Elite::Vector2> m_WorldBoundaries;

	// Aquired entities
	std::vector<EntityInfo>* m_pAquiredEntities;
	std::vector<EntityInfo>* m_pAquiredPistols;
	std::vector<EntityInfo>* m_pAquiredShotguns;
	std::vector<EntityInfo>* m_pAquiredMedkits;
	std::vector<EntityInfo>* m_pAquiredFood;
	std::vector<EntityInfo>* m_pAquiredGarbage;
	std::vector<HouseInfo_Extended>* m_pAquiredHouses;
	
	// In current FOV
	PurgeZoneInfo m_PurgeZoneInFOV;
	std::vector<EnemyInfo> m_EnemiesInFOV;

	float m_FrameTime = 0.f;

	// GOAP
	GOAP::WorldState m_WorldState;
	std::vector<GOAP::BaseAction*> m_pPlan;
	std::vector<GOAP::BaseAction*> m_pActions;
	std::vector<GOAP::WorldState*> m_pGoals;
	GOAP::WorldState* m_CurrentGoal;
	GOAP::Planner m_ASPlanner;

	GOAP::FSM m_FSM;
	GOAP::FSMState m_IdleState;
	GOAP::FSMState m_MoveToState;
	GOAP::FSMState m_ExecuteActionState;
	
	Elite::Blackboard* m_pBlackboard;
	Elite::Blackboard* CreateBlackboard();

	GOAP::WorldState* GetHighestPriorityGoal();
	bool MoveAgent(GOAP::BaseAction* pAction);
	const float m_AngleError{ 25.f };
	bool m_IsPosInsideWall{ true };
	
	// Space Partitioning
	CellSpace m_WorldGrid;

	// Steering
	SteeringPlugin_Output m_Steering;

	// Helpers
	bool CheckForPurgeZone();
	template<typename T> void SortEntitiesByDistance(std::vector<T>* entities);
	void UpdateHouseInfo();
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}