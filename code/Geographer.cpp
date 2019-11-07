#include "Geographer.hpp"

STATIC Geographer* Geographer::s_instance = nullptr;
STATIC uint Geographer::s_mapDimensions = 0;
STATIC uint Geographer::s_mapTotalSize = 0;
STATIC eTileType Geographer::s_observedTerrainMap[MAX_ARENA_TILES];
STATIC bool Geographer::s_observedFoodMap[MAX_ARENA_TILES];
STATIC eTileType Geographer::s_perceivedTerrainMap[MAX_ARENA_TILES];
STATIC bool Geographer::s_perceivedFoodMap[MAX_ARENA_TILES];


Geographer::Geographer()
{
}


Geographer::~Geographer()
{
}


STATIC void Geographer::Startup()
{
	Geographer startup = GetInstance();
	SetMapDimensions(g_matchInfo.mapWidth);
	CopyArenaData();
	UpdatePerception();

	
}


STATIC void Geographer::Shutdown()
{
	delete s_instance;
	s_instance = nullptr;
}


STATIC void Geographer::Update()
{
	CopyArenaData();
	UpdatePerception();
}


STATIC Geographer Geographer::GetInstance()
{
	if(s_instance == nullptr)
	{
		s_instance = new Geographer();
	}

	return *s_instance;
}


STATIC void Geographer::DebugPrintMap()
{
	
}


STATIC bool Geographer::DoesCoordHaveFood(short x, short y)
{
	int tileIdx = GetTileIndex( x, y ); 
	return s_perceivedFoodMap[tileIdx];
}


STATIC void Geographer::SetMapDimensions(const int width)
{
	s_mapDimensions = static_cast<uint>(width);
	s_mapTotalSize = width * width;
}


STATIC void Geographer::CopyArenaData()
{
	std::memcpy(s_observedTerrainMap, g_turnState.observedTiles, s_mapTotalSize);
	std::memcpy(s_observedFoodMap, g_turnState.tilesThatHaveFood, s_mapTotalSize);
}


STATIC void Geographer::UpdatePerception()
{
	// naive approach, loop through entire map
	// TODO: run Dijkstra for each agent's position, and only update those

	for(int tile_idx = 0; tile_idx < s_mapTotalSize; ++tile_idx)
	{
		if(s_observedTerrainMap[tile_idx] == TILE_TYPE_UNSEEN) continue;

		s_perceivedTerrainMap[tile_idx] = g_turnState.observedTiles[tile_idx];
		s_perceivedFoodMap[tile_idx] = g_turnState.tilesThatHaveFood[tile_idx];
	}
}


STATIC short Geographer::GetTileIndex(const short x, const short y)
{
	return y * s_mapDimensions + x; 
}


STATIC void Geographer::DebugPrintTerrainMap()
{

}

STATIC eOrderCode Geographer::GreedyMovement(short start_x, short start_y, short end_x, short end_y)
{
	// positive x go right: ORDER_MOVE_EAST
	// negative x go left: ORDER_MOVE_WEST
	// positive y go up: ORDER_MOVE_NORTH
	// negative y go down: ORDER_MOVE_SOUTH

	const short x = end_x - start_x;
	const short y = end_y - start_y;

	short x_abs = x;
	short y_abs = y;

	const bool x_is_neg = x < 0;
	const bool y_is_neg = y < 0;
	
	if(x_is_neg) x_abs *= -1;
	if(y_is_neg) y_abs *= -1;

	if(x_abs > y_abs) //moving in the x dimension
	{
		if(x_is_neg) //move left
		{
			return ORDER_MOVE_WEST;
		}
		else
		{
			return ORDER_MOVE_EAST;
		}
	}
	else //moving in the y dimension
	{
		if(y_is_neg)
		{
			return ORDER_MOVE_SOUTH;
		}
		else
		{
			return ORDER_MOVE_NORTH;
		}
	}
	
}
