//-----------------------------------------------------------------------------------------------
// ArenaPlayerInterface.hpp
//
// Common interface used by both the Arena Server and by each Arena Player (DLL).
// Defines enums, types, structures, and functions necessary to communicate with Arena Server.
//
#pragma once

//-----------------------------------------------------------------------------------------------
// Versioning
// 2: priority split to combatPriority and sacrificePriority
// 3: assorted changes made due to logic going in - mostly error codes 
// 4: add nutrients lost due to faults to the report
//-----------------------------------------------------------------------------------------------
constexpr int	COMMON_INTERFACE_VERSION_NUMBER		= 4;


//-----------------------------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------------------------
#if defined( ARENA_SERVER )
	#define DLL __declspec( dllimport )
#else // ARENA_PLAYER
	#define DLL __declspec( dllexport )
#endif

//-----------------------------------------------------------------------------------------------
// Common Typedefs
//-----------------------------------------------------------------------------------------------
typedef unsigned char	TeamID;		// 200+
typedef unsigned char	PlayerID;	// 100+
typedef unsigned int	AgentID;	// 100000001+  equal to:  (1000000 * playerID) + someUniqueID


//-----------------------------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------------------------

// Hard limit - actual limits will be provided during
// startup, and may (typically) be much lower
constexpr short	MAX_ARENA_WIDTH						= 256;
constexpr int	MAX_ARENA_TILES						= (MAX_ARENA_WIDTH * MAX_ARENA_WIDTH);
constexpr char	MAX_PLAYERS							= 32;
constexpr char	MAX_TEAMS							= MAX_PLAYERS;
constexpr char	MAX_PLAYERS_PER_TEAM				= MAX_PLAYERS;

constexpr int	MAX_AGENTS_PER_PLAYER				= 256;
constexpr int	MAX_ORDERS_PER_PLAYER				= MAX_AGENTS_PER_PLAYER;
constexpr int	MAX_REPORTS_PER_PLAYER				= 2 * MAX_AGENTS_PER_PLAYER;
constexpr int	MAX_AGENTS_TOTAL					= (MAX_PLAYERS * MAX_AGENTS_PER_PLAYER);

// special penalty values for digging/moving
constexpr int DIG_IMPOSSIBLE						= -1; 
constexpr int TILE_IMPASSABLE						= -1; 

//-----------------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
enum eFaultType : unsigned char 
{
	FAULT_NONE		= 0, 
	FAULT_TURN_START_DURATION_ELAPSED, 
	FAULT_ORDER_FETCH_DURATION_ELAPSED, 
	FAULT_TOTAL_TURN_DURATION_ELAPSED,
	FAULT_INVALID_ORDER, 
	FAULT_DOUBLE_ORDERED_AGENT,
	FAULT_DID_NOT_OWN_AGENT,
	FAULT_INVALID_AGENT_ID, 
};

//-----------------------------------------------------------------------------------------------
enum eAgentType : unsigned char
{
	AGENT_TYPE_SCOUT,
	AGENT_TYPE_WORKER, 
	AGENT_TYPE_SOLDIER, 
	AGENT_TYPE_QUEEN,

	NUM_AGENT_TYPES, 
	INVALID_AGENT_TYPE = 0xff
};

//-----------------------------------------------------------------------------------------------
enum eOrderCode
{
	ORDER_HOLD = 0,

	ORDER_MOVE_EAST,
	ORDER_MOVE_NORTH,
	ORDER_MOVE_WEST,
	ORDER_MOVE_SOUTH,

	ORDER_DIG_HERE, // exception: does not cause dig exhaustion!
	ORDER_DIG_EAST,
	ORDER_DIG_NORTH,
	ORDER_DIG_WEST,
	ORDER_DIG_SOUTH,

	ORDER_PICK_UP_FOOD,
	ORDER_PICK_UP_TILE, 
	ORDER_DROP_CARRIED_OBJECT,

	ORDER_BIRTH_SCOUT,
	ORDER_BIRTH_WORKER,
	ORDER_BIRTH_SOLDIER,
	ORDER_BIRTH_QUEEN,

	ORDER_SUICIDE,

	// EMOTES - To be implemented given infinite time
	ORDER_EMOTE_HAPPY,
	ORDER_EMOTE_SAD,
	ORDER_EMOTE_ANGRY,
	ORDER_EMOTE_TAUNT,
	ORDER_EMOTE_DANCE,

	NUM_ORDERS
};

//-----------------------------------------------------------------------------------------------
enum eAgentOrderResult : unsigned char
{
	// events
	AGENT_WAS_CREATED,
	AGENT_KILLED_BY_ENEMY,
	AGENT_KILLED_BY_WATER,
	AGENT_KILLED_BY_SUFFOCATION,
	AGENT_KILLED_BY_STARVATION,
	AGENT_KILLED_BY_PENALTY,

	// responses
	AGENT_ORDER_SUCCESS_HELD,
	AGENT_ORDER_SUCCESS_MOVED,
	AGENT_ORDER_SUCCESS_DUG,
	AGENT_ORDER_SUCCESS_PICKUP,
	AGENT_ORDER_SUCCESS_DROP,
	AGENT_ORDER_SUCCESS_BIRTHED,	// is a new unit
	AGENT_ORDER_SUCCESS_GAVE_BIRTH,	// unit spawned another unit
	AGENT_ORDER_SUCCESS_SUICIDE,

	AGENT_ORDER_ERROR_BAD_ANT_ID,
	AGENT_ORDER_ERROR_EXHAUSTED,
	AGENT_ORDER_ERROR_CANT_CARRY_FOOD,
	AGENT_ORDER_ERROR_CANT_CARRY_TILE,
	AGENT_ORDER_ERROR_CANT_BIRTH,
	AGENT_ORDER_ERROR_CANT_DIG_INVALID_TILE,
	AGENT_ORDER_ERROR_CANT_DIG_WHILE_CARRYING,
	AGENT_ORDER_ERROR_MOVE_BLOCKED,
	AGENT_ORDER_ERROR_OUT_OF_BOUNDS,
	AGENT_ORDER_ERROR_NO_FOOD_PRESENT,
	AGENT_ORDER_ERROR_ALREADY_CARRYING_FOOD,
	AGENT_ORDER_ERROR_SQUARE_NOT_EMPTY,
	AGENT_ORDER_ERROR_NOT_CARRYING,
	AGENT_ORDER_ERROR_INSUFFICIENT_FOOD,
	AGENT_ORDER_ERROR_MAXIMUM_POPULATION_REACHED,

	NUM_AGENT_STATUSES
};

//-----------------------------------------------------------------------------------------------
enum eAgentState : unsigned char 
{
	STATE_NORMAL = 0,
	STATE_DEAD, 
	STATE_HOLDING_FOOD, 
	STATE_HOLDING_SAND, 
	STATE_HOLDING_DIRT, 
	STATE_HOLDING_GRAVEL, 
//	STATE_GESTATING,  // we don't need this, since births are always immediate, just leaves queen exhausted
};

//-----------------------------------------------------------------------------------------------
enum eTileType : unsigned char 
{
	TILE_TYPE_AIR,				// open space, traversable by most/all agent types

	TILE_TYPE_SAND,				// porous (light); may be dug or traversed by certain ant types
	TILE_TYPE_DIRT,				// porous (medium); may be dug or traversed by certain ant types
	TILE_TYPE_GRAVEL,			// porous (heavy); may be dug or traversed by certain ant types

	TILE_TYPE_WATER,			// lethal; turns to a corpse bridge when ant walks onto it, killing the ant
	TILE_TYPE_CORPSE_BRIDGE,	// open space (like AIR), but can be dug to revert it back to water

	TILE_TYPE_STONE,			// impassable and indestructible
	NUM_TILE_TYPES,
	TILE_TYPE_UNSEEN = 0xff		// not currently visible to any ant, e.g. obscured by fog of war
};

// predeclared types;
struct DebugInterface;

//------------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
struct Color8
{
	Color8()
		: r(0)
		, g(0)
		, b(0)
		, a(255) 
	{}

	Color8( unsigned char redByte, 
		unsigned char greenByte, 
		unsigned char blueByte, 
		unsigned char alphaByte = 255 )
		: r(redByte)
		, g(greenByte)
		, b(blueByte)
		, a(alphaByte)
	{}

	unsigned char r, g, b, a; 
};


//-----------------------------------------------------------------------------------------------
struct VertexPC
{
	float x, y; 
	Color8 rgba; 
};


//-----------------------------------------------------------------------------------------------
struct AgentTypeInfo // information about a given agent type (e.g. AGENT_TYPE_WORKER)
{
	int		costToBirth;		// colony pays this many nutrients for a queen to birth one of these
	int		exhaustAfterBirth;	// queen who birthed an agent of this type gains +exhaustion
								//	int		exhaustOnCombat;	// Hey C4: who/what situation was this for again?
	int		upkeepPerTurn;		// colony pays this many nutrients per turn per one of these alive
	int		visibilityRange;	// sees tiles and agents up to this far away (taxicab distance)
	int		combatStrength;		// survives a duel if enemy agent has a lower combatStrength
	int		combatPriority;		// types with highest priority fight first
	int		sacrificePriority;	// types with highest priority are killed first when a sacrifice is required (water bridge or starvation)
	bool	canCarryFood;		// can pick up & drop food / porous tiles (ORDER_PICK_UP_, ORDER_DROP_)
	bool	canCarryTiles;		// can pick up & drop food / porous tiles (ORDER_PICK_UP_, ORDER_DROP_)
	bool	canBirth;			// can birth other ants (ORDER_BIRTH_)

	int		moveExhaustPenalties[NUM_TILE_TYPES]; // exhaustion gained after moving into each tile type
	int		digExhaustPenalties[NUM_TILE_TYPES];  // exhaustion gained after remote-digging each tile type
};


//-----------------------------------------------------------------------------------------------
struct MatchInfo // information about the match about to be played
{
	int				numPlayers;		// number of players (with unique PlayerIDs) in this match
	int				numTeams;		// <numPlayers> in brawl, 2 for 5v5, 1 for co-op survival
	
	short			mapWidth;		// width & height of [always square] map; tileX,tileY < mapWidth
	
	bool fogOfWar;				// if false, all tiles & agents are always visible
	bool teamSharedVision;		// if true, teammates share combined visibility
	bool teamSharedResources;	// if true, teammates share a single combined "nutrients" score

	int nutrientsEarnedPerFoodEatenByQueen;	// when a queen moves onto food, or food drops on her
	int nutrientLossPerAttackerStrength;	// colony loses nutrients whenever queen is attacked
	int numTurnsBeforeSuddenDeath;			// no new food will appear after this turn number
	int colonyMaxPopulation;				// cannot birth new agents if population is at max
	int startingNutrients;					// each colony starts with this many nutrients

	int foodCarryExhaustPenalty;	// exhaustion gained after each move while carrying food
	int tileCarryExhaustPenalty;	// exhaustion gained after each move while carrying a porous tile

	int combatStrengthQueenAuraBonus;		// agents may gain +combatStrength near a friendly queen
	int combatStrengthQueenAuraDistance;	// num tiles away (taxicab) a queen's bonus extends

	AgentTypeInfo agentTypeInfos[NUM_AGENT_TYPES];	// stats and capabilities for each agent type
};


//------------------------------------------------------------------------------------------------
struct PlayerInfo // server-assigned information about your Player instance in this match context
{
	PlayerID		playerID;	// 100+
	TeamID			teamID;		// 200+
	unsigned char	teamSize;	// 1 in free-for-all, 5 in 5v5, <numPlayers> in co-op survival
	Color8			color;		// use this in your debug drawing, etc.
};


//-----------------------------------------------------------------------------------------------
// Structure given for each of your agents (and/or each of your orders just previously issued)
//
struct AgentReport 
{
	int					agentID;		// your agent's unique ID #

	short				tileX;			// current tile coordinates in map; (0,0) is bottom-left
	short				tileY;
	short				exhaustion;		// number of turns inactive; non-HOLD orders fail if > 0

	eAgentType			type;			// type of agent (permanent/unique per agent)
	eAgentState			state;			// special status of agent (carrying something, etc.)
	eAgentOrderResult	result;			// result of agent's previously issued order
}; 

// -----------------------------------------------------------------------------------------------
struct ObservedAgent
{
	int			agentID;			// another Player's agent's unique ID #
	PlayerID	playerID;			// Player who owns this agent
	TeamID		teamID;				// Team the agent's Player is on

	short		tileX;				// just observed at these tile coordinates; (0,0) is bottom-left
	short		tileY;

	eAgentType	type;				// observed agent's type
	eAgentState	state;				// special status of agent (carrying something, etc.)
	eOrderCode	lastObservedAction;	// what this agent just did / was trying to do (last orders)
}; 


//------------------------------------------------------------------------------------------------
// An order you issue for one of your agents.
// Issuing more than one order for a given agent in the same turn is an illegal FAULT.
struct AgentOrder
{
	int agentID;
	eOrderCode order; 
};

//------------------------------------------------------------------------------------------------
// Fill this out when requested by TurnOrderRequest() to tell the server what actions each of
//	your agents is taking this turn.
//
// Orders [0] through [numberOfOrders-1] should each be a valid order for a valid, unique AgentID
struct PlayerTurnOrders
{
	AgentOrder orders[MAX_ORDERS_PER_PLAYER];
	int numberOfOrders; 
}; 


//------------------------------------------------------------------------------------------------
// Information about the server and match provided when PreGameStartup() is called by the server.
//
typedef void (*EventFunc)( const char* line ); 
typedef void (*RegisterEventFunc)( const char* eventName, EventFunc func );
struct StartupInfo
{
	MatchInfo	matchInfo;			// info about the match itself (numPlayers, mapWidth, etc.)
	PlayerInfo	yourPlayerInfo;		// info about your Player instance in this match context

	int expectedThreadCount;		// how many calls to PlayerThreadEntry() you will get (minimum of 1, typically 2+)
	double	maxTurnSeconds;			// must respond to ReceiveTurnState/TurnOrderRequest within this period
	int freeFaultCount;				// illegal faults allowed before Player is ejected (default 0)
	int nutrientPenaltyPerFault;	// nutrients lost per illegal fault committed (default infinity)
	int agentsKilledPerFault;		// agents kills as punishment per illegal fault (default infinity)

	DebugInterface* debugInterface; // debug draw/log function interface for your Player to use

	RegisterEventFunc RegisterEvent; // call this during PreGameStartup to register event & console commands

	// #ToDo: provide a pre-allocated fixed memory pool that AI can (must?) use for "heap" allocations
	// void*	memoryPool;
	// size_t	memoryPoolSizeInBytes; 
};


// -----------------------------------------------------------------------------------------------
struct ArenaTurnStateForPlayer
{
	// Information at the start of this turn; your next PlayerTurnOrders will be for this turn number
	int turnNumber;
	int currentNutrients;

	// fault reporting to clients
	int numFaults; 
	int nutrientsLostDueToFault;

	// Status reports for each of your living agents (and/or for each order you submitted)
	AgentReport agentReports[MAX_REPORTS_PER_PLAYER];
	int numReports;

	// List of all agents (besides yours) within your current visibility
	ObservedAgent observedAgents[MAX_AGENTS_TOTAL];
	int numObservedAgents; 

	// Copy of the entire map (mapWidth*mapWidth entries!); tile type is TILE_TYPE_UNSEEN for
	//	tiles not within your current visibility.  Unseen tiles always report "false" for food.
	eTileType observedTiles[MAX_ARENA_TILES];
	bool tilesThatHaveFood[MAX_ARENA_TILES]; 
}; 

// -----------------------------------------------------------------------------------------------
struct MatchResults
{
};


//------------------------------------------------------------------------------------------------
// Debug Render Interface
//
// Call debugInterface->LogText() to printf colored text to the dev console and log file
// Call debugInterface->QueueXXX() functions to request asynchronous debug draws on the server;
// Call debugInterface->FlushXXX() when finished; server will thereafter draw all queued items,
//	and continue to do so until your next call to FlushXXX(), which clears the previous items.
// !!! Items Queued for drawing will not appear until a Flush has been called afterwards.
//
// Debug drawing is done in world space; each Tile is 1x1, and (x,y) is the center of tile X,Y.
// For example, tile 7,3 extends from mins(6.5,2.5) to maxs(7.5,3.5), with center at (7.0,3.0).
// All drawing is clipped to world space, i.e. mins(-.5,-.5) to maxs(mapWidth-.5,mapWidth-.5).
//-----------------------------------------------------------------------------------------------
typedef void (*LogTextFunc)( char const* format, ... ); 
typedef void (*DrawVertexArrayFunc)( int count, const VertexPC* vertices );
typedef void (*DrawWorldTextFunc)( 
	float posX, float posY,
	float anchorU, float anchorV, 
	float height, // 1.0 would be text whose characters are one tile high
	Color8 color, 
	char const* format, ... ); 
typedef void (*FlushQueuedDrawsFunc)(); 
//------------------------------------------------------------------------------------------------
struct DebugInterface
{
	LogTextFunc				LogText;				// Print to dev console (and possibly log file)
	
	DrawWorldTextFunc		QueueDrawWorldText;		// Draw (aligned) overlay text in world space
	DrawVertexArrayFunc		QueueDrawVertexArray;	// Draw untextured geometry in world space
	FlushQueuedDrawsFunc	FlushQueuedDraws;		// Call after queuing to commit and show draws
};


//-----------------------------------------------------------------------------------------------
// DLL-EXE Interface
//-----------------------------------------------------------------------------------------------
#if !defined(ARENA_SERVER)
	extern "C"
	{
		// info collection
		DLL int GiveCommonInterfaceVersion(); 
		DLL const char* GivePlayerName(); 
		DLL const char* GiveAuthorName(); 

		// setup
		DLL void PreGameStartup( const StartupInfo& info ); 
		DLL void PostGameShutdown( const MatchResults& results ); 
		DLL void PlayerThreadEntry( int yourThreadIdx );

		// Turn
		DLL void ReceiveTurnState( const ArenaTurnStateForPlayer& state );
		DLL bool TurnOrderRequest( int turnNumber, PlayerTurnOrders* ordersToFill ); 
	}
#else 
	typedef int (*GiveCommandInterfaceVersionFunc)(); 
	typedef const char* (*GivePlayerNameFunc)(); 
	typedef const char* (*GiveAuthorNameFunc)(); 
	typedef void (*PreGameStartupFunc)( const StartupInfo& info ); 
	typedef void (*PostGameShutdownFunc)( const MatchResults& results ); 
	typedef void (*PlayerThreadEntryFunc)( int yourThreadIdx );
	typedef void (*ReceiveTurnStateFunc)( const ArenaTurnStateForPlayer& state );
	typedef bool (*TurnOrderRequestFunc)( int turnNumber, PlayerTurnOrders* ordersToFill ); 
#endif
