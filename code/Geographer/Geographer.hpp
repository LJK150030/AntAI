#pragma once
#include "Blackboard.hpp"

struct TileRecord;
struct NodeRecord;


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
	static std::vector<IntVec2>		Neighbors( const IntVec2& coord );

	//Alter Records
	static void		SetMapDimensions( int width );
	static void		UpdatePerception();
	
	//helpers
	static IntVec2	GetTileCoord( short tile_index );
	static IntVec2	GetCoordFromCardDir( eOrderCode dir, const IntVec2& start_coord, bool reverse_dir = false );
	static bool		IsValidCoord( const IntVec2& coord );
	static short	GetTileIndex(const IntVec2& coord);

	
	//debugging
	static void DebugPrintCostMap(NodeRecord* map_records);
	static void DebugPrintDirectionMap(NodeRecord* map_records);
	static void DebugPrintPath(NodeRecord* map_records, const IntVec2& start, const IntVec2&  end);
	
	//Pathing jobs
	static eOrderCode GreedyMovement( const IntVec2& start, const IntVec2& end );
	static std::vector<eOrderCode> PathfindDijkstra( const IntVec2& start, const IntVec2& end );
	
private:
	Geographer();

private:
	static Geographer*  s_instance;
	static int s_mapDimensions;
	static int s_mapTotalSize;

	static TileRecord s_perceivedMap[MAX_ARENA_TILES];
};

//Structure to relative information together for one tile
//These will be saved in a 1D array where the index -> coord
struct TileRecord
{
	eTileType	m_tileType = TILE_TYPE_UNSEEN;
	bool		m_hasFood = false;
	int			m_lastUpdated = INT_MAX;
};

//Structure to keep track of nodes
struct NodeRecord
{
	IntVec2		m_coord = IntVec2(-1, -1);
	short		m_parentIdx = -1;
	eOrderCode	m_actionTook = ORDER_HOLD;
	float		m_pathCost = FLT_MAX;
	bool		m_inOpenList = false;
	bool		m_inClosedList = false;
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