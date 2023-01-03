#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "GOAP/Actions.h"

#include <limits>

using namespace std;

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Research_GOAP";
	info.Student_FirstName = "Jef";
	info.Student_LastName = "Belmans";
	info.Student_Class = "2DAE15N";

	// Actions
	m_pActions.push_back(new GOAP::Action_MoveTo);
	m_pActions.push_back(new GOAP::Action_Wander);
<<<<<<< Updated upstream
=======
	m_pActions.push_back(new GOAP::Action_EatFood);
	m_pActions.push_back(new GOAP::Action_Heal);
	m_pActions.push_back(new GOAP::Action_SearchEnemy);
	m_pActions.push_back(new GOAP::Action_KillEnemy_Pistol);
	m_pActions.push_back(new GOAP::Action_KillEnemy_Shotgun);
	// m_pActions.push_back(new GOAP::Action_FleeToSafety);
	m_pActions.push_back(new GOAP::Action_FleePurgezone);
	m_pActions.push_back(new GOAP::Action_DestroyGarbage);
	m_pActions.push_back(new GOAP::Action_SearchHouse);
	m_pActions.push_back(new GOAP::Action_SearchArea);
>>>>>>> Stashed changes

	// Initial world state
	m_WorldState.SetVariable("target_in_range", false);
	m_WorldState.SetVariable("wandering", false);

	m_pBlackboard = CreateBlackboard();

	m_pGoals.push_back(new GOAP::Goal_Wander);
	m_pGoals.push_back(new GOAP::Goal_MoveToPistol);
}

//Called only once
void Plugin::DllInit()
{
}

//Called only once
void Plugin::DllShutdown()
{
	//Called wheb the plugin gets unloaded
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = false; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = false;
	params.SpawnDebugShotgun = false;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 1;
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete))
	{
		m_pInterface->RequestShutdown();
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Minus))
	{
		if (m_InventorySlot > 0)
			--m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Plus))
	{
		if (m_InventorySlot < 4)
			++m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	{
		ItemInfo info = {};
		m_pInterface->Inventory_GetItem(m_InventorySlot, info);
		std::cout << (int)info.Type << std::endl;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	m_FrameTime = dt;
	UpdateHouseInfo();
<<<<<<< Updated upstream
	GetNewEntitiesInFOV(m_AquiredEntities);

	m_pBlackboard->ChangeData("Target", m_pInterface->NavMesh_GetClosestPathPoint(m_Target));
	m_pBlackboard->ChangeData("AgentInfo", m_pInterface->Agent_GetInfo());
	m_pBlackboard->ChangeData("Entities", m_AquiredEntities);
	// m_pBlackboard->ChangeData("Interface", m_pInterface);
=======
	GetEntitiesInFOV();
	GetNewHousesInFOV();
	CheckForPurgeZone();
	m_EnemiesInFOV = GetEnemiesInFOV();
	
	AgentInfo agent{ m_pInterface->Agent_GetInfo() };
	m_WorldGrid.MarkCellAsVisited(agent.Position);
	m_pBlackboard->ChangeData("Target",				m_Target);
	m_pBlackboard->ChangeData("AgentInfo",			agent);
	m_pBlackboard->ChangeData("FrameTime",			m_FrameTime);
	m_pBlackboard->ChangeData("CellSpace",			m_WorldGrid);
	m_pBlackboard->ChangeData("EnemyEntities",		m_EnemiesInFOV);
	m_pBlackboard->ChangeData("PistolEntities",		m_PistolsInFOV);
	m_pBlackboard->ChangeData("ShotgunEntities",	m_ShotgunsInFOV);
	m_pBlackboard->ChangeData("MedEntities",		m_MedkitsInFOV);
	m_pBlackboard->ChangeData("FoodEntities",		m_FoodInFOV);
	m_pBlackboard->ChangeData("GarbageEntities",	m_GarbageInFOV);

	// WorldState
	m_WorldState.SetVariable("low_hunger",		m_pInterface->Agent_GetInfo().Energy <= 2.f);
	m_WorldState.SetVariable("low_health",		m_pInterface->Agent_GetInfo().Health <= 2.f);
	m_WorldState.SetVariable("in_danger",		m_WorldState.GetVariable("in_danger") || m_pInterface->Agent_GetInfo().WasBitten || !m_EnemiesInFOV.empty());
	m_WorldState.SetVariable("enemy_aquired",	!m_EnemiesInFOV.empty());
>>>>>>> Stashed changes

	auto goal = GetHighestPriorityGoal();
	if (m_CurrentGoal == nullptr || goal != m_CurrentGoal || empty(m_pPlan))
	{
		m_CurrentGoal = goal;
		TryFindPlan(m_WorldState, *m_CurrentGoal, m_pActions);
	}
<<<<<<< Updated upstream
	else
=======
	ExecutePlan();

	return m_steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
	m_pInterface->Draw_Polygon(&m_WorldBoundaries[0], 4, { 1.f, 0.f, 0.f });

	for (const auto& cell : m_WorldGrid.GetCells())
>>>>>>> Stashed changes
	{
		ExecutePlan();
	}

<<<<<<< Updated upstream
	//INVENTORY USAGE DEMO
	//********************

	if (m_GrabItem)
	{
		ItemInfo item;
		//Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
		//Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
		//Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
		//Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
		if (m_pInterface->Item_Grab({}, item))
		{
			//Once grabbed, you can add it to a specific inventory slot
			//Slot must be empty
			m_pInterface->Inventory_AddItem(m_InventorySlot, item);
		}
=======
	for (const auto& pistolPos : *m_pAquiredPistols)
	{
		m_pInterface->Draw_Circle(pistolPos, 125.f, { 0.f, 0.f, 1.f });
		m_pInterface->Draw_Circle(pistolPos, 2.f, { 0.f, 1.f, 0.f });
	}
	for (const auto& shotgunPos : *m_pAquiredShotguns)
	{
		m_pInterface->Draw_Circle(shotgunPos, 125.f, { 0.f, 0.f, 1.f });
		m_pInterface->Draw_Circle(shotgunPos, 2.f, { 0.f, 1.f, 0.f });
	}
	for (const auto& medkitPos : *m_pAquiredMedkits)
	{
		m_pInterface->Draw_Circle(medkitPos, 125.f, { 0.f, 0.f, 1.f });
		m_pInterface->Draw_Circle(medkitPos, 2.f, { 0.f, 1.f, 0.f });
>>>>>>> Stashed changes
	}

	if (m_UseItem)
	{
<<<<<<< Updated upstream
		//Use an item (make sure there is an item at the given inventory slot)
		m_pInterface->Inventory_UseItem(m_InventorySlot);
	}

	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		m_pInterface->Inventory_RemoveItem(m_InventorySlot);
=======
		m_pInterface->Draw_Circle(foodPos, 125.f, { 0.f, 0.f, 1.f });
		m_pInterface->Draw_Circle(foodPos, 2.f, { 0.f, 1.f, 0.f });
	}
}

void Plugin::GetNewHousesInFOV()
{
	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			auto it = std::find(m_pAquiredHouses->begin(), m_pAquiredHouses->end(), hi);
			if (it == m_pAquiredHouses->end())
			{
				m_pAquiredHouses->push_back(reinterpret_cast<HouseInfo_Extended&>(hi));
				m_pAquiredHouses->back().TimeSinceLastVisit = m_pAquiredHouses->back().ReactivationTime = 600.f;
			}
			continue;
		}
		break;
>>>>>>> Stashed changes
	}

	m_steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

	//@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return m_steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

void Plugin::GetNewEntitiesInFOV(std::vector<EntityInfo>& entities)
{
	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			// Check if we're not already aware of the entity
			auto it = std::find(begin(entities), end(entities), ei);
			if (it == end(entities))
				entities.push_back(ei);

			continue;
		}

		break;
	}
}

Elite::Blackboard* Plugin::CreateBlackboard()
{
	Elite::Blackboard* m_pBlackboard = new Elite::Blackboard();
	m_pBlackboard->AddData("WorldState", &m_WorldState);
	m_pBlackboard->AddData("AgentInfo", AgentInfo{});
	m_pBlackboard->AddData("Target", Elite::Vector2{});
	m_pBlackboard->AddData("Steering", &m_steering);
	m_pBlackboard->AddData("Entities", m_AquiredEntities);
	m_pBlackboard->AddData("Interface", m_pInterface);
	return m_pBlackboard;
}

bool Plugin::TryFindPlan(const GOAP::WorldState& ws, const GOAP::WorldState& goal, std::vector<GOAP::BaseAction*>& actions)
{
	std::cout << "Finding plan for goal [" << goal.name << "]\n";
	try
	{
		m_pPlan = m_ASPlanner.FormulatePlan(ws, goal, actions);
		return !empty(m_pPlan);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
}

bool Plugin::ExecutePlan()
{
	if (empty(m_pPlan)) return true;

	// There are still actions in the plan, execute the first action in line
	GOAP::BaseAction* currentAction = m_pPlan.back();
	if (currentAction->IsValid(m_pBlackboard) && currentAction->Execute(m_pBlackboard))
	{
		m_pPlan.pop_back();
		return empty(m_pPlan);
	}
	return false;
}

GOAP::WorldState* Plugin::GetHighestPriorityGoal()
{
	GOAP::WorldState* newGoal{};
	for (const auto goal : m_pGoals)
	{
		if (goal->IsValid(m_pBlackboard) && (newGoal == nullptr || goal->priority > newGoal->priority))
		{
			newGoal = goal;
		}
	}
	return newGoal;
}

bool Plugin::Seek(SteeringPlugin_Output& steering, const Elite::Vector2& target, float epsilon)
{
	if (m_pInterface->Agent_GetInfo().Position.DistanceSquared(target) <= powf(epsilon, 2.f))
	{
		steering.LinearVelocity = Elite::ZeroVector2;
		return true;
	}

	const Elite::Vector2 desired_location = m_pInterface->NavMesh_GetClosestPathPoint(target);
	steering.LinearVelocity = desired_location - m_pInterface->Agent_GetInfo().Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= m_pInterface->Agent_GetInfo().MaxLinearSpeed;

	steering.AutoOrient = true;

	return false;
}

void Plugin::Wander(SteeringPlugin_Output& steering)
{
	m_WanderAngle += Elite::randomFloat(-45.f, 45.f);
	const Elite::Vector2 circle_center = m_pInterface->Agent_GetInfo().Position + (m_pInterface->Agent_GetInfo().LinearVelocity.GetNormalized() * 10.f);
	const Elite::Vector2 desired_location = m_pInterface->NavMesh_GetClosestPathPoint({ cosf(m_WanderAngle) * 10.f + circle_center.x, sinf(m_WanderAngle) * 10.f + circle_center.y });

	steering.LinearVelocity = desired_location - m_pInterface->Agent_GetInfo().Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= m_pInterface->Agent_GetInfo().MaxLinearSpeed;

	steering.AutoOrient = true;
}

bool Plugin::CheckForPurgeZone()
{
<<<<<<< Updated upstream
	for (auto& e : m_EntitiesInFov)
=======
	if (m_WorldState.GetVariable("medkit_in_inventory")) return false;

	EntityInfo ei{};
	Elite::Vector2 combinedDir{Elite::ZeroVector2};
	for (int i = 0;; ++i)
>>>>>>> Stashed changes
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			m_pInterface->PurgeZone_GetInfo(e, m_PurgeZoneInFov);
			m_WorldState.SetVariable("purge_zone_in_range", true);
			return true;
		}
	}

<<<<<<< Updated upstream
=======
	if (combinedDir != Elite::ZeroVector2)
	{
		m_Target = m_PurgeZoneInFov.Center + combinedDir.GetNormalized() * (m_PurgeZoneInFov.Radius * 1.5f);
		return true;
	}
>>>>>>> Stashed changes
	return false;
}

template<typename T>
void Plugin::SortEntitiesByDistance(std::vector<T>& entities)
{
	if (empty(entities)) throw std::runtime_error("ERROR: can't run \"SortEntitiesByDistance()\" on an empty container!");

	std::sort(entities.begin(), entities.end(), [&](const T& a, const T& b)
		{
			const Elite::Vector2 agentPos{ m_pInterface->Agent_GetInfo().Position };
			const float distToA{ a.Location.DistanceSquared(agentPos) };
			const float distToB{ b.Location.DistanceSquared(agentPos) };

			return distToA > distToB;
		}
	);
}

void Plugin::UpdateHouseInfo()
{
	if (empty(m_AquiredHouses)) return;

	m_WorldState.SetVariable("house_aquired", false);
	for (auto& house : m_AquiredHouses)
	{
		house.TimeSinceLastVisit += m_FrameTime;
		if (!house.HasRecentlyVisited())
		{
			m_WorldState.SetVariable("house_aquired", true);
		}
	}
}
