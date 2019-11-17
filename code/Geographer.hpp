#pragma once
#include "Blackboard.hpp"

struct NodeRecord;


class Geographer
{
public:
	~Geographer();
	static void Startup();
	static void Shutdown();
	static void Update();
	static Geographer GetInstance();
	
	//Tile Queries
	static bool DoesCoordHaveFood(const IntVec2& coord );
	static bool IsSafeTile( short tile_index );

	//Alter Records
	static void SetMapDimensions( int width );
	static void CopyObservableArenaData();
	static void UpdatePerception();
	static void UpdatePerceptionAt( const IntVec2& coord );

	//Helpers Functions
	static short GetTileIndex(const IntVec2& coord);
	static IntVec2 GetTileCoord( short tile_index );
	static bool IsValidCoord( const IntVec2& coord );
	static IntVec2 GetCoordFromCardDir( eOrderCode dir, const IntVec2& start_coord, bool reverse_dir = false );
	static short GetTileIndexFromCardDir( eOrderCode dir, short start_idx, bool reverse_dir = false );

	
	static std::vector<IntVec2> Neighbors( short coord );
	static std::vector<IntVec2> Neighbors( const IntVec2& coord );
	
	//Debug drawing
	static void DebugPrintCostMap(const std::map<short, NodeRecord>& closed_list);
	static void DebugPrintDirectionMap(const std::map<short, NodeRecord>& closed_list);
	static void DebugPrintPath(const std::map<short, NodeRecord>& closed_list, const IntVec2& start, const IntVec2&  end);
	
	//Pathing jobs
	static eOrderCode GreedyMovement( const IntVec2& start, const IntVec2& end );
	static std::vector<eOrderCode> PathfindDijkstra( const IntVec2& start, const IntVec2& end );
	TODO("use the strategy pattern to make only one Pathfind job, but state what method to use to pathfind")
	
private:
	Geographer();

private:
	static Geographer*  s_instance;
	static int s_mapDimensions;
	static int s_mapTotalSize;

	//these are maps of all the tiles the ants can currently can see
	static eTileType s_observedTerrainMap[MAX_ARENA_TILES];
	static bool s_observedFoodMap[MAX_ARENA_TILES];

	//this is what we believe the filed to be
	static eTileType s_perceivedTerrainMap[MAX_ARENA_TILES];
	static bool s_perceivedFoodMap[MAX_ARENA_TILES];
};


//Structure to keep track of nodes
struct NodeRecord
{
	short coord_idx = -1;
	eOrderCode action_took = ORDER_HOLD;
	int cost_so_far = -1;

	bool operator==(const NodeRecord& other_record) const
	{
		return coord_idx == other_record.coord_idx;
	}


	bool operator!=(const NodeRecord& other_record) const
	{
		return coord_idx != other_record.coord_idx;
	}


	bool operator>(const NodeRecord& other_record) const
	{
		return cost_so_far > other_record.cost_so_far;
	}


	bool operator<(const NodeRecord& other_record) const
	{
		return cost_so_far < other_record.cost_so_far;
	}
};