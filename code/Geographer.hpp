#pragma once
#include "Blackboard.hpp"

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
	static void CopyArenaData();
	static void UpdatePerception();

	//helpers and debuggers
	static short GetTileIndex(short x, short y);
	static void GetTileCoord( short& x_out, short& y_out, uint tile_index );
	static void DebugPrintTerrainMap();

	//Pathing jobs
	static eOrderCode GreedyMovement( short start_x, short start_y, short end_x, short end_y );
	
private:
	Geographer();

private:
	static Geographer*  s_instance;
	static uint s_mapDimensions;
	static uint s_mapTotalSize;

	static eTileType s_observedTerrainMap[MAX_ARENA_TILES];
	static bool s_observedFoodMap[MAX_ARENA_TILES];
	
	static eTileType s_perceivedTerrainMap[MAX_ARENA_TILES];
	static bool s_perceivedFoodMap[MAX_ARENA_TILES];
};
