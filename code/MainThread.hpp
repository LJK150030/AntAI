#pragma once
#include "Arena/ArenaPlayerInterface.hpp"
#include "Math/IntVec2.hpp"
#include <mutex>
#include <map>
#include <atomic>

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

	void UpdateScout(AgentReport& report);
	void UpdateWorker(AgentReport& report);
	void UpdateSoldier(AgentReport& report);
	void UpdateQueen(AgentReport& report);

public:
	int							m_lastTurnProcessed; 
	bool						m_running; 

	std::map<AgentID, IntVec2>	m_agentLastKnownLocation;
	AgentID						m_queenID;
	int							m_currentNumScouts = 0;
	int							m_currentNumWorkers = 0;
	int							m_currentNumSoldier = 0;
	int							m_currentNumQueen = 0;

	std::mutex					m_turnLock; 
	std::condition_variable		m_turnCV; 

	PlayerTurnOrders			m_turnOrders;
	std::atomic<int>			m_numActiveThreads;
}; 