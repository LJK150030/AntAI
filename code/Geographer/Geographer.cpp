#include "Geographer/Geographer.hpp"
#include "Architecture/Heap.hpp"
#include <string>
#include <algorithm>
#include <random>
#include "Geographer/SearchGraph.hpp"
#include "Math/MathUtils.hpp"
#include "Architecture/ErrorWarningAssert.hpp"


//--------------------------------------------------------------------------
// sharable data


STATIC Geographer*			Geographer::s_instance = nullptr;
STATIC int					Geographer::s_mapDimensions = 0;
STATIC int					Geographer::s_mapTotalSize = 0;
STATIC TileRecord			Geographer::s_perceivedMap[MAX_ARENA_TILES];
STATIC NodeRecord			Geographer::s_pathingMap[MAX_ARENA_TILES];
STATIC std::vector<short>	Geographer::s_foodLoc = std::vector<short>();
STATIC std::vector<short>	Geographer::s_enemyLoc = std::vector<short>();

STATIC const NodeRecord		Geographer::DEFAULT_PATHING_MAP[MAX_ARENA_TILES];


//--------------------------------------------------------------------------
// Geographer


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
}


STATIC void Geographer::Shutdown()
{
	delete s_instance;
	s_instance = nullptr;
}


STATIC void Geographer::Update()
{
	UpdatePerception();
	//UpdateListOfFood();
}


STATIC Geographer Geographer::GetInstance()
{
	if(s_instance == nullptr)
	{
		s_instance = new Geographer();
	}

	return *s_instance;
}


//--------------------------------------------------------------------------
// Tile Queries


STATIC bool Geographer::DoesCoordHaveFood(const IntVec2& coord)
{
	const short tile_idx = GetTileIndex(coord); 
	return s_perceivedMap[tile_idx].m_hasFood;
}


TODO("Ask for Unit type")
STATIC bool Geographer::IsSafeTile( const IntVec2& coord )
{
	short tile_index = GetTileIndex(coord);
	const eTileType tile_type = s_perceivedMap[tile_index].m_tileType;
	return !(tile_type == TILE_TYPE_STONE);
}

bool Geographer::IsTileSurrounded(const IntVec2& coord)
{
	std::vector<IntVec2> neighboring_tiles = FourNeighbors(coord);
	int num_stone_walls = 0;
	
	for(int tile_idx = 0; tile_idx < 4; ++tile_idx)
	{
		if(neighboring_tiles[tile_idx] == IntVec2::NEG_ONE)
		{
			continue;
		}

		short tile_serial = GetTileIndex(neighboring_tiles[tile_idx]);
		
		if (s_perceivedMap[tile_serial].m_tileType == TILE_TYPE_STONE)
		{
			++num_stone_walls;
		}
	}

	return num_stone_walls >= 4;
}


STATIC std::vector<IntVec2> Geographer::FourNeighbors(const IntVec2& coord)
{
	IntVec2 dir[] = {
		IntVec2(1, 0),
		IntVec2(0, 1),
		IntVec2(-1, 0),
		IntVec2(0, -1)};

	std::vector<IntVec2> result;

	for (auto& idx : dir)
	{
		IntVec2 test_coord(coord.x + idx.x, coord.y + idx.y);
		if(!IsValidCoord(test_coord)) 
		{
			result.emplace_back(IntVec2::NEG_ONE);
		}
		else
		{
			result.emplace_back(test_coord);
		}
	}

	return result;
}

std::vector<IntVec2> Geographer::EightNeighbors(const IntVec2& coord)
{
	IntVec2 dir[] = {
		IntVec2(1, 0),
		IntVec2(1, 1),
		IntVec2(0, 1),
		IntVec2(-1, 1),
		IntVec2(-1, 0),
		IntVec2(-1, -1),
		IntVec2(0, -1),
		IntVec2(1, -1)};

	std::vector<IntVec2> result;

	for (auto& idx : dir)
	{
		IntVec2 test_coord(coord.x + idx.x, coord.y + idx.y);
		if(!IsValidCoord(test_coord) || !IsSafeTile(test_coord)) 
		{
			result.emplace_back(IntVec2(-1, -1));
		}
		else
		{
			result.emplace_back(test_coord);
		}
	}

	return result;
}


STATIC void Geographer::UpdateListOfFood(const IntVec2& coord)
{
	s_foodLoc.clear();
	const short start_idx = GetTileIndex(coord);
	const int total_nodes_to_search = s_mapTotalSize;
	int nodes_searched = 1;
	
	//Setup root node
	s_pathingMap[start_idx].m_coord = coord;
	s_pathingMap[start_idx].m_pathCost = 0;
	s_pathingMap[start_idx].m_nodeState = NodeRecord::OPEN;
	
	MinHeap<NodePriority> frontier(s_mapTotalSize);
	frontier.Push(NodePriority(start_idx, s_pathingMap[start_idx].m_pathCost));

	NodeRecord current_node;
	while(frontier.GetSize() > 0)
	{
		const NodePriority priority_node = frontier.Pop();
		const short current_idx = priority_node.m_idx;
		current_node = s_pathingMap[current_idx];

		std::vector<IntVec2> connections = EightNeighbors(current_node.m_coord);
		for(int con_idx = 0; con_idx < static_cast<int>(connections.size()); ++con_idx)
		{
			++nodes_searched;
			//GetNeighbors will always return 4 elements, if invalid it returns (-1,-1)
			if(connections[con_idx] == IntVec2(-1, -1)) continue;

			NodeRecord new_node;
			new_node.m_coord = connections[con_idx];
			new_node.m_pathCost = nodes_searched;
			
			const short new_node_idx = GetTileIndex(new_node.m_coord);

			//if the node is in the closed list, then we may skip or remove it from the closed list
			switch(s_pathingMap[new_node_idx].m_nodeState)
			{
			case NodeRecord::CLOSED:
			case NodeRecord::OPEN:
				{
					break;
				}
			case NodeRecord::UNVISITED:
				{
					s_pathingMap[new_node_idx].m_coord = new_node.m_coord;
					s_pathingMap[new_node_idx].m_pathCost = nodes_searched;
					s_pathingMap[new_node_idx].m_nodeState = NodeRecord::OPEN;
					frontier.Push(NodePriority(new_node_idx, new_node.m_pathCost));
					break;
				}
			}
		}

		//Finished looking at all connections,
		s_pathingMap[current_idx].m_nodeState = NodeRecord::CLOSED;

		if(s_perceivedMap[current_idx].m_goingToThisTile != UINT_MAX) continue;
		if(s_perceivedMap[current_idx].m_hasFood)
		{
			s_foodLoc.push_back(current_idx);
		}
		
		if(nodes_searched == total_nodes_to_search)	break;
	}

		
	std::reverse(s_foodLoc.begin(),s_foodLoc.end());
		
	ResetPathingMap();
}

int Geographer::HowMuchFoodCanISee()
{
	return s_foodLoc.size();
}

int Geographer::HowManyEnemiesCanISee()
{
	return s_enemyLoc.size();
}

IntVec2 Geographer::GetNextEnemyCoord()
{
	if(!s_enemyLoc.empty())
	{
		IntVec2 enemy_coord(GetTileCoord(s_enemyLoc.back()));
		s_enemyLoc.pop_back();
		return enemy_coord;
	}

	return IntVec2::NEG_ONE;
}

float Geographer::GetHeatMapValueAt(const IntVec2& coord, eMapData map_data)
{
	short coord_idx = GetTileIndex(coord);

	//kinda bad, assuming all map data is floats
	switch(map_data)
	{
		case MAP_TILE_TYPE:
		{
			return static_cast<float>(s_perceivedMap[coord_idx].m_tileType);
			break;
		}
		case MAP_FOOD:
		{
			return static_cast<float>(s_perceivedMap[coord_idx].m_hasFood);
			break;
		}
		case MAP_LAST_UPDATED:
		{
			return static_cast<float>(s_perceivedMap[coord_idx].m_lastUpdated);
			break;
		}
		case MAP_ANT_RESERVE:
		{
			return static_cast<float>(s_perceivedMap[coord_idx].m_goingToThisTile);
			break;
		}
		default:
		{
			return -1;	
			
		}
	}
}

void Geographer::EdgeDetection(std::vector<float>& out_card_dir, const IntVec2& coord, int depth, eMapData heat_map)
{
	//same as the cardinal directions enums
	// 0: EAST
	// 1: NORTH
	// 2: WEST
	// 3: SOUTH

	float east_summation = 0.0f;
	float north_summation = 0.0f;
	float west_summation = 0.0f;
	float south_summation = 0.0f;

	for(int x_dir = 0; x_dir <= depth; ++x_dir)
	{
		IntVec2 check_coord = coord;
		const int y_dif = depth - x_dir;
		if(x_dir == 0 && y_dif == 0)
		{
			//center (only for 0 depth case)
			east_summation += GetHeatMapValueAt(coord, heat_map);
			north_summation += GetHeatMapValueAt(coord, heat_map);
			west_summation += GetHeatMapValueAt(coord, heat_map);
			south_summation += GetHeatMapValueAt(coord, heat_map);
		}
		else if(x_dir == 0)
		{
			//out_coords.emplace_back(0, 1 * y_dif); NORTH
			//out_coords.emplace_back(0, -1 * y_dif); SOUTH
			IntVec2 north_coord(coord.x, coord.y + 1 * y_dif);
			IntVec2 south_coord(coord.x, coord.y + -1 * y_dif);

			if(IsValidCoord(north_coord))	
				north_summation += GetHeatMapValueAt(north_coord, heat_map);

			if(IsValidCoord(south_coord))	
				south_summation += GetHeatMapValueAt(south_coord, heat_map);
		}
		else if(y_dif == 0)
		{
			//out_coords.emplace_back(1 * x_dir, 0); EAST
			//out_coords.emplace_back(-1 * x_dir, 0); WEST

			IntVec2 east_coord(coord.x + 1 * x_dir, coord.y);
			IntVec2 west_coord(coord.x + -1 * y_dif, coord.y);

			if(IsValidCoord(east_coord))	
				east_summation += GetHeatMapValueAt(east_coord, heat_map);

			if(IsValidCoord(west_coord))	
				west_summation += GetHeatMapValueAt(west_coord, heat_map);
		}
		else
		{
			// this will be a bit more hands on
			//out_coords.emplace_back(1 * x_dir, 1 * y_dif);
			//out_coords.emplace_back(1 * x_dir, -1 * y_dif);
			//out_coords.emplace_back(-1 * x_dir, 1 * y_dif);
			//out_coords.emplace_back(-1 * x_dir, -1 * y_dif);

			IntVec2 NE_coord(coord.x + 1 * x_dir, coord.y + 1 * y_dif);
			IntVec2 NW_coord(coord.x + 1 * x_dir, coord.y + -1 * y_dif);
			IntVec2 SE_coord(coord.x + -1 * x_dir, coord.y + 1 * y_dif);
			IntVec2 SW_coord(coord.x + -1 * x_dir, coord.y + -1 * y_dif);

			//even

			//odd
			
			
			//need to consider even and odd depth
		}
	}

	out_card_dir.push_back(east_summation);
	out_card_dir.push_back(north_summation);
	out_card_dir.push_back(west_summation);
	out_card_dir.push_back(south_summation);
}


//--------------------------------------------------------------------------
// Alter Records


STATIC void Geographer::SetMapDimensions(const int width)
{
	s_mapDimensions = width;
	s_mapTotalSize = width * width;
}


STATIC void Geographer::UpdatePerception()
{
	for(int tile_idx = 0; tile_idx < s_mapTotalSize; ++tile_idx)
	{
		if(g_turnState.observedTiles[tile_idx] == TILE_TYPE_UNSEEN) continue;

		s_perceivedMap[tile_idx].m_tileType = g_turnState.observedTiles[tile_idx];
		s_perceivedMap[tile_idx].m_hasFood = g_turnState.tilesThatHaveFood[tile_idx];
		s_perceivedMap[tile_idx].m_lastUpdated = g_turnState.turnNumber;
	}

	s_enemyLoc.clear();
	if(g_turnState.numObservedAgents > 0)
	{
		for(int enemy_num = 0; enemy_num < g_turnState.numObservedAgents; ++enemy_num)
		{
			ObservedAgent enemy = g_turnState.observedAgents[enemy_num];
			if (enemy.type == AGENT_TYPE_SCOUT) continue;
			IntVec2 enemy_coord(enemy.tileX, enemy.tileY);
			s_enemyLoc.push_back(GetTileIndex(enemy_coord));
		}
	}
}

IntVec2 Geographer::AddAntToFoodTile(AgentID ant)
{
	if(s_foodLoc.empty()) return IntVec2(-1, -1);

	short food_idx = s_foodLoc.back();
	s_foodLoc.pop_back();
	s_perceivedMap[food_idx].m_goingToThisTile = ant;
	return GetTileCoord(food_idx);
}

void Geographer::RemoveAntFromFoodTile(IntVec2 coord)
{
	short food_idx = GetTileIndex(coord);
	s_perceivedMap[food_idx].m_goingToThisTile = UINT_MAX;
}

//--------------------------------------------------------------------------
// Helper functions


STATIC short Geographer::GetTileIndex(const IntVec2& coord)
{
	return static_cast<short>(coord.y * s_mapDimensions + coord.x); 
}

STATIC float Geographer::ManhattanHeuristic(const IntVec2& start, const IntVec2& end)
{
	//|x1 - x2| + |y1 - y2|
	float dx = static_cast<float>(Abs(end.x - start.x));
	float dy = static_cast<float>(Abs(end.y - start.y));
	return dx + dy;
}

float Geographer::OctileDistance(const IntVec2& start, const IntVec2& end)
{	
	float dx = static_cast<float>(Abs(start.x - end.x));
	float dy = static_cast<float>(Abs(start.y - end.y));
	return (dx + dy) + (1.41421356237f - 2.0f) * Min(dx, dy);
}

float Geographer::EuclideanHeuristic(const IntVec2& start, const IntVec2& end)
{
	return Sqrt( static_cast<float>((end.x - start.x) * (end.x - start.x) +
		(end.y - start.y) * (end.y - start.y)));
}

STATIC void Geographer::ResetPathingMap()
{
	memcpy(s_pathingMap, DEFAULT_PATHING_MAP, MAX_ARENA_TILES * sizeof(NodeRecord));
}

STATIC void Geographer::GetCenteredSquareDis(std::vector<IntVec2>& out_coords, int depth, bool just_edge)
{
	if(just_edge)
	{
		// C_{4,0} = { (0, 0) }
		// C_{4,1} = { (-1, 0), (0, -1), (0, 1), (1, 0) } 
		// C_{4,1} = { (-2, 0), (-1, -1), (-1, 1), (0, -2), (0, 2), (1, -1), (1, 1), (2, 0) }

		for(int x_dir = 0; x_dir <= depth; ++x_dir)
		{
			const int y_dif = depth - x_dir;
			if(x_dir == 0 && y_dif == 0)
			{
				out_coords.emplace_back(0, 0);
			}
			else if(x_dir == 0)
			{
				out_coords.emplace_back(0, 1 * y_dif);
				out_coords.emplace_back(0, -1 * y_dif);
			}
			else if(y_dif == 0)
			{
				out_coords.emplace_back(1 * x_dir, 0);
				out_coords.emplace_back(-1 * x_dir, 0);
			}
			else
			{
				out_coords.emplace_back(1 * x_dir, 1 * y_dif);
				out_coords.emplace_back(1 * x_dir, -1 * y_dif);
				out_coords.emplace_back(-1 * x_dir, 1 * y_dif);
				out_coords.emplace_back(-1 * x_dir, -1 * y_dif);
			}
		}
	}
	else
	{
		for(int num = 0; num <= depth; num++)
		{
			for(int x_dir = 0; x_dir <= num; ++x_dir)
			{
				const int y_dif = num - x_dir;

				if(x_dir == 0 && y_dif == 0)
				{
					out_coords.emplace_back(0, 0);
				}
				else if(x_dir == 0)
				{
					out_coords.emplace_back(0, 1 * y_dif);
					out_coords.emplace_back(0, -1 * y_dif);
				}
				else if(y_dif == 0)
				{
					out_coords.emplace_back(1 * x_dir, 0);
					out_coords.emplace_back(-1 * x_dir, 0);
				}
				else
				{
					out_coords.emplace_back(1 * x_dir, 1 * y_dif);
					out_coords.emplace_back(1 * x_dir, -1 * y_dif);
					out_coords.emplace_back(-1 * x_dir, 1 * y_dif);
					out_coords.emplace_back(-1 * x_dir, -1 * y_dif);
				}
			}
		}
	}
}

STATIC int Geographer::GetCenteredSquareCount(int depth, bool just_edge)
{
	int offset = depth + 1; //1 vision means one tiles away from us
	
	if(just_edge)
	{
		// C_{4,0} = 0
		// C_{4,1} = 1
		// C_{4,2} = 4
		// C_{4,3} = 9
		// C_{4,4} = 16
		// C_{4,n} = n^2
		return offset*offset;
	} // else we want the entire diamond
	{
		// C_{4,0} = 0
		// C_{4,1} = 1
		// C_{4,2} = 5
		// C_{4,3} = 13
		// C_{4,4} = 25
		// C_{4,n} = n^2 + (n - 1)^2
		return offset*offset + (offset - 1)*(offset - 1);
	}
}


STATIC IntVec2 Geographer::GetTileCoord(const short tile_index)
{
	IntVec2 coord;
	coord.x = tile_index % s_mapDimensions;
	coord.y = tile_index / s_mapDimensions;

	return coord;
}


STATIC bool Geographer::IsValidCoord(const IntVec2& coord)
{
	bool x_dim = coord.x >= 0 && coord.x < static_cast<int>(s_mapDimensions);
	bool y_dim = coord.y >= 0 && coord.y < static_cast<int>(s_mapDimensions);
	return x_dim && y_dim;
}


STATIC IntVec2 Geographer::GetCoordFromCardDir(const eOrderCode dir, const IntVec2& start_coord, bool reverse_dir)
{
	switch(dir)
	{
	case ORDER_MOVE_EAST: 
	{
		return reverse_dir ? 
			IntVec2(start_coord.x - 1, start_coord.y): 
			IntVec2(start_coord.x + 1, start_coord.y);
	}
	case ORDER_MOVE_NORTH:
	{
		return reverse_dir ? 
			IntVec2(start_coord.x, start_coord.y - 1): 
			IntVec2(start_coord.x, start_coord.y + 1);
	}
	case ORDER_MOVE_WEST:
	{
		return reverse_dir ? 
			IntVec2(start_coord.x + 1, start_coord.y): 
			IntVec2(start_coord.x - 1, start_coord.y);
	}
	case ORDER_MOVE_SOUTH:
	{
		return reverse_dir ? 
			IntVec2(start_coord.x, start_coord.y + 1): 
			IntVec2(start_coord.x, start_coord.y - 1);
	}

	default: return start_coord;
	}
}


//--------------------------------------------------------------------------
// Debug Drawing


STATIC void Geographer::DebugPrintCostMap(NodeRecord* map_records)
{
	for (int node_idx = 0; node_idx < s_mapTotalSize; ++node_idx)
	{
		if(map_records[node_idx].m_nodeState == NodeRecord::UNVISITED)
		{
			continue;
		}
		
		const int cost = static_cast<int>(map_records[node_idx].m_pathCost);
		std::string cost_string	= std::to_string(cost);
		const IntVec2 pos = map_records[node_idx].m_coord;
		const float x_coord = static_cast<float>(pos.x);
		const float y_coord = static_cast<float>(pos.y);
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 0.8f, Color8(255, 255, 255), cost_string.c_str());
	}

	g_debugInterface->FlushQueuedDraws();
}


STATIC void Geographer::DebugPrintDirectionMap(NodeRecord* map_records)
{
	for (int node_idx = 0; node_idx < s_mapTotalSize; ++node_idx)
	{
		eOrderCode action = map_records[node_idx].m_actionTook;
		std::string dir_string;

		switch(action)
		{
			case ORDER_MOVE_EAST: 
			{
				dir_string = ">";
				break;
			}
			case ORDER_MOVE_NORTH:
			{
				dir_string = "A";
				break;
			}
			case ORDER_MOVE_WEST:
			{
				dir_string = "<";
				break;
			}
			case ORDER_MOVE_SOUTH:
			{
				dir_string = "V";
				break;
			}
		}
		
		const IntVec2 pos = map_records[node_idx].m_coord;
		const float x_coord = static_cast<float>(pos.x);
		const float y_coord = static_cast<float>(pos.y);
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 1.0f, Color8(255, 255, 255), dir_string.c_str());
	}

	g_debugInterface->FlushQueuedDraws();
}


STATIC void Geographer::DebugPrintPath(NodeRecord* map_records, const IntVec2& start, const IntVec2&  end)
{
	IntVec2 current_coord = end;

	do
	{
		const float x_coord = static_cast<float>(current_coord.x);
		const float y_coord = static_cast<float>(current_coord.y);
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 0.8f, Color8(255, 255, 255), "@");

		eOrderCode action_took = map_records[GetTileIndex(current_coord)].m_actionTook;
		current_coord = GetCoordFromCardDir(action_took, current_coord, true);
	}while(current_coord != start);

	g_debugInterface->FlushQueuedDraws();
}

void Geographer::DebugPrintCostMap()
{
	for (int node_idx = 0; node_idx < s_mapTotalSize; ++node_idx)
	{
		if(s_pathingMap[node_idx].m_nodeState == NodeRecord::UNVISITED)
		{
			continue;
		}

		const int cost = static_cast<int>(s_pathingMap[node_idx].m_pathCost);
		std::string cost_string	= std::to_string(cost);
		const IntVec2 pos = s_pathingMap[node_idx].m_coord;
		const float x_coord = static_cast<float>(pos.x);
		const float y_coord = static_cast<float>(pos.y);
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 0.8f, Color8(255, 255, 255), cost_string.c_str());
	}

	g_debugInterface->FlushQueuedDraws();
}

void Geographer::DebugPrintDirectionMap()
{
	for (int node_idx = 0; node_idx < s_mapTotalSize; ++node_idx)
	{
		eOrderCode action = s_pathingMap[node_idx].m_actionTook;
		std::string dir_string;

		switch(action)
		{
		case ORDER_MOVE_EAST: 
		{
			dir_string = ">";
			break;
		}
		case ORDER_MOVE_NORTH:
		{
			dir_string = "A";
			break;
		}
		case ORDER_MOVE_WEST:
		{
			dir_string = "<";
			break;
		}
		case ORDER_MOVE_SOUTH:
		{
			dir_string = "V";
			break;
		}
		}

		const IntVec2 pos = s_pathingMap[node_idx].m_coord;
		const float x_coord = static_cast<float>(pos.x);
		const float y_coord = static_cast<float>(pos.y);
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 1.0f, Color8(255, 255, 255), dir_string.c_str());
	}

	g_debugInterface->FlushQueuedDraws();
}

void Geographer::DebugPrintPath(const IntVec2& start, const IntVec2& end)
{
	IntVec2 current_coord = end;

	do
	{
		const float x_coord = static_cast<float>(current_coord.x);
		const float y_coord = static_cast<float>(current_coord.y);
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 0.8f, Color8(255, 255, 255), "@");

		eOrderCode action_took = s_pathingMap[GetTileIndex(current_coord)].m_actionTook;
		current_coord = GetCoordFromCardDir(action_took, current_coord, true);
	}while(current_coord != start);

	g_debugInterface->FlushQueuedDraws();
}


//--------------------------------------------------------------------------
//Pathing algorithums

STATIC eOrderCode Geographer::GreedyMovement(const IntVec2& start, const IntVec2& end)
{
	const short x = end.x - start.x;
	const short y = end.y - start.y;

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


TODO("Ask for unit type: 1. we can determine the exaust for a tile, and use as m_pathCost. 2. We can determine if a tile is walkable or not")
TODO("Make a generic Pathfind job, pass a strategy to decided pathing")
TODO("Create Pathing objects that we can pool")
STATIC std::vector<eOrderCode> Geographer::PathfindDijkstra( const IntVec2& start, const IntVec2& end )
{
	std::vector<eOrderCode> order_list;
	if(start == end)
	{
		order_list.push_back(ORDER_HOLD);
		return order_list;
	}

	const short start_idx = GetTileIndex(start);

	//Map representation for node records
	NodeRecord* map = new NodeRecord[s_mapTotalSize];

	//Setup root node
	map[start_idx].m_coord = start;
	map[start_idx].m_parentIdx = -1;
	map[start_idx].m_actionTook = ORDER_HOLD;
	map[start_idx].m_pathCost = 0;
	map[start_idx].m_nodeState = NodeRecord::OPEN;

	MinHeap<NodePriority> frontier(s_mapTotalSize);
	frontier.Push(NodePriority(start_idx, map[start_idx].m_pathCost));

	NodeRecord current_node;
	while(frontier.GetSize() > 0)
	{
		const NodePriority priority_node = frontier.Pop();
		const short current_idx = priority_node.m_idx;
		current_node = map[current_idx];
		
		if((current_node.m_coord == end))
		{
			//node should already be in the map, might need to update something
			break;
		}

		std::vector<IntVec2> connections = FourNeighbors(map[current_idx].m_coord);
		for(int con_idx = 0; con_idx < static_cast<int>(connections.size()); ++con_idx)
		{
			//GetNeighbors will always return 4 elements, if invalid it returns (-1,-1)
			if(connections[con_idx] == IntVec2(-1, -1)) continue;
			
			NodeRecord new_node;
			new_node.m_coord = connections[con_idx];
			new_node.m_parentIdx = GetTileIndex(current_node.m_coord);
			new_node.m_actionTook = static_cast<eOrderCode>(con_idx + 1);
			new_node.m_pathCost = static_cast<float>(current_node.m_pathCost + 1);
			
			const short new_node_idx = GetTileIndex(new_node.m_coord);
			bool update_open_list = false;
			
			//if the node is in the closed list, then we can skip
			if (map[new_node_idx].m_nodeState == NodeRecord::CLOSED) continue;
			
			//or if the node is in the open list
			if(map[new_node_idx].m_nodeState == NodeRecord::OPEN)
			{
				// need to know if the node in the open list is better or worse
				//if the node in the list is better, we can skip it
				if(map[new_node_idx].m_pathCost <= new_node.m_pathCost) continue;

				// if our new node is better, highly unlikely but, we need to update
				// the node in the list with the lower cost, and the action we took
				update_open_list = true;
			}

			// we either have an unvisited node, or need to updated a node
			if(!update_open_list) // most likely
			{
				frontier.Push(NodePriority(new_node_idx, new_node.m_pathCost));
			}
			else //Update node with new value
			{
				//this is linear time, make sure we really need to do this
				const int frontier_idx = frontier.GetIndex(NodePriority(new_node_idx, new_node.m_pathCost));
				if(frontier_idx == -1) break;
				frontier.UpdateAtIdx(frontier_idx, NodePriority(new_node_idx, new_node.m_pathCost));
			}

			map[new_node_idx].m_coord = new_node.m_coord;
			map[new_node_idx].m_parentIdx = new_node.m_parentIdx;
			map[new_node_idx].m_actionTook = new_node.m_actionTook;
			map[new_node_idx].m_pathCost = new_node.m_pathCost;
			map[new_node_idx].m_nodeState = NodeRecord::OPEN;
		}

		//Finished looking at all connections,
		map[current_idx].m_nodeState = NodeRecord::CLOSED;
	}

	//Either found the goal, or exhausted the open list
	if(current_node.m_coord != end)
	{
		order_list.push_back(ORDER_HOLD);
	}
	else
	{
		short current_idx = GetTileIndex(current_node.m_coord);

		//pushing back coord, need to reverse
		while(current_idx != start_idx)
		{
			eOrderCode action_took = map[current_idx].m_actionTook;
			order_list.push_back(action_took);
			current_idx = map[current_idx].m_parentIdx;
		}

		std::reverse(order_list.begin(),order_list.end());
	}

	DebugPrintCostMap(map);
	//DebugPrintDirectionMap(map);
	//DebugPrintPath(map, start, end);
	
	delete[] map;
	return order_list;
}

//I don't like copy and past, however, I don't believe I will ever use Dijkstra again
std::vector<eOrderCode> Geographer::PathfindAstar(const IntVec2& start, const IntVec2& end, bool for_worker)
{
	std::vector<eOrderCode> order_list;
	if(start == end) //redundent check
	{
		order_list.push_back(ORDER_HOLD);
		return order_list;
	}

	const short start_idx = GetTileIndex(start);
	int max_expansions = s_mapDimensions * 2;
	int num_expansion = 0;
	
	//Setup root node
	s_pathingMap[start_idx].m_coord = start;
	s_pathingMap[start_idx].m_parentIdx = -1;
	s_pathingMap[start_idx].m_actionTook = ORDER_HOLD;
	s_pathingMap[start_idx].m_pathCost = 0;
	s_pathingMap[start_idx].m_heuristic =  ManhattanHeuristic(start, end);
	s_pathingMap[start_idx].m_nodeState = NodeRecord::OPEN;

	MinHeap<NodePriority> frontier(s_mapTotalSize);
	frontier.Push(NodePriority(start_idx, s_pathingMap[start_idx].m_pathCost));

	NodeRecord current_node;
	while(frontier.GetSize() > 0)
	{
		const NodePriority priority_node = frontier.Pop();
		const short current_idx = priority_node.m_idx;
		current_node = s_pathingMap[current_idx];

		++num_expansion;
		if((current_node.m_coord == end) || num_expansion == max_expansions)	break;

		std::vector<IntVec2> connections = FourNeighbors(current_node.m_coord);
		for(int con_idx = 0; con_idx < static_cast<int>(connections.size()); ++con_idx)
		{
			//GetNeighbors will always return 4 elements, if invalid it returns (-1,-1)
			if(connections[con_idx] == IntVec2(-1, -1)) continue;

			NodeRecord new_node;
			new_node.m_coord = connections[con_idx];
			new_node.m_parentIdx = GetTileIndex(current_node.m_coord);
			new_node.m_actionTook = static_cast<eOrderCode>(con_idx + 1);
			new_node.m_heuristic = ManhattanHeuristic(new_node.m_coord, end) +
				0.03f * OctileDistance(new_node.m_coord, end);

			float exhaust_penalty = 0.0f;
			switch (s_perceivedMap[GetTileIndex(new_node.m_coord)].m_tileType)
			{
			case TILE_TYPE_AIR:
				exhaust_penalty = 0.0f;
				break;
			case TILE_TYPE_STONE:
				exhaust_penalty = 1000.0f;
				break;
			case TILE_TYPE_WATER:
				if (for_worker)
					exhaust_penalty = 2.0f;
				else
					exhaust_penalty = 1000.0f;
				break;
			case TILE_TYPE_CORPSE_BRIDGE:
				exhaust_penalty = 0.0f;
				break;
			case TILE_TYPE_DIRT:
				if (for_worker)
					exhaust_penalty = 1.0f;
				else
					exhaust_penalty = 1000.0f;
			break;
			}
			
			
			new_node.m_pathCost = current_node.m_pathCost + exhaust_penalty;
			
			const short new_node_idx = GetTileIndex(new_node.m_coord);

			//if the node is in the closed list, then we may skip or remove it from the closed list
			switch(s_pathingMap[new_node_idx].m_nodeState)
			{
			case NodeRecord::CLOSED:
			case NodeRecord::OPEN:
				{
					// need to know if the node in the open list is better or worse
					//if the node in the list is better, we can skip it
					if(s_pathingMap[new_node_idx].m_pathCost <= new_node.m_pathCost) continue;

					// if our new node is better, highly unlikely but, we need to update
					// the node in the list with the lower cost, and the action we took
					// this is linear time, make sure we really need to do this
					const int frontier_idx = frontier.GetIndex(NodePriority(new_node_idx, new_node.m_pathCost));
					if(frontier_idx == -1) break;
					frontier.UpdateAtIdx(frontier_idx, NodePriority(new_node_idx, new_node.m_pathCost + new_node.m_heuristic));
					break;
				}
			case NodeRecord::UNVISITED:
				{
					frontier.Push(NodePriority(new_node_idx, new_node.m_pathCost + new_node.m_heuristic));
					break;
				}
			}

			s_pathingMap[new_node_idx].m_coord = new_node.m_coord;
			s_pathingMap[new_node_idx].m_parentIdx = new_node.m_parentIdx;
			s_pathingMap[new_node_idx].m_actionTook = new_node.m_actionTook;
			s_pathingMap[new_node_idx].m_pathCost = new_node.m_pathCost;
			s_pathingMap[new_node_idx].m_nodeState = NodeRecord::OPEN;
		}

		//Finished looking at all connections,
		s_pathingMap[current_idx].m_nodeState = NodeRecord::CLOSED;
	}

	//Either found the goal, or exhausted the open list
	if(current_node.m_coord != end)
	{
		order_list.push_back(ORDER_HOLD);
	}
	else
	{
		short current_idx = GetTileIndex(current_node.m_coord);

		//pushing back coord, need to reverse
		while(current_idx != start_idx)
		{
			eOrderCode action_took = s_pathingMap[current_idx].m_actionTook;
			order_list.push_back(action_took);
			current_idx = s_pathingMap[current_idx].m_parentIdx;
		}

		std::reverse(order_list.begin(),order_list.end());
		
		//DebugPrintCostMap();
		//DebugPrintDirectionMap();
		//DebugPrintPath(start, end);
	}

	ResetPathingMap();
	return order_list;
}
