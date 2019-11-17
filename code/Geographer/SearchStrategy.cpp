#include "Geographer/SearchStrategy.hpp"

SearchStrategy::SearchStrategy()
{
	TODO("Need to make SearchStrategy thread safe")
	memcpy ( &m_map, &Geographer::s_perceivedMap, MAX_ARENA_TILES * sizeof(TileRecord));

	m_versionNumber = g_turnState.turnNumber;
}

SearchStrategy::~SearchStrategy() {}
