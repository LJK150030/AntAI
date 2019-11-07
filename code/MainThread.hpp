#pragma once
#include "Arena/ArenaPlayerInterface.hpp"
#include <mutex>
#include <map>

//-----------------------------------------------------------------------------------------------
class MainThread; 

//-----------------------------------------------------------------------------------------------
class MainThread
{
public:
	void Startup( const StartupInfo& info );
	void Shutdown( const MatchResults& results ); 
	void ThreadEntry( int threadIdx ); 
	void ReceiveTurnState( const ArenaTurnStateForPlayer& state );
	bool TurnOrderRequest( PlayerTurnOrders* orders ); 

public:
	void ProcessTurn( ArenaTurnStateForPlayer& turn_state );
	
	// Helpers
	void MoveRandom( AgentID agent );
	void MoveGreedy( AgentID agent );
	void AddOrder( AgentID agent, eOrderCode order ); 

public:
	int m_lastTurnProcessed; 
	bool m_running; 

	std::map<AgentID, std::pair<short, short>> m_agentLastKnownLocation;
	AgentID m_queenID;
	int m_currentNumWorkers = 0;

	
	std::mutex m_turnLock; 
	std::condition_variable m_turnCV; 

	PlayerTurnOrders			m_turnOrders; 
}; 