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
	info.BotName =				"Research_GOAP";
	info.Student_FirstName =	"Jef";
	info.Student_LastName =		"Belmans";
	info.Student_Class =		"2DAE15N";

	m_pAquiredPistols = new std::vector<ItemInfo>{};
	m_pAquiredShotguns = new std::vector<ItemInfo>{};
	m_pAquiredMedkits = new std::vector<ItemInfo>{};
	m_pAquiredFood = new std::vector<ItemInfo>{};
	m_pAquiredGarbage = new std::vector<ItemInfo>{};
	m_pAquiredEnemies = std::vector<EnemyInfo*>{};

	// Actions
	m_pActions.push_back(new GOAP::Action_MoveTo);
	m_pActions.push_back(new GOAP::Action_GrabPistol);
	m_pActions.push_back(new GOAP::Action_GrabShotgun);
	m_pActions.push_back(new GOAP::Action_GrabMedkit);
	m_pActions.push_back(new GOAP::Action_GrabFood);
	m_pActions.push_back(new GOAP::Action_Wander);
	m_pActions.push_back(new GOAP::Action_EatFood);
	m_pActions.push_back(new GOAP::Action_Heal);
	m_pActions.push_back(new GOAP::Action_SearchEnemy);
	m_pActions.push_back(new GOAP::Action_KillEnemy_Pistol);
	m_pActions.push_back(new GOAP::Action_KillEnemy_Shotgun);
	m_pActions.push_back(new GOAP::Action_FleeToSafety);

	// Initial world state
	m_WorldState.SetVariable("target_in_range",		false);
	m_WorldState.SetVariable("item_collected",		false);
	m_WorldState.SetVariable("inside_purgezone",	false);
	m_WorldState.SetVariable("pistol_aquired",		false);
	m_WorldState.SetVariable("pistol_in_inventory", false);
	m_WorldState.SetVariable("shotgun_aquired",		false);
	m_WorldState.SetVariable("shotgun_in_inventory", false);
	m_WorldState.SetVariable("medkit_aquired",		false);
	m_WorldState.SetVariable("medkit_in_inventory", false);
	m_WorldState.SetVariable("low_health",			false);
	m_WorldState.SetVariable("food_aquired",		false);
	m_WorldState.SetVariable("food_in_inventory",	false);
	m_WorldState.SetVariable("low_hunger",			false);
	m_WorldState.SetVariable("wandering",			false);
	m_WorldState.SetVariable("enemy_aquired",		false);
	m_WorldState.SetVariable("in_danger",			false);
	m_WorldState.SetVariable("house_aquired",		false);

	m_pBlackboard = CreateBlackboard();

	m_pGoals.push_back(new GOAP::Goal_FleePurgezone);
	m_pGoals.push_back(new GOAP::Goal_CollectPistol);
	m_pGoals.push_back(new GOAP::Goal_CollectShotgun);
	m_pGoals.push_back(new GOAP::Goal_CollectMedkit);
	m_pGoals.push_back(new GOAP::Goal_CollectFood);
	m_pGoals.push_back(new GOAP::Goal_EatFood);
	m_pGoals.push_back(new GOAP::Goal_Heal);
	m_pGoals.push_back(new GOAP::Goal_EnterHouse);
	m_pGoals.push_back(new GOAP::Goal_EliminateThreat);
	m_pGoals.push_back(new GOAP::Goal_Wander);
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
	params.Seed = 70;
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
	GetNewEntitiesInFOV();
	GetNewHousesInFOV();
	CheckForPurgeZone();
	ClearKilledEnemies();
		
	AgentInfo agent{ m_pInterface->Agent_GetInfo() };
	m_pBlackboard->ChangeData("Target",		m_Target);
	m_pBlackboard->ChangeData("AgentInfo",  agent);
	m_pBlackboard->ChangeData("Houses",		m_AquiredHouses);
	m_pBlackboard->ChangeData("Enemies",	m_pAquiredEnemies);

	// WorldState
	bool in_danger = m_WorldState.GetVariable("in_danger");
	m_WorldState.SetVariable("low_hunger",	m_pInterface->Agent_GetInfo().Energy < 4.f);
	m_WorldState.SetVariable("low_health",	m_pInterface->Agent_GetInfo().Health < 4.f);
	m_WorldState.SetVariable("in_danger",	in_danger || m_pInterface->Agent_GetInfo().WasBitten || !m_pAquiredEnemies.empty());
	if (!m_pAquiredEnemies.empty())
	{
		m_WorldState.SetVariable("enemy_close", m_pAquiredEnemies.back()->Location.DistanceSquared(agent.Position) < 50.f);
	}

	auto goal = GetHighestPriorityGoal();
	if (m_CurrentGoal == nullptr || goal != m_CurrentGoal || empty(m_pPlan))
	{
		m_CurrentGoal = goal;
		TryFindPlan(m_WorldState, *m_CurrentGoal, m_pActions);
	}
	else
	{
		ExecutePlan();
	}

	return m_steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}

void Plugin::GetNewHousesInFOV()
{
	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			auto it = std::find(begin(m_AquiredHouses), end(m_AquiredHouses), hi);
			if (it == end(m_AquiredHouses))
			{
				m_AquiredHouses.push_back(reinterpret_cast<HouseInfo_Extended&>(hi));
				m_AquiredHouses.back().TimeSinceLastVisit = m_AquiredHouses.back().ReactivationTime = 120.f;
			}
			continue;
		}
		break;
	}

	if (!empty(m_AquiredHouses))
		SortEntitiesByDistance(&m_AquiredHouses);

	m_WorldState.SetVariable("house_aquired", !m_AquiredHouses.empty());
}

void Plugin::GetNewEntitiesInFOV()
{
	EntityInfo ei{};
	BYTE flags{};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			// Check if we're not already aware of the entity
			auto it = std::find(begin(m_AquiredEntities), end(m_AquiredEntities), ei);
			if (it == end(m_AquiredEntities))
			{
				m_AquiredEntities.push_back(ei);
				if (ei.Type == eEntityType::ENEMY)
				{
					EnemyInfo enemy;
					if (m_pInterface->Enemy_GetInfo(ei, enemy))
					{
						m_pAquiredEnemies.push_back(&enemy);
						flags |= 1U << 5;
						continue;
					}
				}
					
				if (ei.Type != eEntityType::ITEM) continue;

				ItemInfo item{};
				m_pInterface->Item_GetInfo(ei, item);
				switch (item.Type)
				{
				case eItemType::PISTOL:
					m_pAquiredPistols->emplace_back(item);
					flags |= 1U << 0;
					break;
				case eItemType::SHOTGUN:
					m_pAquiredShotguns->emplace_back(item);
					flags |= 1U << 1;
					break;
				case eItemType::MEDKIT:
					m_pAquiredMedkits->emplace_back(item);
					flags |= 1U << 2;
					break;
				case eItemType::FOOD:
					m_pAquiredFood->emplace_back(item);
					flags |= 1U << 3;
					break;
				case eItemType::GARBAGE:
					m_pAquiredGarbage->emplace_back(item);
					flags |= 1U << 4;
					break;
				}
			}
			continue;
		} // If entity
		break;
	}

	if ((flags >> 0) & 1U)
		SortEntitiesByDistance(m_pAquiredPistols);

	m_WorldState.SetVariable("pistol_aquired", !m_pAquiredPistols->empty());

	if ((flags >> 1) & 1U)
		SortEntitiesByDistance(m_pAquiredShotguns);

	m_WorldState.SetVariable("shotgun_aquired", !m_pAquiredShotguns->empty());

	if ((flags >> 2) & 1U)
		SortEntitiesByDistance(m_pAquiredMedkits);

	m_WorldState.SetVariable("medkit_aquired", !m_pAquiredMedkits->empty());

	if ((flags >> 3) & 1U)
		SortEntitiesByDistance(m_pAquiredFood);

	m_WorldState.SetVariable("food_aquired", !m_pAquiredFood->empty());

	if ((flags >> 4) & 1U)
		SortEntitiesByDistance(m_pAquiredGarbage);

	m_WorldState.SetVariable("garbage_aquired", !m_pAquiredGarbage->empty());

	m_WorldState.SetVariable("enemy_aquired", !m_pAquiredEnemies.empty());
}

Elite::Blackboard* Plugin::CreateBlackboard()
{
	Elite::Blackboard* m_pBlackboard = new Elite::Blackboard();
	m_pBlackboard->AddData("WorldState",	&m_WorldState);
	m_pBlackboard->AddData("AgentInfo",		AgentInfo{});
	m_pBlackboard->AddData("TargetItem",	ItemInfo{});
	m_pBlackboard->AddData("InventorySlot",	0U);
	m_pBlackboard->AddData("Target",		Elite::Vector2{});
	m_pBlackboard->AddData("Steering",		&m_steering);
	m_pBlackboard->AddData("Interface",		m_pInterface);
	m_pBlackboard->AddData("Pistols",		m_pAquiredPistols);
	m_pBlackboard->AddData("Shotguns",		m_pAquiredShotguns);
	m_pBlackboard->AddData("Meds",			m_pAquiredMedkits);
	m_pBlackboard->AddData("Food",			m_pAquiredFood);
	m_pBlackboard->AddData("Garbage",		m_pAquiredGarbage);
	m_pBlackboard->AddData("Houses",		std::vector<HouseInfo_Extended>{});
	m_pBlackboard->AddData("Enemies",		m_pAquiredEnemies);
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
				std::cout << " << " <<action->GetName();
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
	if (currentAction->IsValid(m_pBlackboard) && currentAction->Execute(m_pBlackboard))
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

void Plugin::ClearKilledEnemies()
{
	for (size_t i = 0; i < m_pAquiredEnemies.size(); i++)
	{
		if (m_pAquiredEnemies[i]->Health == 0.f) m_pAquiredEnemies.erase(m_pAquiredEnemies.begin() + i);
	}
}

bool Plugin::CheckForPurgeZone()
{
	EntityInfo ei{};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::PURGEZONE)
			{
				m_pInterface->PurgeZone_GetInfo(ei, m_PurgeZoneInFov);
				m_Target = (m_pInterface->Agent_GetInfo().Position - m_PurgeZoneInFov.Center).GetNormalized() * (m_PurgeZoneInFov.Radius);
				m_WorldState.SetVariable("inside_purgezone", true);
				return true;
			}
			continue;
		}
		break;
	}

	m_WorldState.SetVariable("inside_purgezone", false);
	return false;
}

template<typename T>
void Plugin::SortEntitiesByDistance(std::vector<T>* entities)
{
	if (entities == nullptr) return;

	std::sort(entities->begin(), entities->end(), [&](const T& a, const T& b)
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

	for (auto& house : m_AquiredHouses)
	{		
		if (m_pInterface->Agent_GetInfo().Position.DistanceSquared(house.Location) < 20.f)
		{
			house.TimeSinceLastVisit = 0.f;
		}
		else
			house.TimeSinceLastVisit += m_FrameTime;
	}
}
