#pragma once
#pragma region MISC
#include <string>

struct SteeringPlugin_Output
{
	Elite::Vector2 LinearVelocity = { 0.f,0.f };
	float AngularVelocity = 0.f;
	bool AutoOrient = true;
	bool RunMode = false;
};

struct PluginInfo
{
	std::string BotName = "Minion";
	std::string Student_FirstName = "Not Set";
	std::string Student_LastName = "Not Set";
	std::string Student_Class = "Not Set";
};	

struct GameDebugParams //Debuggin Purposes only (Ignored during release build)
{
	GameDebugParams(bool spawnEnemies = true, int enemyCount = 20, bool godMode = false, bool autoFollowCam = false)
	{
		SpawnEnemies = spawnEnemies;
		GodMode = godMode;
		AutoFollowCam = autoFollowCam;
		EnemyCount = enemyCount;
	}

	bool SpawnEnemies = true; //Spawn enemies?
	int EnemyCount = 20; //Amount of enemies?
	int ItemCount = 40; //Amount of items?
	bool GodMode = false; //Use GodMode? (Invincible)
	bool IgnoreEnergy = false; //Ignore energy depletion
	bool AutoFollowCam = false; //Auto follow the player
	bool RenderUI = false; //Render Player UI (Parameters)
	bool AutoGrabClosestItem = false; //Auto Grab closest item (Item_Grab)
	std::string LevelFile = "GameLevel.gppl"; //Level to load?
	int Seed = -1; //Seed for random generator
	int StartingDifficultyStage = 0; // Overwrites the difficulty stage
	bool InfiniteStamina = false; // Agent has infinite stamina
	bool SpawnDebugPistol = false; // Spawns pistol with 1000 ammo at start
	bool SpawnDebugShotgun = false; // Spawns shotgun with 1000 ammo at start
	bool SpawnPurgeZonesOnMiddleClick = false; // Middle mouse clicks spawns purge zone
	bool SpawnZombieOnRightClick = false; // Right mouse clicks spawns zombie
	bool PrintDebugMessages = true;
	bool ShowDebugItemNames = true;
};
#pragma endregion

#pragma region ENTITIES
//Enumerations
//************
enum class eEntityType
{
	ITEM,
	ENEMY,
	PURGEZONE,

	//@END
	_LAST = PURGEZONE
};

enum class eItemType
{
	PISTOL,
	SHOTGUN,
	MEDKIT,
	FOOD,
	GARBAGE,
	RANDOM_DROP, //Internal Only
	RANDOM_DROP_WITH_CHANCE, //Internal Only

	//@END
	_LAST = GARBAGE
};

enum class eEnemyType
{
	DEFAULT,
	ZOMBIE_NORMAL,
	ZOMBIE_RUNNER,
	ZOMBIE_HEAVY,
	RANDOM_ENEMY, //Internal Only
	//...

	//@END
	_LAST = ZOMBIE_HEAVY
};


//Structures
//**********
struct StatisticsInfo
{
	int Score; //Current Score
	float Difficulty; //Current difficulty (0 > 1 > ... / Easy > Normal > Hard)
	float TimeSurvived; //Total time survived
	float KillCountdown; //Make sure to make a kill before this timer runs out

	int NumEnemiesKilled; //Amount of enemies killed
	int NumEnemiesHit; //Amount of enemy hits
	int NumItemsPickUp; //Amount of items picked up
	int NumMissedShots; //Shots missed after firing
	int NumChkpntsReached; //Amount of checkpoints reached

};

struct HouseInfo
{
	Elite::Vector2 Location;
	Elite::Vector2 Size;
};

enum class Corner{ BottomLeft, TopLeft, BottomRight, TopRight };

struct HouseInfo_Extended : HouseInfo
{
	int itemsLootedSinceLastVisit;
	int itemsLootedReactivation;
	bool AreaSearched;
	Corner NextCornerAreaSearch;
	Corner NextCornerHouseSearch;

	std::vector<Elite::Vector2> GetRectPoints() const
	{
		return
		{
			{ Location - Size / 2.f },
			{ Location.x - Size.x / 2.f , Location.y + Size.y / 2.f },
			{ Location + Size / 2.f},
			{ Location.x + Size.x / 2.f, Location.y - Size.y / 2.f }
		};
	};

	std::vector<Elite::Vector2> GetRectPointsInnerWall() const
	{
		return
		{
			{ Location - Size / 2.f + Elite::Vector2{2.5f, 2.5f} },
			{ Location.x - Size.x / 2.f + 2.5f , Location.y + Size.y / 2.f - 2.5f },
			{ Location + Size / 2.f - Elite::Vector2{2.5f, 2.5f}},
			{ Location.x + Size.x / 2.f - 2.5f, Location.y - Size.y / 2.f + 2.5f }
		};
	};

	std::vector<Elite::Vector2> GetMidPoints() const
	{
		return
		{
			{ Location.x + Size.x / 3.5f, Location.y},
			{ Location.x, Location.y - Size.y / 3.5f},
			{ Location.x - Size.x / 3.5f, Location.y},
			{ Location.x, Location.y + Size.y / 3.5f},
		};
	}

	std::vector<Elite::Vector2> GetCorners() const
	{
		return
		{
			{ Location - Size / 4.5f },
			{ Location.x - Size.x / 4.5f, Location.y + Size.y / 4.5f },
			{ Location + Size / 4.5f },
			{ Location.x + Size.x / 4.5f, Location.y - Size.y / 4.5f }
		};
	};

	bool IsPositionInsideWall(const Elite::Vector2& pos) const
	{
		auto rectPoints{ GetRectPoints() };
		const float wallThickness{ 0.75f };
		
		return
			pos.x >= rectPoints[0].x && pos.x <= rectPoints[3].x && pos.y >= rectPoints[0].y - wallThickness && pos.y <= rectPoints[0].y + wallThickness || // Bottom wall
			pos.x >= rectPoints[1].x && pos.x <= rectPoints[2].x && pos.y >= rectPoints[1].y - wallThickness && pos.y <= rectPoints[1].y + wallThickness || // Top wall
			pos.x >= rectPoints[0].x - wallThickness && pos.x <= rectPoints[0].x + wallThickness && pos.y >= rectPoints[0].y && pos.y <= rectPoints[1].y || // Left Wall
			pos.x >= rectPoints[2].x - wallThickness && pos.x <= rectPoints[2].x + wallThickness && pos.y >= rectPoints[2].y && pos.y <= rectPoints[3].y; // Right Wall

	}

	bool HasRecentlyVisited() const { return itemsLootedSinceLastVisit < itemsLootedReactivation; }
	bool operator==(const HouseInfo_Extended& rhs) { return Location == rhs.Location; }
	bool operator==(const HouseInfo& rhs) { return Location == rhs.Location; }
};

struct EnemyInfo
{
	eEnemyType Type;
	Elite::Vector2 Location;
	Elite::Vector2 LinearVelocity;

	int EnemyHash = 0;
	float Size;
	float Health;
};

struct ItemInfo
{
	eItemType Type;
	Elite::Vector2 Location;

	int ItemHash = 0;
};

struct PurgeZoneInfo
{
	Elite::Vector2 Center;
	float Radius = 0.0f;

	int ZoneHash = 0;
	bool operator!=(const PurgeZoneInfo& other) { return Center != other.Center; }
};

struct EntityInfo
{
	eEntityType Type;
	Elite::Vector2 Location;

	int EntityHash = 0;
	bool operator==(const EntityInfo& other) const { return Location == other.Location; }
	bool operator==(EntityInfo&& other) const { return Location == other.Location; }
};

struct WorldInfo
{
	Elite::Vector2 Center;
	Elite::Vector2 Dimensions;
};

struct AgentInfo
{
	float Stamina;
	float Health;
	float Energy;
	bool RunMode;
	bool IsInHouse;
	bool Bitten;		// agent was bitten by a zombie this frame (for internal use)
	bool WasBitten;		// agent was bitten by a zombie recently (0.5 seconds)
	bool Death;

	float FOV_Angle;
	float FOV_Range;

	Elite::Vector2 LinearVelocity;
	float AngularVelocity;
	float CurrentLinearSpeed;
	Elite::Vector2 Position;
	float Orientation;
	float MaxLinearSpeed;
	float MaxAngularSpeed;
	float GrabRange;
	float AgentSize;
};
#pragma endregion