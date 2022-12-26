#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"

// GOAP includes
#include "GOAP/WorldState.h"
#include "GOAP/Action.h"
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
	std::vector<HouseInfo> GetHousesInFOV() const;
	std::vector<EntityInfo> GetEntitiesInFOV() const;

	Elite::Vector2 m_Target = {};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose

	// World Info
	UINT m_InventorySlot = 0;
	UINT m_ItemsInInventory = 0;

	std::vector<HouseInfo_Extended> m_AquiredHouses;
	
	// In current FOV
	std::vector<EntityInfo> m_EntitiesInFov;
	std::vector<ItemInfo> m_ItemsInFov;
	PurgeZoneInfo m_PurgeZoneInFov;


	std::vector<EntityInfo> m_AquiredPistols;

	float m_FrameTime = 0.f;

	// States
	// Constants for the various states are helpful to keep us from
	// accidentally mistyping a state name.
	const int house_aquired = 0;
	const int inside_house = 5;
	const int health_low = 10;
	const int energy_low = 15;
	const int stamina_low = 20;
	const int food_aquired = 25;
	const int food_in_range = 30;
	const int pistol_aquired = 35;
	const int pistol_in_range = 40;
	const int pistol_in_inventory = 45;
	const int pistol_collected = 50;
	const int purge_zone_in_range = 55;

	// GOAP
	bool m_HasPlan{ false };
	GOAP::WorldState m_WorldState;
	std::vector<GOAP::Action> m_Plan;
	std::vector<GOAP::Action> m_Actions;
	std::vector<GOAP::WorldState> m_Goals;
	GOAP::Action m_WanderAction;
	GOAP::WorldState m_CurrentGoal;
	GOAP::Planner m_ASPlanner;

	bool FindPlan();

	// Steering
	float m_WanderAngle{};
	SteeringPlugin_Output m_Steering;
	bool Seek(SteeringPlugin_Output& steering, const Elite::Vector2& target, float epsilon);
	void Wander(SteeringPlugin_Output& steering);

	// Helpers
	bool CheckForPurgeZone();
	bool CheckForPistol();
	template<typename T>
	void SortEntitiesByDistance(std::vector<T>& entities);
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