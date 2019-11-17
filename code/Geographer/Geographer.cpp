#include "Geographer/Geographer.hpp"
#include "Architecture/Heap.hpp"
#include <string>
#include "Geographer/SearchGraph.hpp"


//--------------------------------------------------------------------------
// sharable data


STATIC Geographer*	Geographer::s_instance = nullptr;
STATIC int			Geographer::s_mapDimensions = 0;
STATIC int			Geographer::s_mapTotalSize = 0;
STATIC TileRecord	Geographer::s_perceivedMap[MAX_ARENA_TILES];


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
	return !(tile_type == TILE_TYPE_STONE || tile_type == TILE_TYPE_WATER || tile_type == TILE_TYPE_DIRT);
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
	}
}

//--------------------------------------------------------------------------
// Helper functions


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
		if(!map_records[node_idx].m_inOpenList && !map_records[node_idx].m_inClosedList)
		{
			continue;
		}
		
		const int cost = map_records[node_idx].m_pathCost;
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
	const short end_idx = GetTileIndex(end);

	//Map representation for node records
	NodeRecord* map = new NodeRecord[s_mapTotalSize];

	//Setup root node
	map[start_idx].m_coord = start;
	map[start_idx].m_parentIdx = -1;
	map[start_idx].m_actionTook = ORDER_HOLD;
	map[start_idx].m_pathCost = 0;
	map[start_idx].m_inOpenList = true;
	map[start_idx].m_inClosedList = false;

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

		std::vector<IntVec2> connections = Neighbors(map[current_idx].m_coord);
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
			if (map[new_node_idx].m_inClosedList) continue;
			
			//or if the node is in the open list
			if(map[new_node_idx].m_inOpenList)
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
				frontier.UpdateAtIdx(frontier_idx, NodePriority(new_node_idx, new_node.m_pathCost));
			}

			map[new_node_idx].m_coord = new_node.m_coord;
			map[new_node_idx].m_parentIdx = new_node.m_parentIdx;
			map[new_node_idx].m_actionTook = new_node.m_actionTook;
			map[new_node_idx].m_pathCost = new_node.m_pathCost;
			map[new_node_idx].m_inOpenList = true;
		}

		//Finished looking at all connections,
		map[current_idx].m_inOpenList = false;
		map[current_idx].m_inClosedList = true;
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

	//DebugPrintCostMap(map);
	//DebugPrintDirectionMap(map);
	//DebugPrintPath(map, start, end);
	
	delete[] map;
	return order_list;
}
