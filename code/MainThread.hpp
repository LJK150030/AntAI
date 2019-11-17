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
	void MoveGreedy( AgentID agent, const IntVec2& start, const IntVec2& goal );
	void AddOrder( AgentID agent, eOrderCode order );

	void UpdateScout(AgentReport& report);
	void UpdateWorker(AgentReport& report);
	void UpdateSoldier(AgentReport& report);
	void UpdateQueen(AgentReport& report);

public:
	int							m_lastTurnProcessed; 
	bool						m_running; 

	IntVec2						m_queenPos;
	std::map<AgentID, short>	m_workerJobs;
	int							m_currentNumScouts = 0;
	int							m_currentNumWorkers = 0;
	int							m_currentNumSoldier = 0;
	int							m_currentNumQueen = 0;

	std::mutex					m_turnLock; 
	std::condition_variable		m_turnCV;
	bool						m_workerDead = false;

	PlayerTurnOrders			m_turnOrders;
	std::atomic<int>			m_numActiveThreads;
}; 