#pragma once
#include "Arena/ArenaPlayerInterface.hpp"
#include "Geographer/Geographer.hpp"
#include "Architecture/Queue.hpp"

class Queue;
class QueueIterator;

class SearchGraph
{
	struct NodeRecord
	{
		short		m_parentIdx = -1;
		eOrderCode	m_actionTook = ORDER_HOLD;
		float		m_pathCost = FLT_MAX;
		bool		m_inOpenList = false;
		bool		m_inClosedList = false;
	};
	
public:
	// Graph (with a version number)
	TileRecord	m_map[MAX_ARENA_TILES];
	NodeRecord	m_searchSpace[MAX_ARENA_TILES];
	int			m_versionNumber = 0;

	// list of indexs to potentially search
	Queue			m_frontier;
	QueueIterator*	m_frontierIterator = nullptr;
	
public:
	SearchGraph(eQueueType queue_type, const TileRecord map[MAX_ARENA_TILES]);
	~SearchGraph();

	//function GRAPH-SEARCH(problem) returns a solution, or failure
	bool FloodFill( std::vector<TileRecord>& out_tiles, short start_tile_index, int depth );
	bool Path( std::vector<eOrderCode>& out_orders, short start_tile_index, short end_tile_index );

	// Debug drawing
	void DebugPrintCostMap();
	void DebugPrintDirectionMap();
	void DebugPrintPath();
	
private:
	std::vector<short>	GetNeighbors( short idx_coord);
	int					TotalNodesToSearch(int depth);
};
