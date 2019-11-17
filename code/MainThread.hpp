#pragma once
#include "Arena/ArenaPlayerInterface.hpp"
#include "Math/IntVec2.hpp"
#include <mutex>
#include <map>
#include <atomic>

//-----------------------------------------------------------------------------------------------
class MainThread; 

//very hacky but for now, make a struct of ants
struct Ant
{
public:
	AgentReport report;
	float temperature = 1.0f;

public:
	Ant(); 
	Ant( AgentReport& agent );
	~Ant();

	void Update(AgentReport& updated_report);
	void UpdateMap();
};


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
	void MoveGreedy( AgentID agent, const IntVec2& coord );
	void AddOrder( AgentID agent, eOrderCode order );

	void UpdateScout(AgentReport& report);
	void UpdateWorker(AgentReport& report);
	void UpdateSoldier(AgentReport& report);
	void UpdateQueen(AgentReport& report);

public:
	int							m_lastTurnProcessed; 
	bool						m_running; 

	std::map<AgentID, Ant>		m_colony;
	AgentID						m_queenID;
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