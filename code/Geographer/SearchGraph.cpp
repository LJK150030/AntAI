#include "Geographer/SearchGraph.hpp"
#include "Architecture/Queue.hpp"
#include "Architecture/QueueIterator.hpp"
#include <string>


SearchGraph::SearchGraph(eQueueType queue_type, const TileRecord map[MAX_ARENA_TILES])
{
	memcpy(m_map, map, Geographer::s_mapDimensions * sizeof(TileRecord));

	switch(queue_type)
	{
	case QUEUE_FIFO:
		{
			m_frontierIterator = reinterpret_cast<QueueIterator*>(m_frontier.CreateFifoIterator());
			//m_frontierIterator->m_type = QUEUE_FIFO;
			break;
		}
	case QUEUE_LIFO:
		{
			break;
		}
	case QUEUE_PRIORITY:
		{
			break;
		}
	}
}


SearchGraph::~SearchGraph()
{
}

bool SearchGraph::FloodFill( std::vector<TileRecord>& out_tiles, const short start_tile_index,
                             const int depth )
{
	//start node
	const short root_idx = start_tile_index;
	m_searchSpace[root_idx].m_parentIdx = -1;
	m_searchSpace[root_idx].m_actionTook = ORDER_HOLD;
	m_searchSpace[root_idx].m_pathCost = 0;
	m_searchSpace[root_idx].m_inOpenList = true;
	m_searchSpace[root_idx].m_inClosedList = false;
	
	
	if(m_searchSpace[root_idx].m_pathCost > depth)
	{
		TileRecord new_tile;
		new_tile.m_tileType =  g_turnState.observedTiles[root_idx];
		new_tile.m_hasFood = g_turnState.tilesThatHaveFood[root_idx];
		new_tile.m_lastUpdated = g_turnState.turnNumber;

		out_tiles.push_back(new_tile);
		return true;
	};

	m_frontierIterator->Push(root_idx);

	//while(!m_frontier->IsEmpty())
	//{
		const short node_idx = m_frontierIterator->Peak();
		m_frontierIterator->Pop();
		
		//update node and add it to the closed list
		TileRecord new_tile_info;
		new_tile_info.m_tileType = g_turnState.observedTiles[node_idx];
		new_tile_info.m_hasFood = g_turnState.tilesThatHaveFood[node_idx];
		new_tile_info.m_lastUpdated = g_turnState.turnNumber;
		out_tiles.push_back(new_tile_info);
		m_searchSpace[node_idx].m_inClosedList = true;

		//TODO:check goal (easy for breadth-first-search, need to look at depth-first-search)
		//if(m_searchSpace[node_idx].m_pathCost > depth) break;

		//expand node and look at neighbors
		std::vector<short> neighboring_node_idx = GetNeighbors(node_idx);
		const int num_neighbors = static_cast<int>(neighboring_node_idx.size());
		for(int neighbor_idx = 0; neighbor_idx < num_neighbors; ++neighbor_idx)
		{
			if(neighboring_node_idx[neighbor_idx] == -1) continue;
			const short child_node_idx = neighboring_node_idx[neighbor_idx];

			if(	!m_searchSpace[child_node_idx].m_inOpenList || 
				!m_searchSpace[child_node_idx].m_inClosedList)
			{
				m_searchSpace[child_node_idx].m_parentIdx = node_idx;
				m_searchSpace[child_node_idx].m_actionTook = static_cast<eOrderCode>(neighbor_idx + 1);;
				m_searchSpace[child_node_idx].m_pathCost = m_searchSpace[node_idx].m_pathCost + 1;
				m_searchSpace[child_node_idx].m_inOpenList = true;
			}
		}
	//}

	DebugPrintCostMap();
	
	return true;
}

bool SearchGraph::Path( std::vector<eOrderCode>& out_orders, const short start_tile_index, 
						const short end_tile_index )
{

	return false;
}

int SearchGraph::TotalNodesToSearch(int depth)
{
	return depth*depth + (depth + 1)*(depth+1);
}

//-----------------------------------------------------------------
// Helper functions
std::vector<short> SearchGraph::GetNeighbors(const short idx_coord)
{
	const IntVec2 coord = Geographer::GetTileCoord(idx_coord);

	IntVec2 dir[] = {
		IntVec2(1, 0),	//East
		IntVec2(0, 1),	//North
		IntVec2(-1, 0),	//West
		IntVec2(0, -1)}; //South

	std::vector<short> result = std::vector<short>();

	for (auto& idx : dir)
	{
		IntVec2 test_coord(coord.x + idx.x, coord.y + idx.y);
		if(!Geographer::IsValidCoord(test_coord))
		{
			result.emplace_back(-1);
			continue;
		};

		short valid_idx = Geographer::GetTileIndex(test_coord);
		result.emplace_back(valid_idx);
	}

	return result;
}

void SearchGraph::DebugPrintCostMap()
{
	for(short node_idx = 0; node_idx > Geographer::s_mapTotalSize; ++node_idx)
	{
		IntVec2 coord = Geographer::GetTileCoord(node_idx);
		const float x_coord = static_cast<float>(coord.x);
		const float y_coord = static_cast<float>(coord.y);

		int cost = static_cast<int>(m_searchSpace[node_idx].m_pathCost);
		std::string cost_string;
		if(cost == FLT_MAX) cost_string = ".";
		else cost_string = std::to_string(cost);

		g_debugInterface->QueueDrawWorldText(x_coord, y_coord, 0.5f, 0.5f, 0.8f, Color8(255, 255, 255), cost_string.c_str());

	}
	g_debugInterface->FlushQueuedDraws();
}