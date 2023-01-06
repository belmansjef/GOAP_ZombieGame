#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"

#include <limits>

// Actions
#include "GOAP/Actions/BaseAction.h"
#include "GOAP/Actions/Action_FleePurgezone.h"
#include "GOAP/Actions/Action_Heal.h"
#include "GOAP/Actions/Action_KillEnemyPistol.h"
#include "GOAP/Actions/Action_KillEnemyShotgun.h"
#include "GOAP/Actions/Action_SearchEnemy.h"
#include "GOAP/Actions/Action_Eat.h"
#include "GOAP/Actions/Action_GrabPistol.h"
#include "GOAP/Actions/Action_GrabShotgun.h"
#include "GOAP/Actions/Action_GrabFood.h"
#include "GOAP/Actions/Action_GrabMedkit.h"
#include "GOAP/Actions/Action_DestroyGarbage.h"
#include "GOAP/Actions/Action_LootHouse.h"
#include "GOAP/Actions/Action_SearchArea.h"
#include "GOAP/Actions/Action_GoToClosestCell.h"

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
	m_WorldGrid = std::move(CellSpace(m_WorldDimensions.x, m_WorldDimensions.y, 32, 32));

	// Entities
	m_pAquiredHouses		= new std::vector<HouseInfo_Extended>;
	m_pAquiredHouses->reserve(20);
	m_pAquiredEntities		= new std::vector<EntityInfo>;
	m_pAquiredEntities->reserve(50);
	m_pAquiredPistols		= new std::vector<EntityInfo>;
	m_pAquiredPistols->reserve(50);
	m_pAquiredShotguns		= new std::vector<EntityInfo>;
	m_pAquiredShotguns->reserve(50);
	m_pAquiredMedkits		= new std::vector<EntityInfo>;
	m_pAquiredMedkits->reserve(50);
	m_pAquiredFood			= new std::vector<EntityInfo>;
	m_pAquiredFood->reserve(50);
	m_pAquiredGarbage		= new std::vector<EntityInfo>;
	m_pAquiredGarbage->reserve(50);
	m_EnemiesInFOV			= std::vector<EnemyInfo>();

	// GOAP Actions
	m_pActions.push_back(new GOAP::Action_FleePurgezone);
	m_pActions.push_back(new GOAP::Action_Heal);
	m_pActions.push_back(new GOAP::Action_KillEnemyPistol);
	m_pActions.push_back(new GOAP::Action_KillEnemyShotgun);
	m_pActions.push_back(new GOAP::Action_SearchEnemy);
	m_pActions.push_back(new GOAP::Action_Eat);
	m_pActions.push_back(new GOAP::Action_GrabPistol);
	m_pActions.push_back(new GOAP::Action_GrabShotgun);
	m_pActions.push_back(new GOAP::Action_GrabFood);
	m_pActions.push_back(new GOAP::Action_GrabMedkit);
	m_pActions.push_back(new GOAP::Action_DestroyGarbage);
	m_pActions.push_back(new GOAP::Action_LootHouse);
	m_pActions.push_back(new GOAP::Action_SearchArea);
	m_pActions.push_back(new GOAP::Action_GotoClosestCell);

	// Initial world state
	m_WorldState.SetVariable("inside_purgezone",	false);
	m_WorldState.SetVariable("in_danger",			false);
	m_WorldState.SetVariable("enemy_aquired",		false);
	m_WorldState.SetVariable("pistol_aquired",		false);
	m_WorldState.SetVariable("pistol_in_inventory", false);
	m_WorldState.SetVariable("shotgun_aquired",		false);
	m_WorldState.SetVariable("shotgun_in_inventory",false);
	m_WorldState.SetVariable("food_aquired",		false);
	m_WorldState.SetVariable("food_in_inventory",	false);
	m_WorldState.SetVariable("food_inventory_full", false);
	m_WorldState.SetVariable("use_first_food",		true);
	m_WorldState.SetVariable("low_energy",			false);
	m_WorldState.SetVariable("medkit_aquired",		false);
	m_WorldState.SetVariable("medkit_in_inventory",	false);
	m_WorldState.SetVariable("low_health",			false);
	m_WorldState.SetVariable("house_aquired",		false);
	m_WorldState.SetVariable("all_houses_looted",	true);
	m_WorldState.SetVariable("all_areas_searched",	true);
	m_WorldState.SetVariable("garbage_aquired",		false);
	m_WorldState.SetVariable("is_world_explored",	false);
	m_WorldState.SetVariable("has_explored",		false);
	m_pBlackboard = CreateBlackboard();
	
	// GOAP Goals
	m_pGoals.push_back(new GOAP::Goal_FleePurgezone);
	m_pGoals.push_back(new GOAP::Goal_ReplenishHealth);
	m_pGoals.push_back(new GOAP::Goal_EliminateThreat);
	m_pGoals.push_back(new GOAP::Goal_ReplenishEnergy);
	m_pGoals.push_back(new GOAP::Goal_LootPistol);
	m_pGoals.push_back(new GOAP::Goal_LootPistol);
	m_pGoals.push_back(new GOAP::Goal_LootShotgun);
	m_pGoals.push_back(new GOAP::Goal_LootFood);
	m_pGoals.push_back(new GOAP::Goal_LootMedkit);
	m_pGoals.push_back(new GOAP::Goal_ClearGarbage);
	m_pGoals.push_back(new GOAP::Goal_LootHouse);
	m_pGoals.push_back(new GOAP::Goal_SearchArea);
	m_pGoals.push_back(new GOAP::Goal_ExploreWorld);

	// FSM States
	m_IdleState = GOAP::FSMState([&](GOAP::FSM* pFSM, Elite::Blackboard* pBlackboard)
		{
			m_CurrentGoal = GetHighestPriorityGoal();
			m_pPlan = m_ASPlanner.FormulatePlan(m_WorldState, *m_CurrentGoal, m_pActions, m_pBlackboard);
			if (!m_pPlan.empty())
			{
				std::cout << m_WorldState << std::endl;
				std::cout << "found a plan for goal [" << m_CurrentGoal->name << "]: ";
				for (auto& it = m_pPlan.rbegin(); it != m_pPlan.rend(); ++it)
				{
					if (it != m_pPlan.rbegin())
					{
						std::cout << " -> ";
					}
					std::cout << "[" << (*it)->GetName() << "]";
				}
				std::cout << std::endl;
				pFSM->PopState();
				pFSM->PushState(m_ExecuteActionState);
			}
			else
			{
				pFSM->PopState();
				pFSM->PushState(m_IdleState);
			}
		});

	m_MoveToState = GOAP::FSMState([&](GOAP::FSM* pFSM, Elite::Blackboard* pBlackboard)
		{
			GOAP::BaseAction* pAction = m_pPlan.back();
			if (pAction->RequiresInRange() && pAction->GetTarget() == nullptr)
			{
				pFSM->PopState();
				pFSM->PopState();
				pFSM->PushState(m_IdleState);
				return;
			}

			if (MoveAgent(pAction))
			{
				pFSM->PopState();
			}
		});

	m_ExecuteActionState = GOAP::FSMState([&](GOAP::FSM* pFSM, Elite::Blackboard* pBlackboard)
		{
			// Action is done, move to next action
			GOAP::BaseAction* pAction = m_pPlan.back();
			if (pAction->IsDone())
			{
				std::cout << "finished executing [" << pAction->GetName() << "]" << std::endl;
				m_pPlan.pop_back();
				if (m_pPlan.empty())
				{
					pFSM->PopState();
					pFSM->PushState(m_IdleState);
					return;
				}
			}

			pAction = m_pPlan.back();
			bool inRange = pAction->RequiresInRange() ? pAction->IsInRange() : true;
			if (inRange)
			{
				// Action returns false if it fails -- If it fails, abort plan and get new one
				if (!pAction->Execute(pBlackboard))
				{
					pFSM->PopState();
					pFSM->PushState(m_IdleState);
				}
			}
			else
			{
				pFSM->PushState(m_MoveToState);
			}
		});

	m_FSM.PushState(m_IdleState);
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
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = false;
	params.ShowDebugItemNames = true;
	params.SpawnZombieOnRightClick = true;
	params.Seed = 8;
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
	m_pBlackboard->ChangeData("Target",		m_Target);
	m_pBlackboard->ChangeData("FrameTime",	m_FrameTime);
	m_pBlackboard->ChangeData("CellSpace",	m_WorldGrid);
	m_pBlackboard->ChangeData("Enemies",	m_EnemiesInFOV);

	// World State
	m_WorldState.SetVariable("low_energy", m_pInterface->Agent_GetInfo().Energy <= 4.f);
	m_WorldState.SetVariable("use_first_food", true);
	if (m_WorldState.GetVariable("food_in_inventory"))
	{
		ItemInfo item;
		int energy;
		if (m_WorldState.GetVariable("food_inventory_full"))
		{
			m_pInterface->Inventory_GetItem(4U, item);
			energy = m_pInterface->Food_GetEnergy(item);
		}
		else
		{
			m_pInterface->Inventory_GetItem(3U, item);
			energy = m_pInterface->Food_GetEnergy(item);
		}
		m_WorldState.SetVariable("low_energy", m_pInterface->Agent_GetInfo().Energy <= 10.f - energy);
	}
	
	m_WorldState.SetVariable("low_health",		m_pInterface->Agent_GetInfo().Health <= 6.f);
	m_WorldState.SetVariable("in_danger",		m_WorldState.GetVariable("in_danger") || m_pInterface->Agent_GetInfo().WasBitten || !m_EnemiesInFOV.empty());
	m_WorldState.SetVariable("enemy_aquired",	!m_EnemiesInFOV.empty());

	m_FSM.Update(m_pBlackboard);
	GOAP::WorldState goal{*GetHighestPriorityGoal()};
	if (*m_CurrentGoal != goal)
	{
		m_FSM.ClearStack();
		m_FSM.PushState(m_IdleState);
	}

	return m_Steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });

	// World grid
	for (const auto& cell : m_WorldGrid.GetCells())
	{
		if (cell.hasVisited)
			m_pInterface->Draw_Polygon(&cell.GetRectPoints()[0], 4, {0.f, 1.f, 0.f}, -1.f);
		else
			m_pInterface->Draw_Polygon(&cell.GetRectPoints()[0], 4, { 1.f, 0.f, 0.f });
	}

	for (const auto& cell : m_WorldGrid.GetPath())
	{
		m_pInterface->Draw_Polygon(&cell.GetRectPoints()[0], 4, { 0.f, 0.f, 1.f }, -1.f);
	}

	// World border
	m_pInterface->Draw_Polygon(&m_WorldBoundaries[0], 4, { 0.f, 0.f, 0.f }, m_pInterface->NextDepthSlice());

	// Entities
	for (const auto& e : *m_pAquiredEntities)
	{
		m_pInterface->Draw_Circle(e.Location, 2.f,	{ 0.f, 1.f, 0.f });
	}

	// Houses
	for (const auto& house : *m_pAquiredHouses)
	{
		Elite::Vector3 color{0.f, 0.f, 1.f};
		if (house.HasRecentlyVisited()) color = { 1.f, 1.f, 0.f };
		if (house.AreaSearched) color = { 0.f, 1.f, 1.f };

		m_pInterface->Draw_Polygon(&house.GetRectPoints()[0], 4, color);
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
				m_pAquiredHouses->back().AreaSearched = false;
				m_pAquiredHouses->back().NextCornerAreaSearch = Corner::TopRight;
				m_pAquiredHouses->back().NextCornerHouseSearch = Corner::BottomLeft;
				m_pAquiredHouses->back().TimeSinceLastVisit = m_pAquiredHouses->back().ReactivationTime = 900.f;
			}
			continue;
		}
		break;
	}

	m_WorldState.SetVariable("house_aquired", !m_pAquiredHouses->empty());
}

void Plugin::GetEntitiesInFOV()
{
	EntityInfo ei;
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type != eEntityType::ITEM) continue;

			ItemInfo item;
			m_pInterface->Item_GetInfo(ei, item);

			// Check if we are aware of this entity
			if (std::find(m_pAquiredEntities->begin(), m_pAquiredEntities->end(), ei) == m_pAquiredEntities->end())
			{
				m_pAquiredEntities->push_back(ei);

				// If we aren't aware of this entity, add it to the correct vector
				switch (item.Type)
				{
				case eItemType::PISTOL:
					m_pAquiredPistols->emplace_back(ei);
					break;
				case eItemType::SHOTGUN:
					m_pAquiredShotguns->emplace_back(ei);
					break;
				case eItemType::MEDKIT:
					m_pAquiredMedkits->emplace_back(ei);
					break;
				case eItemType::FOOD:
					m_pAquiredFood->emplace_back(ei);
					break;
				case eItemType::GARBAGE:
					m_pAquiredGarbage->emplace_back(ei);
					break;
				}
			}
			else
			{
				// We are already aware of the entity, update it's hash
				switch (item.Type)
				{
					case eItemType::PISTOL:
					{
						auto& it = std::find(m_pAquiredPistols->begin(), m_pAquiredPistols->end(), ei);
						if (it != m_pAquiredPistols->end())
							it->EntityHash = ei.EntityHash;
						break;
					}
					case eItemType::SHOTGUN:
					{
						auto& it = std::find(m_pAquiredShotguns->begin(), m_pAquiredShotguns->end(), ei);
						if (it != m_pAquiredShotguns->end())
							it->EntityHash = ei.EntityHash;
						break;
					}
					case eItemType::MEDKIT:
					{
						auto& it = std::find(m_pAquiredMedkits->begin(), m_pAquiredMedkits->end(), ei);
						if (it != m_pAquiredMedkits->end())
							it->EntityHash = ei.EntityHash;
						break;
					}
					case eItemType::FOOD:
					{
						auto& it = std::find(m_pAquiredFood->begin(), m_pAquiredFood->end(), ei);
						if (it != m_pAquiredFood->end())
							it->EntityHash = ei.EntityHash;
						break;
					}
					case eItemType::GARBAGE:
					{
						auto& it = std::find(m_pAquiredGarbage->begin(), m_pAquiredGarbage->end(), ei);
						if (it != m_pAquiredGarbage->end())
							it->EntityHash = ei.EntityHash;
						break;
					}
				}
			}
			continue;
		} // If entity
		break;
	}
	m_WorldState.SetVariable("pistol_aquired",	!m_pAquiredPistols->empty());
	m_WorldState.SetVariable("shotgun_aquired", !m_pAquiredShotguns->empty());
	m_WorldState.SetVariable("medkit_aquired",	!m_pAquiredMedkits->empty());
	m_WorldState.SetVariable("food_aquired",	!m_pAquiredFood->empty());
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

	SortEntitiesByDistance(&enemiesInFOV);
	return enemiesInFOV;
}

Elite::Blackboard* Plugin::CreateBlackboard()
{
	Elite::Blackboard* m_pBlackboard = new Elite::Blackboard();
	m_pBlackboard->AddData("WorldState",	&m_WorldState);
	m_pBlackboard->AddData("FrameTime",		m_FrameTime);
	m_pBlackboard->AddData("Target",		Elite::Vector2{});
	m_pBlackboard->AddData("Steering",		&m_Steering);
	m_pBlackboard->AddData("Interface",		m_pInterface);
	m_pBlackboard->AddData("CellSpace",		m_WorldGrid);
	
	// Entities
	m_pBlackboard->AddData("Entities",		m_pAquiredEntities);
	m_pBlackboard->AddData("Houses",		m_pAquiredHouses);
	m_pBlackboard->AddData("Pistols",		m_pAquiredPistols);
	m_pBlackboard->AddData("Shotguns",		m_pAquiredShotguns);
	m_pBlackboard->AddData("Medkits",		m_pAquiredMedkits);
	m_pBlackboard->AddData("Food",			m_pAquiredFood);
	m_pBlackboard->AddData("Garbage",		m_pAquiredGarbage);

	// Entities in FOV
	m_pBlackboard->AddData("Enemies",		std::vector<EnemyInfo>{});
	return m_pBlackboard;
}

GOAP::WorldState* Plugin::GetHighestPriorityGoal()
{
	GOAP::WorldState* newGoal{};
	for (const auto goal : m_pGoals)
	{
		if ((newGoal == nullptr || goal->priority > newGoal->priority) && goal->IsValid(m_WorldState))
		{
			newGoal = goal;
		}
	}
	return newGoal;
}

bool Plugin::MoveAgent(GOAP::BaseAction* pAction)
{
	// Debug draw
	m_pInterface->Draw_SolidCircle(pAction->GetTarget()->Location, .7f, { 0,0 }, { 1, 0, 0 });

	AgentInfo agentInfo{ m_pInterface->Agent_GetInfo() };
	if (agentInfo.Position.DistanceSquared(pAction->GetTarget()->Location) <= (agentInfo.GrabRange * agentInfo.GrabRange) + 5.f)
	{
		m_Steering.LinearVelocity = Elite::ZeroVector2;
		m_Steering.AutoOrient = false;

		const float angleError{ 1.f };
		const Elite::Vector2 dirVector = (pAction->GetTarget()->Location - agentInfo.Position).GetNormalized();
		const float targetAngle = atan2f(dirVector.y, dirVector.x);
		const float agentAngle = agentInfo.Orientation;
		const float delta_angle = targetAngle - agentAngle;
		
		// Looking at target
		if (abs(delta_angle) <= angleError)
		{
			m_Steering.AutoOrient = true;
			m_Steering.RunMode = false;
			m_Steering.LinearVelocity = Elite::ZeroVector2;
			m_Steering.AngularVelocity = 0.f;
			pAction->SetInRange(true);
			return true;
		}

		m_Steering.AngularVelocity = agentInfo.MaxAngularSpeed;
		if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
		{
			m_Steering.AngularVelocity = -agentInfo.MaxAngularSpeed;
		}
	}

	m_Steering.LinearVelocity = (m_pInterface->NavMesh_GetClosestPathPoint(pAction->GetTarget()->Location) - agentInfo.Position).GetNormalized();
	m_Steering.LinearVelocity *= agentInfo.MaxLinearSpeed;
	if (m_WorldState.GetVariable("in_danger") && agentInfo.Stamina >= 5.f)
		m_Steering.RunMode = true;
	else if (agentInfo.Stamina == 0.f)
		m_Steering.RunMode = false;

	return false;
}

bool Plugin::CheckForPurgeZone()
{
	EntityInfo ei{};
	Elite::Vector2 combinedDir{Elite::ZeroVector2};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::PURGEZONE && !m_WorldState.GetVariable("inside_purgezone"))
			{
				m_pInterface->PurgeZone_GetInfo(ei, m_PurgeZoneInFOV);
				const Elite::Vector2 dir_vector = m_pInterface->Agent_GetInfo().Position - m_PurgeZoneInFOV.Center;
				if (dir_vector.MagnitudeSquared() <= (m_PurgeZoneInFOV.Radius * m_PurgeZoneInFOV.Radius) + 250.f)
				{
					m_Target = m_PurgeZoneInFOV.Center + dir_vector.GetNormalized() * (m_PurgeZoneInFOV.Radius * 1.5f);
					m_WorldState.SetVariable("inside_purgezone", true);
					m_WorldState.SetVariable("in_danger", true);
					return true;
				}
			}
			continue;
		}
		break;
	}
	return false;
}

template<typename T>
void Plugin::SortEntitiesByDistance(std::vector<T>* entities)
{
	if (entities->empty()) return;

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
	if (m_pAquiredHouses->empty()) return;

	m_WorldState.SetVariable("all_houses_looted", true);
	m_WorldState.SetVariable("all_areas_searched", true);
	for (auto& house : *m_pAquiredHouses)
	{	
		house.TimeSinceLastVisit += m_FrameTime;
		if (!house.HasRecentlyVisited())
		{
			m_WorldState.SetVariable("all_houses_looted", false);
		}
		if(!house.AreaSearched)
		{
			m_WorldState.SetVariable("all_areas_searched", false);
		}
	}
}
