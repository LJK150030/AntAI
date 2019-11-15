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
	
	//accessor
	static Geographer GetInstance();
	static void DebugPrintMap();
	static bool DoesCoordHaveFood( short x, short y );

	//mutators
	static void SetMapDimensions( int width );
	static void CopyObservableArenaData();
	static void UpdatePerception();

	//helpers
	static short GetTileIndex(short x, short y);
	static short GetTileIndex(const IntVec2& coord);
	static void GetTileCoord( short& x_out, short& y_out, uint tile_index );
	static bool IsValidCoord( const IntVec2& coord );
	static bool IsSafeTile( const IntVec2& coord );
	static IntVec2 GetCoordFromCardDir( eOrderCode dir, const IntVec2& start_coord );
	static IntVec2 GetReverseCoordFromCardDir( eOrderCode dir, const IntVec2& start_coord );

	//debugging
	static void DebugPrintTerrainMap();
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
	static uint s_mapDimensions;
	static uint s_mapTotalSize;

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
	IntVec2 coord = IntVec2(-1, -1);
	eOrderCode action_took = ORDER_HOLD;
	int cost_so_far = -1;
	//do not need a list of connections, we know we can always go cardinal directions

	bool operator==(const NodeRecord& other_record) const
	{
		return Geographer::GetTileIndex(coord) == Geographer::GetTileIndex(other_record.coord);
	}


	bool operator!=(const NodeRecord& other_record) const
	{
		return Geographer::GetTileIndex(coord) != Geographer::GetTileIndex(other_record.coord);
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