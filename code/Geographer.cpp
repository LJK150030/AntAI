#include "Geographer.hpp"
#include "Architecture/Heap.hpp"
#include <string>

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
	//CopyObservableArenaData();
	//UpdatePerception();

	
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


STATIC void Geographer::CopyObservableArenaData()
{
	std::memcpy(s_observedTerrainMap, g_turnState.observedTiles, s_mapTotalSize);
	std::memcpy(s_observedFoodMap, g_turnState.tilesThatHaveFood, s_mapTotalSize);
}


STATIC void Geographer::UpdatePerception()
{
	// naive approach, loop through entire map
	TODO("run Dijkstra for each agent's position, and only update those vision positions")

	for(uint tile_idx = 0; tile_idx < s_mapTotalSize; ++tile_idx)
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


STATIC short Geographer::GetTileIndex(const IntVec2& coord)
{
	return coord.y * s_mapDimensions + coord.x; 
}


STATIC void Geographer::GetTileCoord(short& x_out, short& y_out, uint tile_index)
{
	x_out = tile_index % s_mapDimensions;
	y_out = tile_index / s_mapDimensions;
}


STATIC void Geographer::DebugPrintTerrainMap()
{

}


STATIC void Geographer::DebugPrintCostMap(const std::map<short, NodeRecord>& closed_list)
{
	for (auto& it : closed_list)
	{
		const int cost = it.second.cost_so_far;
		std::string cost_string = std::to_string(cost);
		const IntVec2 pos = it.second.coord;
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
		
		const IntVec2 pos = it.second.coord;
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
		current_coord = GetReverseCoordFromCardDir(action_took, current_coord);
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


STATIC bool Geographer::IsSafeTile(const IntVec2& coord)
{
	int idx = GetTileIndex(coord);
	eTileType tile_type = s_perceivedTerrainMap[idx];

	return !(tile_type == TILE_TYPE_STONE || tile_type == TILE_TYPE_WATER || tile_type == TILE_TYPE_DIRT);
}


STATIC IntVec2 Geographer::GetCoordFromCardDir(const eOrderCode dir, const IntVec2& start_coord)
{
	switch(dir)
	{
		case ORDER_MOVE_EAST: 
		{
			return IntVec2(start_coord.x + 1, start_coord.y);
		}
		case ORDER_MOVE_NORTH:
		{
			return IntVec2(start_coord.x, start_coord.y + 1);
		}
		case ORDER_MOVE_WEST:
		{
			return IntVec2(start_coord.x - 1, start_coord.y);
		}
		case ORDER_MOVE_SOUTH:
		{
			return IntVec2(start_coord.x, start_coord.y - 1);
		}
		
		default: return start_coord;
	}
}

IntVec2 Geographer::GetReverseCoordFromCardDir(eOrderCode dir, const IntVec2& start_coord)
{
	switch(dir)
	{
	case ORDER_MOVE_EAST: 
	{
		return IntVec2(start_coord.x - 1, start_coord.y);
	}
	case ORDER_MOVE_NORTH:
	{
		return IntVec2(start_coord.x, start_coord.y - 1);
	}
	case ORDER_MOVE_WEST:
	{
		return IntVec2(start_coord.x + 1, start_coord.y);
	}
	case ORDER_MOVE_SOUTH:
	{
		return IntVec2(start_coord.x, start_coord.y + 1);
	}

	default: return start_coord;
	}
}


STATIC eOrderCode Geographer::GreedyMovement(const IntVec2& start, const IntVec2& end)
{
	// positive x go right: ORDER_MOVE_EAST
	// negative x go left: ORDER_MOVE_WEST
	// positive y go up: ORDER_MOVE_NORTH
	// negative y go down: ORDER_MOVE_SOUTH

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


TODO("Ask for unit type")
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

	// initialize the open list and closed list
	MinHeap<NodeRecord, MAX_PATH> open_list;

	TODO("This is obviosly needs to be an array")
	std::map<short, NodeRecord> closed_list;

	
	// Initialize the record for the start node
	NodeRecord start_node;
	start_node.coord = start;
	start_node.cost_so_far = 0;
	open_list.Push(start_node);

	
	// Remember the current node, we will need it to back track in the end
	NodeRecord current_node;

	//process through each node
	while(open_list.GetSize() > 0)
	{
		//find smallest element in the open list
		current_node = open_list.Pop();

		//if this is the goal, get out of the loop
		if((current_node.coord == end))
		{
			closed_list.emplace(GetTileIndex(current_node.coord), current_node);
			break;
		}
		
		//else, get the out going connections and loop through each
		for(int con_idx = 0; con_idx < 4; ++con_idx)
		{

			//save temp data for the current node
			TODO("With unit type, we can determine the exaust for a tile, and use as cost_so_far, rather than using a bool check if safe")
			const eOrderCode order_code = static_cast<eOrderCode>(con_idx + 1);
			const IntVec2 end_node = GetCoordFromCardDir(order_code, current_node.coord);
			const int end_node_cost = current_node.cost_so_far + 1;

			NodeRecord new_node;
			new_node.coord = end_node;
			new_node.cost_so_far = end_node_cost;
			new_node.action_took = order_code;
			
			const int index_in_open_list = open_list.GetIndex(new_node);
			bool update_open_list = false;
			short tile_idx = GetTileIndex(end_node);

			//determine if the connection is valid
			if(!IsValidCoord(end_node)) continue;
			if(!IsSafeTile(end_node)) continue;
						
			//if the node is in the closed list, then we can skip
			std::map<short, NodeRecord>::iterator node_itr;
			node_itr = closed_list.find(tile_idx);
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
		closed_list.emplace(GetTileIndex(current_node.coord), current_node);
	}

	//Either found the goal, or exhausted the open list
	if(current_node.coord != end)
	{
		order_list.push_back(ORDER_HOLD);
	}
	else
	{
		IntVec2 current_coord = current_node.coord;

		//pushing back coord, need to reverse
		while(current_coord != start)
		{
			eOrderCode action_took = closed_list[GetTileIndex(current_coord)].action_took;
			order_list.push_back(action_took);
			current_coord = GetReverseCoordFromCardDir(action_took, current_coord);
		}

		std::reverse(order_list.begin(),order_list.end());
	}

	//DebugPrintCostMap(closed_list);
	//DebugPrintDirectionMap(closed_list);
	DebugPrintPath(closed_list, start, end);
	return order_list;
}
