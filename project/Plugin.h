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

	UINT m_InventorySlot = 0;

	// GOAP
	UINT m_ItemsInInventory{ 0 };

	bool m_HasPlan{ false };
	GOAP::WorldState m_WorldState;
	std::vector<GOAP::Action> m_Plan;
	std::vector<GOAP::Action> m_Actions;
	GOAP::Planner m_ASPlanner;
	Elite::Vector2 m_HousePos;
	std::vector<ItemInfo> m_ItemsInView;
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