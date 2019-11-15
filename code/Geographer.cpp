#include "Geographer.hpp"
#include "Architecture/Heap.hpp"
#include <string>

STATIC Geographer* Geographer::s_instance = nullptr;
STATIC int Geographer::s_mapDimensions = 0;
STATIC int Geographer::s_mapTotalSize = 0;

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
}


STATIC void Geographer::Shutdown()
{
	delete s_instance;
	s_instance = nullptr;
}


STATIC void Geographer::Update()
{
	CopyObservableArenaData();
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


STATIC bool Geographer::DoesCoordHaveFood(const IntVec2& coord)
{
	const short tile_idx = GetTileIndex(coord); 
	return s_perceivedFoodMap[tile_idx];
}


STATIC void Geographer::SetMapDimensions(const int width)
{
	s_mapDimensions = width;
	s_mapTotalSize = width * width;
}


STATIC void Geographer::CopyObservableArenaData()
{
	std::memcpy(s_observedTerrainMap, g_turnState.observedTiles, s_mapTotalSize);
	std::memcpy(s_observedFoodMap, g_turnState.tilesThatHaveFood, s_mapTotalSize);
}


STATIC void Geographer::UpdatePerception()
{
	// naive approach, loop through entire map
	TODO("run Breath first search for each agent's position, and only update those vision positions")

	for(int tile_idx = 0; tile_idx < s_mapTotalSize; ++tile_idx)
	{
		if(s_observedTerrainMap[tile_idx] == TILE_TYPE_UNSEEN) continue;

		s_perceivedTerrainMap[tile_idx] = g_turnState.observedTiles[tile_idx];
		s_perceivedFoodMap[tile_idx] = g_turnState.tilesThatHaveFood[tile_idx];		
	}
}


STATIC short Geographer::GetTileIndex(const IntVec2& coord)
{
	return static_cast<short>(coord.y * s_mapDimensions + coord.x); 
}


STATIC IntVec2 Geographer::GetTileCoord(const short tile_index)
{
	IntVec2 coord;
	coord.x = tile_index % s_mapDimensions;
	coord.y = tile_index / s_mapDimensions;

	return coord;
}


STATIC void Geographer::DebugPrintCostMap(const std::map<short, NodeRecord>& closed_list)
{
	for (auto& it : closed_list)
	{
		const int cost = it.second.cost_so_far;
		std::string cost_string = std::to_string(cost);
		const IntVec2 pos = GetTileCoord(it.second.coord_idx);
		const float x_coord = static_cast<float>(pos.x);
		const float y_coord = static_cast<float>(pos.y);
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 0.8f, Color8(255, 255, 255), cost_string.c_str());
	}

	g_debugInterface->FlushQueuedDraws();
}


STATIC void Geographer::DebugPrintDirectionMap(const std::map<short, NodeRecord>& closed_list)
{
	for (auto& it : closed_list)
	{
		eOrderCode action = it.second.action_took;
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
		
		const IntVec2 pos = GetTileCoord(it.second.coord_idx);
		const float x_coord = static_cast<float>(pos.x);
		const float y_coord = static_cast<float>(pos.y);
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 1.0f, Color8(255, 255, 255), dir_string.c_str());
	}

	g_debugInterface->FlushQueuedDraws();
}


STATIC void Geographer::DebugPrintPath(const std::map<short, NodeRecord>& closed_list, const IntVec2& start, const IntVec2&  end)
{
	IntVec2 current_coord = end;
	
	while(current_coord != start)
	{
		eOrderCode action_took = closed_list.at(GetTileIndex(current_coord)).action_took;
		current_coord = GetCoordFromCardDir(action_took, current_coord, true);
		const float x_coord = static_cast<float>(current_coord.x);
		const float y_coord = static_cast<float>(current_coord.y);
		
		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 0.8f, Color8(255, 255, 255), "@");
	}

	g_debugInterface->FlushQueuedDraws();
}


STATIC bool Geographer::IsValidCoord(const IntVec2& coord)
{
	bool x_dim = coord.x >= 0 && coord.x < static_cast<int>(s_mapDimensions);
	bool y_dim = coord.y >= 0 && coord.y < static_cast<int>(s_mapDimensions);
	return x_dim && y_dim;
}


TODO("Ask for Unit type")
STATIC bool Geographer::IsSafeTile( short tile_index )
{
	const eTileType tile_type = s_perceivedTerrainMap[tile_index];
	return !(tile_type == TILE_TYPE_STONE || tile_type == TILE_TYPE_WATER || tile_type == TILE_TYPE_DIRT);
}

STATIC short Geographer::GetTileIndexFromCardDir(const eOrderCode dir, short start_idx, bool reverse_dir)
{
	return GetTileIndex(GetCoordFromCardDir(dir, GetTileCoord(start_idx), reverse_dir));
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


STATIC std::vector<IntVec2> Geographer::Neighbors(const short coord)
{
	return Neighbors(GetTileCoord(coord));
}

STATIC std::vector<IntVec2> Geographer::Neighbors(const IntVec2& coord)
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
		if(!IsValidCoord(test_coord)) continue;
		result.emplace_back(test_coord);
	}

	return result;
}


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


TODO("Ask for unit type: 1. we can determine the exaust for a tile, and use as cost_so_far. 2. We can determine if a tile is walkable or not")
TODO("Make a generic Pathfind job, pass a strategy to decided pathing")
TODO("Create Pathing objects that we can pool")
STATIC std::vector<eOrderCode> Geographer::PathfindDijkstra( const IntVec2& start, const IntVec2& end )
{
	const short start_idx = GetTileIndex(start);
	const short end_idx = GetTileIndex(end);

	std::vector<eOrderCode> order_list;
	if(start == end) //early out
	{
		order_list.push_back(ORDER_HOLD);
		return order_list;
	}

	//TODO: store as an array, and look up nodes as open, closed, connection, and cost
	MinHeap<NodeRecord, MAX_PATH> open_list;
	std::map<short, NodeRecord> closed_list;

	
	NodeRecord start_node;
	start_node.coord_idx = start_idx;
	start_node.action_took = ORDER_HOLD;
	start_node.cost_so_far = 0;
	open_list.Push(start_node);

	NodeRecord current_node;
	while(open_list.GetSize() > 0)
	{
		current_node = open_list.Pop();

		if((current_node.coord_idx == end_idx))
		{
			closed_list.emplace(current_node.coord_idx, current_node);
			break;
		}
		
		std::vector<IntVec2> connections = Neighbors(current_node.coord_idx);
		for(int con_idx = 0; con_idx < static_cast<int>(connections.size()); ++con_idx)
		{

			NodeRecord new_node;
			new_node.coord_idx = GetTileIndex(connections[con_idx]);
			new_node.cost_so_far = current_node.cost_so_far + 1;
			new_node.action_took = static_cast<eOrderCode>(con_idx + 1);

			//this is a linear search, every frame... this is terrible
			const int index_in_open_list = open_list.GetIndex(new_node);
			bool update_open_list = false;

			if(!IsSafeTile(new_node.coord_idx)) continue;
						
			//if the node is in the closed list, then we can skip
			std::map<short, NodeRecord>::iterator node_itr;
			node_itr = closed_list.find(new_node.coord_idx);
			if (node_itr != closed_list.end()) continue;
			
			//or if the node is in the open list
			if(index_in_open_list >= 0)
			{
				// need to know if the node in the open list is better or worse
				const NodeRecord open_list_node = open_list.GetReferenceAt(index_in_open_list);

				//if the node in the list is better, we can skip it
				if(open_list_node.cost_so_far <= new_node.cost_so_far) continue;

				// if our new node is better, highly unlikely but, we need to update
				// the node in the list with the lower cost, and the action we took
				update_open_list = true;
			}

			// we either have an unvisited node, or need to updated a node
			if(!update_open_list) // most likely
			{
				open_list.Push(new_node);
			}
			else
			{
				open_list.UpdateAtIdx(index_in_open_list, new_node);
			}
		}

		//Finished looking at all connections,
		closed_list.emplace(current_node.coord_idx, current_node);
	}

	//Either found the goal, or exhausted the open list
	if(current_node.coord_idx != end_idx)
	{
		order_list.push_back(ORDER_HOLD);
	}
	else
	{
		short current_idx = current_node.coord_idx;

		//pushing back coord, need to reverse
		while(current_idx != start_idx)
		{
			eOrderCode action_took = closed_list[current_idx].action_took;
			order_list.push_back(action_took);
			current_idx = GetTileIndexFromCardDir(action_took, current_idx, true);
		}

		std::reverse(order_list.begin(),order_list.end());
	}

	//DebugPrintCostMap(closed_list);
	//DebugPrintDirectionMap(closed_list);
	DebugPrintPath(closed_list, start, end);
	return order_list;
}
