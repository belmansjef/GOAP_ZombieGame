#include "stdafx.h"
#include "Plugin.h"
#include "GOAP/Actions.h"
#include "IExamInterface.h"

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
	info.BotName =				"Research_GOAP";
	info.Student_FirstName =	"Jef";
	info.Student_LastName =		"Belmans";
	info.Student_Class =		"2DAE15N";


	// World Info
	m_WorldCenter = m_pInterface->World_GetInfo().Center;
	m_WorldDimensions = m_pInterface->World_GetInfo().Dimensions;
	m_WorldBoundaries =
	{
		{m_WorldCenter - m_WorldDimensions / 2.f},
		{m_WorldCenter.x + m_WorldDimensions.x / 2.f, m_WorldCenter.y - m_WorldDimensions.y / 2.f},
		{m_WorldCenter + m_WorldDimensions / 2.f},
		{m_WorldCenter.x - m_WorldDimensions.x / 2.f, m_WorldCenter.y + m_WorldDimensions.y / 2.f},
	};

	// Space Partitioning
	m_WorldGrid = std::move(CellSpace(m_WorldDimensions.x / 1.5f, m_WorldDimensions.y / 1.5f, 14, 14));

	// Entities
	m_pAquiredHouses		= new std::vector<HouseInfo_Extended>;
	m_pAquiredEntities		= new std::vector<EntityInfo>;
	m_pAquiredPistols		= new std::vector<Elite::Vector2>;
	m_pAquiredShotguns		= new std::vector<Elite::Vector2>;
	m_pAquiredMedkits		= new std::vector<Elite::Vector2>;
	m_pAquiredFood			= new std::vector<Elite::Vector2>;
	m_pAquiredGarbage		= new std::vector<Elite::Vector2>;

	// GOAP Actions
	m_pActions.push_back(new GOAP::Action_MoveTo);
	m_pActions.push_back(new GOAP::Action_FaceTarget);
	m_pActions.push_back(new GOAP::Action_GrabPistol);
	m_pActions.push_back(new GOAP::Action_GrabShotgun);
	m_pActions.push_back(new GOAP::Action_GrabMedkit);
	m_pActions.push_back(new GOAP::Action_GrabFood);
	m_pActions.push_back(new GOAP::Action_DropPistol);
	m_pActions.push_back(new GOAP::Action_DropShotgun);
	m_pActions.push_back(new GOAP::Action_Wander);
	m_pActions.push_back(new GOAP::Action_EatFood);
	m_pActions.push_back(new GOAP::Action_Heal);
	m_pActions.push_back(new GOAP::Action_SearchEnemy);
	m_pActions.push_back(new GOAP::Action_KillEnemy_Pistol);
	// m_pActions.push_back(new GOAP::Action_KillEnemy_Shotgun);
	m_pActions.push_back(new GOAP::Action_FleeToSafety);
	m_pActions.push_back(new GOAP::Action_FleePurgezone);
	m_pActions.push_back(new GOAP::Action_DestroyGarbage);
	m_pActions.push_back(new GOAP::Action_SearchHouse);
	m_pActions.push_back(new GOAP::Action_SearchArea);

	// Initial world state
	m_WorldState.SetVariable("target_in_range",		false);
	m_WorldState.SetVariable("item_collected",		false);
	m_WorldState.SetVariable("inside_purgezone",	false);
	m_WorldState.SetVariable("pistol_aquired",		false);
	m_WorldState.SetVariable("pistol_nearby",		false);
	m_WorldState.SetVariable("pistol_in_inventory", false);
	m_WorldState.SetVariable("shotgun_aquired",		false);
	m_WorldState.SetVariable("shotgun_nearby",		false);
	m_WorldState.SetVariable("shotgun_in_inventory",false);
	m_WorldState.SetVariable("medkit_aquired",		false);
	m_WorldState.SetVariable("medkit_nearby",		false);
	m_WorldState.SetVariable("medkit_in_inventory", false);
	m_WorldState.SetVariable("low_health",			false);
	m_WorldState.SetVariable("food_aquired",		false);
	m_WorldState.SetVariable("food_nearby",			false);
	m_WorldState.SetVariable("food_in_inventory",	false);
	m_WorldState.SetVariable("food_inventory_full",	false);
	m_WorldState.SetVariable("low_hunger",			false);
	m_WorldState.SetVariable("garbage_aquired",		false);
	m_WorldState.SetVariable("garbage_destroyed",	false);
	m_WorldState.SetVariable("enemy_aquired",		false);
	m_WorldState.SetVariable("in_danger",			false);
	m_WorldState.SetVariable("house_aquired",		false);
	m_WorldState.SetVariable("wandering",			false);

	m_pBlackboard = CreateBlackboard();

	m_pGoals.push_back(new GOAP::Goal_FleePurgezone);
	m_pGoals.push_back(new GOAP::Goal_CollectPistol);
	m_pGoals.push_back(new GOAP::Goal_CollectShotgun);
	m_pGoals.push_back(new GOAP::Goal_CollectMedkit);
	m_pGoals.push_back(new GOAP::Goal_CollectFood);
	m_pGoals.push_back(new GOAP::Goal_EatFood);
	m_pGoals.push_back(new GOAP::Goal_Heal);
	m_pGoals.push_back(new GOAP::Goal_SearchHouse);
	m_pGoals.push_back(new GOAP::Goal_EliminateThreat);
	m_pGoals.push_back(new GOAP::Goal_Wander);
	m_pGoals.push_back(new GOAP::Goal_ExploreWorld);
	m_pGoals.push_back(new GOAP::Goal_DestroyGarbage);
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
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = true; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = false;
	params.SpawnDebugShotgun = false;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 64;
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
	m_WorldState.SetVariable("low_health",		m_pInterface->Agent_GetInfo().Health <= 8.f);
	m_WorldState.SetVariable("in_danger",		m_WorldState.GetVariable("in_danger") || m_pInterface->Agent_GetInfo().WasBitten || !m_EnemiesInFOV.empty());
	m_WorldState.SetVariable("enemy_aquired",	!m_EnemiesInFOV.empty());

	auto goal = GetHighestPriorityGoal();
	if (m_CurrentGoal == nullptr || goal != m_CurrentGoal || empty(m_pPlan))
	{
		m_CurrentGoal = goal;
		TryFindPlan(m_WorldState, *m_CurrentGoal, m_pActions);
	}
	else ExecutePlan();

	return m_steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
	m_pInterface->Draw_Polygon(&m_WorldBoundaries[0], 4, { 1.f, 0.f, 0.f });

	for (const auto& cell : m_WorldGrid.GetCells())
	{
		if (cell.hasVisited)
			m_pInterface->Draw_Polygon(&cell.GetRectPoints()[0], 4, {0.f, 1.f, 0.f}, -1.f);
		else
			m_pInterface->Draw_Polygon(&cell.GetRectPoints()[0], 4, { 1.f, 0.f, 0.f }, 0.f);
	}

	for (const auto& pistolPos : *m_pAquiredPistols)
	{
		m_pInterface->Draw_Circle(pistolPos, 125.f, { 0.f, 0.f, 1.f });
	}
	for (const auto& shotgunPos : *m_pAquiredShotguns)
	{
		m_pInterface->Draw_Circle(shotgunPos, 125.f, { 0.f, 0.f, 1.f });
	}
	for (const auto& medkitPos : *m_pAquiredMedkits)
	{
		m_pInterface->Draw_Circle(medkitPos, 125.f, { 0.f, 0.f, 1.f });
	}
	for (const auto& foodPos : *m_pAquiredFood)
	{
		m_pInterface->Draw_Circle(foodPos, 125.f, { 0.f, 0.f, 1.f });
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
				m_pAquiredHouses->back().TimeSinceLastVisit = m_pAquiredHouses->back().ReactivationTime = 500.f;
			}
			continue;
		}
		break;
	}

	if (!m_pAquiredHouses->empty())
		SortEntitiesByDistance(*m_pAquiredHouses);

	m_WorldState.SetVariable("house_aquired", !m_pAquiredHouses->empty());
}

void Plugin::GetEntitiesInFOV()
{
	EntityInfo ei{};
	BYTE flags{};

	// Clear previous items
	m_PistolsInFOV.clear();
	m_ShotgunsInFOV.clear();
	m_MedkitsInFOV.clear();
	m_FoodInFOV.clear();
	m_GarbageInFOV.clear();

	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type != eEntityType::ITEM) continue;

			ItemInfo item{};
			m_pInterface->Item_GetInfo(ei, item);
			switch (item.Type)
			{
			case eItemType::PISTOL:
				m_PistolsInFOV.emplace_back(ei);
				break;
			case eItemType::SHOTGUN:
				m_ShotgunsInFOV.emplace_back(ei);
				break;
			case eItemType::MEDKIT:
				m_MedkitsInFOV.emplace_back(ei);
				break;
			case eItemType::FOOD:
				m_FoodInFOV.emplace_back(ei);
				break;
			case eItemType::GARBAGE:
				m_GarbageInFOV.emplace_back(ei);
				break;
			}

			// Check if we're not already aware of the item
			if (std::find(m_pAquiredEntities->begin(), m_pAquiredEntities->end(), ei) == m_pAquiredEntities->end())
			{
				// If we haven't spotted this item yet, add it's location to all known items
				m_pAquiredEntities->push_back(ei);
				switch (item.Type)
				{
				case eItemType::PISTOL:
					m_pAquiredPistols->emplace_back(item.Location);
					flags |= 1U << 0;
					break;
				case eItemType::SHOTGUN:
					m_pAquiredShotguns->emplace_back(item.Location);
					flags |= 1U << 1;
					break;
				case eItemType::MEDKIT:
					m_pAquiredMedkits->emplace_back(item.Location);
					flags |= 1U << 2;
					break;
				case eItemType::FOOD:
					m_pAquiredFood->emplace_back(item.Location);
					flags |= 1U << 3;
					break;
				case eItemType::GARBAGE:
					m_pAquiredGarbage->emplace_back(item.Location);
					flags |= 1U << 4;
					break;
				}
			}
			continue;
		} // If entity
		break;
	}

	auto SortVectors = [&](const Elite::Vector2& a, const Elite::Vector2& b)
	{
		return a.DistanceSquared(m_pInterface->Agent_GetInfo().Position) > b.DistanceSquared(m_pInterface->Agent_GetInfo().Position);
	};

	const Elite::Vector2 agentPos{ m_pInterface->Agent_GetInfo().Position };
	if ((flags >> 0) & 1U)
	{
		std::sort(m_pAquiredPistols->begin(), m_pAquiredPistols->end(), SortVectors);
	}
	SortEntitiesByDistance(m_PistolsInFOV);
	m_WorldState.SetVariable("pistol_aquired", !m_pAquiredPistols->empty());
	m_WorldState.SetVariable("pistol_nearby", false);
	if (m_pAquiredPistols->size() > 1)
		m_WorldState.SetVariable("pistol_nearby", m_pAquiredPistols->at(m_pAquiredPistols->size() - 1).DistanceSquared(agentPos) <= 15'625.f);

	if ((flags >> 1) & 1U)
	{
		std::sort(m_pAquiredShotguns->begin(), m_pAquiredShotguns->end(), SortVectors);
	}
	SortEntitiesByDistance(m_ShotgunsInFOV);
	m_WorldState.SetVariable("shotgun_aquired", !m_pAquiredShotguns->empty());
	m_WorldState.SetVariable("shotgun_nearby", false);
	if (m_pAquiredShotguns->size() > 1)
		m_WorldState.SetVariable("shotgun_nearby", m_pAquiredShotguns->at(m_pAquiredShotguns->size() - 1).DistanceSquared(agentPos) <= 15'625.f);

	if ((flags >> 2) & 1U)
	{
		std::sort(m_pAquiredMedkits->begin(), m_pAquiredMedkits->end(), SortVectors);
	}
	SortEntitiesByDistance(m_MedkitsInFOV);
	m_WorldState.SetVariable("medkit_aquired", !m_pAquiredMedkits->empty());
	m_WorldState.SetVariable("medkit_nearby", false);
	if (m_pAquiredMedkits->size() > 1)
		m_WorldState.SetVariable("medkit_nearby", m_pAquiredMedkits->at(m_pAquiredMedkits->size() - 1).DistanceSquared(agentPos) <= 15'625.f);

	if ((flags >> 3) & 1U)
	{
		std::sort(m_pAquiredFood->begin(), m_pAquiredFood->end(), SortVectors);
	}
	SortEntitiesByDistance(m_FoodInFOV);
	m_WorldState.SetVariable("food_aquired", !m_pAquiredFood->empty());
	m_WorldState.SetVariable("food_nearby", false);
	if (m_pAquiredFood->size() > 1)
		m_WorldState.SetVariable("food_nearby", m_pAquiredFood->at(m_pAquiredFood->size() - 1).DistanceSquared(agentPos) <= 15'625.f);

	if ((flags >> 4) & 1U)
	{
		std::sort(m_pAquiredGarbage->begin(), m_pAquiredGarbage->end(), SortVectors);
	}
	SortEntitiesByDistance(m_GarbageInFOV);
	m_WorldState.SetVariable("garbage_aquired", !m_pAquiredGarbage->empty());
}

std::vector<EnemyInfo> Plugin::GetEnemiesInFOV()
{
	std::vector<EnemyInfo> enemiesInFOV;

	EntityInfo ei;
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::ENEMY)
			{
				EnemyInfo enemy;
				m_pInterface->Enemy_GetInfo(ei, enemy);
				enemiesInFOV.push_back(enemy);
			}
			continue;
		}
		break;
	}

	SortEntitiesByDistance(enemiesInFOV);
	return enemiesInFOV;
}

Elite::Blackboard* Plugin::CreateBlackboard()
{
	Elite::Blackboard* m_pBlackboard = new Elite::Blackboard();
	m_pBlackboard->AddData("WorldState",	&m_WorldState);
	m_pBlackboard->AddData("FrameTime",		m_FrameTime);
	m_pBlackboard->AddData("AgentInfo",		AgentInfo{});
	m_pBlackboard->AddData("InventorySlot",	0U);
	m_pBlackboard->AddData("Target",		Elite::Vector2{});
	m_pBlackboard->AddData("Steering",		&m_steering);
	m_pBlackboard->AddData("Interface",		m_pInterface);
	m_pBlackboard->AddData("CellSpace",		m_WorldGrid);
	
	// Entity positions
	m_pBlackboard->AddData("Houses",				m_pAquiredHouses);
	m_pBlackboard->AddData("PistolPositions",		m_pAquiredPistols);
	m_pBlackboard->AddData("ShotgunPositions",		m_pAquiredShotguns);
	m_pBlackboard->AddData("MedPositions",			m_pAquiredMedkits);
	m_pBlackboard->AddData("FoodPositions",			m_pAquiredFood);
	m_pBlackboard->AddData("GarbagePositions",		m_pAquiredGarbage);

	// Entities in FOV
	m_pBlackboard->AddData("EnemyEntities",		std::vector<EnemyInfo>{});
	m_pBlackboard->AddData("PistolEntities",	std::vector<EntityInfo>{});
	m_pBlackboard->AddData("ShotgunEntities",	std::vector<EntityInfo>{});
	m_pBlackboard->AddData("MedEntities",		std::vector<EntityInfo>{});
	m_pBlackboard->AddData("FoodEntities",		std::vector<EntityInfo>{});
	m_pBlackboard->AddData("GarbageEntities",	std::vector<EntityInfo>{});
	return m_pBlackboard;
}

bool Plugin::TryFindPlan(const GOAP::WorldState& ws, const GOAP::WorldState& goal, std::vector<GOAP::BaseAction*>& actions)
{
	std::cout << "Finding plan for goal [" << goal.name << "]\n";
	try
	{
		m_pPlan = m_ASPlanner.FormulatePlan(ws, goal, actions);
		if (!empty(m_pPlan))
		{
			std::cout << "Found a plan: ";
			for (auto action : m_pPlan)
			{
				if (action != m_pPlan[0]) std::cout << " << ";
				std::cout << action->GetName();
			}
			std::cout << std::endl;
			return true;
		}
		return false;
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
	if (!currentAction->IsValid(m_pBlackboard))
	{
		m_CurrentGoal = GetHighestPriorityGoal();
		TryFindPlan(m_WorldState, *m_CurrentGoal, m_pActions);
		return true;
	}
	else if(currentAction->Execute(m_pBlackboard))
	{
		std::cout << "Finished excecuting " << currentAction->GetName() << std::endl;
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
		if ((newGoal == nullptr || goal->priority > newGoal->priority) && goal->IsValid(m_pBlackboard))
		{
			newGoal = goal;
		}
	}
	return newGoal;
}

bool Plugin::CheckForPurgeZone()
{
	EntityInfo ei{};
	Elite::Vector2 combinedDir{Elite::ZeroVector2};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::PURGEZONE)
			{
				m_pInterface->PurgeZone_GetInfo(ei, m_PurgeZoneInFov);
				Elite::Vector2 dir_vector = m_pInterface->Agent_GetInfo().Position - m_PurgeZoneInFov.Center;
				if (dir_vector.MagnitudeSquared() <= (m_PurgeZoneInFov.Radius * m_PurgeZoneInFov.Radius) + 200.f)
				{
					combinedDir += dir_vector;
					m_WorldState.SetVariable("inside_purgezone", true);
				}
			}
			continue;
		}
		break;
	}

	if (combinedDir != Elite::ZeroVector2)
	{
		m_Target = m_PurgeZoneInFov.Center + combinedDir.GetNormalized() * (m_PurgeZoneInFov.Radius * 1.65f);
		std::cout << "Target at: " << m_Target << std::endl;
		return true;
	}
	return false;
}

template<typename T>
void Plugin::SortEntitiesByDistance(std::vector<T>& entities)
{
	if (entities.empty()) return;

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
	if (m_pAquiredHouses->empty()) return;

	for (auto& house : *m_pAquiredHouses)
	{	
		house.TimeSinceLastVisit += m_FrameTime;
	}
}
