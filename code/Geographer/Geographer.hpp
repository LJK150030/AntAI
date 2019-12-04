#pragma once
#include "Blackboard.hpp"
#include "Math/IntVec2.hpp"

struct TileRecord;
struct NodeRecord;

enum eMapData
{
	UNKNOWN_MAP_DATA = -1,

	MAP_TILE_TYPE,
	MAP_FOOD,
	MAP_LAST_UPDATED,
	MAP_ANT_RESERVE,

	NUM_MAP_DATA
};

class Geographer
{
	friend class SearchGraph;
	
public:
	~Geographer();
	static void			Startup();
	static void			Shutdown();
	static void			Update();
	static Geographer	GetInstance();
	
	//Tile Queries
	static bool						DoesCoordHaveFood(const IntVec2& coord );
	static bool						IsSafeTile( const IntVec2& coord );
	static bool						IsTileSurrounded(const IntVec2& coord);
	static std::vector<IntVec2>		FourNeighbors( const IntVec2& coord );
	static std::vector<IntVec2>		EightNeighbors( const IntVec2& coord );
	static void						UpdateListOfFood(const IntVec2& coord);
	static int						HowMuchFoodCanISee();
	static float					GetHeatMapValueAt(const IntVec2& coord, eMapData map_data);
	static void						EdgeDetection(std::vector<float>& out_card_dir, const IntVec2& coord, int depth, eMapData heat_map);

	
	//Alter Records
	static void		SetMapDimensions( int width );
	static void		UpdatePerception();
	static IntVec2	AddAntToFoodTile( AgentID ant );
	static void		RemoveAntFromFoodTile( IntVec2 coord );
	
	//helpers
	static IntVec2	GetTileCoord( short tile_index );
	static IntVec2	GetCoordFromCardDir( eOrderCode dir, const IntVec2& start_coord, bool reverse_dir = false );
	static bool		IsValidCoord( const IntVec2& coord );
	static short	GetTileIndex(const IntVec2& coord);
	static float	ManhattanHeuristic(const IntVec2& start, const IntVec2& end);
	static float	OctileDistance(const IntVec2& start, const IntVec2& end);
	static float	EuclideanHeuristic(const IntVec2& start, const IntVec2& end);
	static void		ResetPathingMap();
	static void		GetCenteredSquareDis(std::vector<IntVec2>& out_coords, int depth, bool just_edge);
	static int		GetCenteredSquareCount(int depth, bool just_edge);
	
	//debugging
	static void DebugPrintCostMap(NodeRecord* map_records);
	static void DebugPrintDirectionMap(NodeRecord* map_records);
	static void DebugPrintPath(NodeRecord* map_records, const IntVec2& start, const IntVec2&  end);
	static void DebugPrintCostMap();
	static void DebugPrintDirectionMap();
	static void DebugPrintPath(const IntVec2& start, const IntVec2&  end);

	//Pathing jobs
	static eOrderCode GreedyMovement( const IntVec2& start, const IntVec2& end );
	static std::vector<eOrderCode> PathfindDijkstra( const IntVec2& start, const IntVec2& end );
	static std::vector<eOrderCode> PathfindAstar( const IntVec2& start, const IntVec2& end );

private:
	Geographer();

private:
	static Geographer*  s_instance;
	static int s_mapDimensions;
	static int s_mapTotalSize;

	static TileRecord s_perceivedMap[MAX_ARENA_TILES];
	static NodeRecord s_pathingMap[MAX_ARENA_TILES];
	const static NodeRecord DEFAULT_PATHING_MAP[MAX_ARENA_TILES];

	static std::vector<short> s_foodLoc;
};

//Structure to relative information together for one tile
//These will be saved in a 1D array where the index -> coord
struct TileRecord
{
	eTileType	m_tileType = TILE_TYPE_UNSEEN;
	bool		m_hasFood = false;
	int			m_lastUpdated = 0;
	AgentID		m_goingToThisTile = UINT_MAX;
};

//Structure to keep track of nodes
struct NodeRecord
{
	enum eNodeState {UNVISITED, OPEN, CLOSED};

	IntVec2		m_coord = IntVec2(-1, -1);
	short		m_parentIdx = -1;
	eOrderCode	m_actionTook = ORDER_HOLD;
	float		m_pathCost = FLT_MAX; 
	float		m_heuristic = 0.0f;
	eNodeState	m_nodeState = UNVISITED;
};

struct NodePriority
{
	NodePriority() = default;
	NodePriority(const short idx, const float priority):
		m_idx(idx), m_priority(priority) {}
	~NodePriority() = default;
	
	short m_idx = -1;
	float m_priority = -1.0f;

	friend bool operator==(const NodePriority& lhs, const NodePriority& rhs)
	{
		return lhs.m_idx == rhs.m_idx;
	}

	friend bool operator!=(const NodePriority& lhs, const NodePriority& rhs)
	{
		return lhs.m_idx != rhs.m_idx;
	}
	
	friend bool operator<(const NodePriority& lhs, const NodePriority& rhs)
	{
		return lhs.m_priority < rhs.m_priority;
	}

	friend bool operator>(const NodePriority& lhs, const NodePriority& rhs)
	{
		return lhs.m_priority > rhs.m_priority;
	}

};