#include "stdafx.h"
#include "Plugin.h"
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
	info.BotName = "Research_GOAP";
	info.Student_FirstName = "Jef";
	info.Student_LastName = "Belmans";
	info.Student_Class = "2DAE15N";
}

//Called only once
void Plugin::DllInit()
{
	auto search_house_func = [&]()
	{
		Wander(m_Steering);

		HouseInfo_Extended hi = {};
		size_t numHousesBeforeSearch{ m_AquiredHouses.size() };
		for (int i = 0;; ++i)
		{
			if (m_pInterface->Fov_GetHouseByIndex(i, hi))
			{
				if (std::find(begin(m_AquiredHouses), end(m_AquiredHouses), hi) == end(m_AquiredHouses))
				{
					m_AquiredHouses.push_back(hi);
				}
				continue;
			}
			break;
		}

		if (!empty(m_AquiredHouses) && numHousesBeforeSearch < m_AquiredHouses.size())
		{
			SortEntitiesByDistance(m_AquiredHouses);
			return true;
		}

		return false;
	};
	GOAP::Action search_house("Search For House", 5, search_house_func);
	search_house.SetPrecondition(house_aquired, false);
	search_house.SetPrecondition(inside_house, false);
	search_house.SetEffect(house_aquired, true);
	m_Actions.push_back(search_house);

	auto move_to_house_func = [&]()
	{
		auto it = std::find_if(begin(m_AquiredHouses), end(m_AquiredHouses), [&](const HouseInfo_Extended& house)
			{
				return !house.HasRecentlyVisited();
			});

		if (it != end(m_AquiredHouses))
		{
			const float EPSILON{ 1.f };
			if (Seek(m_Steering, it->Location, EPSILON))
			{
				it->TimeSinceLastVisit = 0.f;
				return true;
			}
			return false;
		}
		return false;
	};
	auto move_to_house_costfunc = [&](int cost)
	{
		if (!empty(m_AquiredHouses))
		{
			return static_cast<int>(m_AquiredHouses.back().Location.DistanceSquared(m_pInterface->Agent_GetInfo().Position));
		}
		
		return cost;
	};
	GOAP::Action move_to_house("Move To House", 50, move_to_house_func);
	move_to_house.SetCostFunction(move_to_house_costfunc);
	move_to_house.SetPrecondition(house_aquired, true);
	move_to_house.SetPrecondition(inside_house, false);
	move_to_house.SetEffect(inside_house, true);
	m_Actions.push_back(move_to_house);

	auto search_pistol_func = [&]()
	{
		m_Steering.AutoOrient = false;
		m_Steering.AngularVelocity = 45.f;

		return CheckForPistol();
	};
	GOAP::Action search_pistol("Search For Pistol", 5, search_pistol_func);
	search_pistol.SetPrecondition(inside_house, true);
	search_pistol.SetPrecondition(pistol_in_range, false);
	search_pistol.SetPrecondition(pistol_aquired, false);
	search_pistol.SetEffect(pistol_aquired, true);
	m_Actions.push_back(search_pistol);

	auto move_to_pistol_func = [&]()
	{
		const float EPSILON{ m_pInterface->Agent_GetInfo().GrabRange };
		return Seek(m_Steering, m_AquiredPistols.back().Location, EPSILON);
	};
	auto move_to_pistol_costfunc = [&](int cost)
	{
		if (!empty(m_AquiredPistols))
		{
			return static_cast<int>(m_AquiredPistols.back().Location.DistanceSquared(m_pInterface->Agent_GetInfo().Position));
		}
		
		return cost;
	};
	GOAP::Action move_to_pistol("Move To Pistol", 50, move_to_pistol_func);
	move_to_pistol.SetCostFunction(move_to_pistol_costfunc);
	move_to_pistol.SetPrecondition(pistol_aquired, true);
	move_to_pistol.SetPrecondition(pistol_in_range, false);
	move_to_pistol.SetEffect(pistol_in_range, true);
	m_Actions.push_back(move_to_pistol);

	auto grab_pistol_func = [&]()
	{
		m_Steering.AutoOrient = false;
		m_Steering.LinearVelocity = Elite::ZeroVector2;
		const Elite::Vector2 dir_vector = (m_AquiredPistols.back().Location - m_pInterface->Agent_GetInfo().Position).GetNormalized();
		const float target_angle = atan2f(dir_vector.y, dir_vector.x);
		const float agent_angle = m_pInterface->Agent_GetInfo().Orientation;
		const float delta_angle = target_angle - agent_angle;

		if (abs(delta_angle) > 1.f)
		{
			m_Steering.AngularVelocity = m_pInterface->Agent_GetInfo().MaxAngularSpeed;
			if (delta_angle < 0 || delta_angle > static_cast<float>(M_PI))
			{
				m_Steering.AngularVelocity = -m_pInterface->Agent_GetInfo().MaxAngularSpeed;
			}
		}
		else
		{
			m_Steering.AngularVelocity = 0.f;
			if (m_pInterface->Item_Grab({}, reinterpret_cast<ItemInfo&>(m_AquiredPistols.back())))
			{
				if (m_pInterface->Inventory_AddItem(0, reinterpret_cast<ItemInfo&>(m_AquiredPistols.back())))
				{
					m_ItemsInInventory++;
					m_AquiredPistols.pop_back();
					return true;
				}
			}
		}

		return false;
	};

	GOAP::Action grab_pistol("Grab Pistol", 5, grab_pistol_func);
	grab_pistol.SetPrecondition(pistol_in_range, true);
	grab_pistol.SetPrecondition(pistol_collected, false);
	grab_pistol.SetEffect(pistol_in_range, false);
	grab_pistol.SetEffect(pistol_collected, true);
	grab_pistol.SetEffect(pistol_in_inventory, true);
	m_Actions.push_back(grab_pistol);

	auto flee_purgezone_func = [&]()
	{
		Elite::Vector2 directionTowardsEdge((m_pInterface->Agent_GetInfo().Position - m_PurgeZoneInFov.Center).GetNormalized());
		return Seek(m_Steering, m_PurgeZoneInFov.Center + directionTowardsEdge * (m_PurgeZoneInFov.Radius + 10.f), 0.5f);
	};
	GOAP::Action flee_purgezone("Flee Purge Zone", 5, flee_purgezone_func);
	flee_purgezone.SetPrecondition(purge_zone_in_range, true);
	flee_purgezone.SetEffect(purge_zone_in_range, false);
	m_Actions.push_back(flee_purgezone);

	auto wander_func = [&]()
	{
		Wander(m_Steering);
		return false;
	};

	m_WanderAction = GOAP::Action("Wander", 30, wander_func);

	// Initial world state
	m_WorldState.SetVariable(house_aquired, false);
	m_WorldState.SetVariable(inside_house, false);
	m_WorldState.SetVariable(health_low, false);
	m_WorldState.SetVariable(energy_low, false);
	m_WorldState.SetVariable(stamina_low, false);
	m_WorldState.SetVariable(food_aquired, false);
	m_WorldState.SetVariable(food_in_range, false);
	m_WorldState.SetVariable(pistol_aquired, false);
	m_WorldState.SetVariable(pistol_in_range, false);
	m_WorldState.SetVariable(pistol_in_inventory, false);
	m_WorldState.SetVariable(pistol_collected, false);
	m_WorldState.SetVariable(purge_zone_in_range, false);

	// Goal states
	GOAP::WorldState goal_collect_pistol("Collect Pistol");
	goal_collect_pistol.SetVariable(pistol_collected, true);
	goal_collect_pistol.priority = 100.f;
	auto collect_pistol_priorityfunc = [&](float& priority)
	{
		// Check if we have already spotted a pistol previously, which we didn't pick up yet (pistol is not nullptr)
		// or if there is a pistol in view when we haven't seen one already
		const bool pistolAquired = CheckForPistol();
		m_WorldState.SetVariable(pistol_aquired, pistolAquired);
		if(pistolAquired)
			m_WorldState.SetVariable(pistol_collected, false);

		return pistolAquired ? priority = 200.f : priority = 100.f;
	};
	goal_collect_pistol.SetPriorityCalcFunc(collect_pistol_priorityfunc);
	m_Goals.push_back(goal_collect_pistol);

	GOAP::WorldState goal_flee_purgezone("Flee Purgezone");
	goal_flee_purgezone.SetVariable(purge_zone_in_range, false);
	goal_flee_purgezone.priority = 0;
	auto flee_purgezone_priorityfunc = [&](float& priority)
	{
		m_WorldState.SetVariable(purge_zone_in_range, CheckForPurgeZone());
		return m_WorldState.GetVariable(purge_zone_in_range) ? priority = std::numeric_limits<float>::infinity() : priority = 0.f;
	};
	goal_flee_purgezone.SetPriorityCalcFunc(flee_purgezone_priorityfunc);
	m_Goals.push_back(goal_flee_purgezone);
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
	m_EntitiesInFov = GetEntitiesInFOV();
	UpdateHouseInfo();

	for (auto& goal : m_Goals)
	{
		goal.UpdatePriority();
		if (goal.priority > m_CurrentGoal.priority)
		{
			std::cout << "Another goal has a higher priority, formulating new plan" << std::endl;
			FindPlan();
			break;
		}
	}

	if (!m_Plan.empty())
	{
		// There are still actions in the plan, execute the first action in line
		auto& currentAction = m_Plan.back();
		if (!currentAction.Execute(m_WorldState, m_FrameTime))
		{
			// Current action aborted, find new plan
			FindPlan();
			
		}
		else if (currentAction.GetIsDone())
		{
			// Action finished, remove it from the plan
			m_Plan.pop_back();
		}
	}
	else
	{
		// Finished plan, find new plan
		FindPlan();
	}

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
	}

	if (m_UseItem)
	{
		//Use an item (make sure there is an item at the given inventory slot)
		m_pInterface->Inventory_UseItem(m_InventorySlot);
	}

	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		m_pInterface->Inventory_RemoveItem(m_InventorySlot);
	}

	m_Steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

	//@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return m_Steering;
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

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}

bool Plugin::FindPlan()
{
	m_CurrentGoal = m_Goals.back();
	for (const auto& goal : m_Goals)
	{
		if (m_CurrentGoal.priority < goal.priority)
		{
			m_CurrentGoal = goal;
		}
	}

	std::cout << "Current goal: [" << m_CurrentGoal.name << "]\n";

	try
	{
		m_Plan = m_ASPlanner.FormulatePlan(m_WorldState, m_CurrentGoal, m_Actions);
		if (empty(m_Plan))
		{
			std::cout << "Goal already met, wandering!\n";
			m_Plan.push_back(m_WanderAction);
			return false;
		}
		
		std::cout << "Found a plan!\n";
		for (std::vector<GOAP::Action>::reverse_iterator rit = rbegin(m_Plan); rit != rend(m_Plan); ++rit)
		{
			std::cout << rit->GetName() << " -> ";
		}
		std::cout << std::endl;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cout << "Could not find a plan, wandering!\n";
		m_Plan.push_back(m_WanderAction);
		return false;
	}
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
	for (auto& e : m_EntitiesInFov)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			m_pInterface->PurgeZone_GetInfo(e, m_PurgeZoneInFov);
			m_WorldState.SetVariable(purge_zone_in_range, true);
			return true;
		}
	}

	return false;
}

bool Plugin::CheckForPistol()
{
	if (!empty(m_AquiredPistols)) return true;

	for (size_t i = 0; i < m_EntitiesInFov.size(); ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, m_EntitiesInFov[i]))
		{
			if (m_EntitiesInFov[i].Type != eEntityType::ITEM) continue;

			ItemInfo item;
			if (m_pInterface->Item_GetInfo(m_EntitiesInFov[i], item) && item.Type == eItemType::PISTOL)
			{
				m_AquiredPistols.push_back(m_EntitiesInFov[i]);
			}
		}
	}

	if (!empty(m_AquiredPistols))
	{
		SortEntitiesByDistance(m_EntitiesInFov);
		return true;
	}
	
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

	m_WorldState.SetVariable(house_aquired, false);
	for (auto& house : m_AquiredHouses)
	{
		house.TimeSinceLastVisit += m_FrameTime;
		if (!house.HasRecentlyVisited())
		{
			m_WorldState.SetVariable(house_aquired, true);
		}
	}
}
