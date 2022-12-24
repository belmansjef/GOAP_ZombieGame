#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"

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
	//Called when the plugin is loaded
	std::cout << "Zombie example running...\n";

	// Constants for the various states are helpful to keep us from
	// accidentally mistyping a state name.
	const int house_aquired = 0;
	const int house_in_range = 5;
	const int moving_to_house = 10;
	const int item_aquired = 15;
	const int item_in_range = 20;
	const int item_inventory_full = 25;

	// Now establish all the possible actions for the action pool
	// In this example we're providing the AI some different FPS actions
	auto search_house_func = [&](IExamInterface* pInterface, SteeringPlugin_Output& steering)
	{
		float m_WanderAngle{};
		m_WanderAngle += Elite::randomFloat(-45.f, 45.f);
		const Elite::Vector2 circle_center = pInterface->Agent_GetInfo().Position + (pInterface->Agent_GetInfo().LinearVelocity.GetNormalized() * 5.f);
		const Elite::Vector2 desired_location = pInterface->NavMesh_GetClosestPathPoint({ cosf(m_WanderAngle) * 10.f + circle_center.x, sinf(m_WanderAngle) * 10.f + circle_center.y });
		
		steering.LinearVelocity = desired_location - pInterface->Agent_GetInfo().Position;
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= pInterface->Agent_GetInfo().MaxLinearSpeed;

		HouseInfo hi = {};
		for (int i = 0;; ++i)
		{
			if (m_pInterface->Fov_GetHouseByIndex(i, hi))
			{
				if (hi.Center != m_HousePos)
				{
					m_HousePos = hi.Center;
					return true;
				}
			}

			break;
		}

		return false;
	};
	GOAP::Action search_house("SearchForHouse", 5, search_house_func);
	search_house.SetPrecondition(house_aquired, false);
	search_house.SetPrecondition(moving_to_house, false);
	search_house.SetEffect(house_aquired, true);
	m_Actions.push_back(search_house);

	auto move_to_house_func = [&](IExamInterface* pInterface, SteeringPlugin_Output& steering)
	{
		const float EPSILON{ 1.f };
		const Elite::Vector2 desired_location = pInterface->NavMesh_GetClosestPathPoint(m_HousePos);
		steering.LinearVelocity = desired_location - pInterface->Agent_GetInfo().Position;
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= pInterface->Agent_GetInfo().MaxLinearSpeed;

		return pInterface->Agent_GetInfo().Position.DistanceSquared(m_HousePos) < EPSILON;
	};
	GOAP::Action move_to_house("MoveToHouse", 10, move_to_house_func);
	move_to_house.SetPrecondition(house_aquired, true);
	move_to_house.SetPrecondition(moving_to_house, false);
	move_to_house.SetPrecondition(house_in_range, false);
	move_to_house.SetEffect(house_in_range, true);
	m_Actions.push_back(move_to_house);

	auto search_item_func = [&](IExamInterface* pInterface, SteeringPlugin_Output& steering)
	{
		float m_WanderAngle{};
		m_WanderAngle += Elite::randomFloat(-45.f, 45.f);
		const Elite::Vector2 circle_center = pInterface->Agent_GetInfo().Position + (pInterface->Agent_GetInfo().LinearVelocity.GetNormalized() * 5.f);
		const Elite::Vector2 desired_location = pInterface->NavMesh_GetClosestPathPoint({ cosf(m_WanderAngle) * 10.f + circle_center.x, sinf(m_WanderAngle) * 10.f + circle_center.y });

		steering.LinearVelocity = desired_location - pInterface->Agent_GetInfo().Position;
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= pInterface->Agent_GetInfo().MaxLinearSpeed;

		EntityInfo ei = {};

		for (int i = 0;; ++i)
		{
			if (m_pInterface->Fov_GetEntityByIndex(i, ei))
			{
				ItemInfo item;
				if (pInterface->Item_GetInfo(ei, item))
				{
					m_ItemsInView.push_back(item);
				}
				continue;
			}

			break;
		}

		return !m_ItemsInView.empty();
	};
	GOAP::Action search_item("SearchItem", 5, search_item_func);
	search_item.SetPrecondition(house_in_range, true);
	search_item.SetPrecondition(item_in_range, false);
	search_item.SetPrecondition(item_inventory_full, false);
	search_item.SetEffect(item_aquired, true);
	m_Actions.push_back(search_item);

	auto move_to_item_func = [&](IExamInterface* pInterface, SteeringPlugin_Output& steering)
	{
		const float EPSILON{ pInterface->Agent_GetInfo().GrabRange * pInterface->Agent_GetInfo().GrabRange };
		const Elite::Vector2 desired_location = pInterface->NavMesh_GetClosestPathPoint(m_ItemsInView.back().Location);

		steering.LinearVelocity = desired_location - pInterface->Agent_GetInfo().Position;
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= pInterface->Agent_GetInfo().MaxLinearSpeed;

		return pInterface->Agent_GetInfo().Position.DistanceSquared(m_ItemsInView.back().Location) < EPSILON;
	};
	GOAP::Action move_to_item("MoveToItem", 5, move_to_item_func);
	move_to_item.SetPrecondition(item_aquired, true);
	move_to_item.SetPrecondition(item_in_range, false);
	move_to_item.SetPrecondition(item_inventory_full, false);
	move_to_item.SetEffect(item_in_range, true);
	m_Actions.push_back(move_to_item);

	auto grab_item_func = [&](IExamInterface* pInterface, SteeringPlugin_Output& steering)
	{
		if (m_pInterface->Item_Grab({}, m_ItemsInView.back()))
		{
			if (m_pInterface->Inventory_AddItem(m_ItemsInInventory++, m_ItemsInView.back()))
			{
				m_ItemsInView.pop_back();
				m_ItemsInInventory++;
				return true;
			}
		}

		return false;
	};
	GOAP::Action grab_item("GrabItem", 5, grab_item_func);
	grab_item.SetPrecondition(item_in_range, true);
	grab_item.SetPrecondition(item_inventory_full, false);
	grab_item.SetEffect(item_in_range, false);
	grab_item.SetEffect(item_inventory_full, true);
	m_Actions.push_back(grab_item);

	// Here's the initial state...
	m_WorldState.SetVariable(house_aquired, false);
	m_WorldState.SetVariable(moving_to_house, false);
	m_WorldState.SetVariable(house_in_range, false);
	m_WorldState.SetVariable(item_aquired, false);
	m_WorldState.SetVariable(item_in_range, false);
	m_WorldState.SetVariable(item_inventory_full, false);

	// ...and the goal state
	GOAP::WorldState goal_house_aquired;
	goal_house_aquired.SetVariable(item_inventory_full, true);
	goal_house_aquired.priority = 50;

	// Fire up the A* planner
	auto steering = SteeringPlugin_Output();
	try
	{
		m_Plan = m_ASPlanner.FormulatePlan(m_WorldState, goal_house_aquired, m_Actions);
		std::cout << "Found a path!\n";
		for (std::vector<GOAP::Action>::reverse_iterator rit = rbegin(m_Plan); rit != rend(m_Plan); ++rit)
		{
			std::cout << rit->GetName() << std::endl;
		}
	} 
	catch (const std::exception&) 
	{
		std::cout << "Sorry, could not find a path!\n";
	}
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
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 36;
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
	auto steering = SteeringPlugin_Output();
	
	if (!m_Plan.empty())
	{
		auto& currentAction = m_Plan.back();
		if (currentAction.Execute(m_WorldState, m_pInterface, steering))
		{
			m_Plan.pop_back();
		}
	}

	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();

	//Use the navmesh to calculate the next navmesh point
	//auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);

	//OR, Use the mouse target
	auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(m_Target); //Uncomment this to use mouse position as guidance

	auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	auto vEntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)

	for (auto& e : vEntitiesInFOV)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo zoneInfo;
			m_pInterface->PurgeZone_GetInfo(e, zoneInfo);
			//std::cout << "Purge Zone in FOV:" << e.Location.x << ", "<< e.Location.y << "---Radius: "<< zoneInfo.Radius << std::endl;
		}
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

	//Simple Seek Behaviour (towards Target)
	//steering.LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
	//steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	//steering.LinearVelocity *= agentInfo.MaxLinearSpeed; //Rescale to Max Speed

	/*if (Distance(nextTargetPos, agentInfo.Position) < 2.f)
	{
		steering.LinearVelocity = Elite::ZeroVector2;
	}*/

	//steering.AngularVelocity = m_AngSpeed; //Rotate your character to inspect the world while walking
	steering.AutoOrient = true; //Setting AutoOrient to TRue overrides the AngularVelocity

	steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

	//SteeringPlugin_Output is works the exact same way a SteeringBehaviour output

//@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return steering;
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
