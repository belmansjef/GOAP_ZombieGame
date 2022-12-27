#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"

// GOAP includes
#include "GOAP/WorldState.h"
#include "GOAP/BaseAction.h"
#include "GOAP/Planner.h"

class IBaseInterface;
class IExamInterface;


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
	void GetNewEntitiesInFOV();

	Elite::Vector2 m_Target = {};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose

	// World Info
	UINT m_InventorySlot = 0;
	UINT m_ItemsInInventory = 0;

	// Aquired entities
	std::vector<HouseInfo_Extended> m_AquiredHouses;
	std::vector<EntityInfo> m_AquiredEntities;
	std::vector<ItemInfo>* m_pAquiredPistols;
	std::vector<ItemInfo>* m_pAquiredShotguns;
	std::vector<ItemInfo>* m_pAquiredMedkits;
	std::vector<ItemInfo>* m_pAquiredFood;
	std::vector<ItemInfo>* m_pAquiredGarbage;
	
	// In current FOV
	std::vector<EntityInfo> m_EntitiesInFov;
	std::vector<ItemInfo> m_ItemsInFov;
	PurgeZoneInfo m_PurgeZoneInFov;

	float m_FrameTime = 0.f;

	// GOAP
	GOAP::WorldState m_WorldState;
	std::vector<GOAP::BaseAction*> m_pPlan;
	std::vector<GOAP::BaseAction*> m_pActions;
	std::vector<GOAP::WorldState*> m_pGoals;
	GOAP::WorldState* m_CurrentGoal;
	GOAP::Planner m_ASPlanner;
	
	Elite::Blackboard* m_pBlackboard;
	Elite::Blackboard* CreateBlackboard();

	bool TryFindPlan(const GOAP::WorldState& ws, const GOAP::WorldState& goal, std::vector<GOAP::BaseAction*>& actions);
	bool ExecutePlan();
	GOAP::WorldState* GetHighestPriorityGoal();

	// Steering
	float m_WanderAngle{};
	SteeringPlugin_Output m_steering;

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