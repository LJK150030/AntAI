#pragma once
#include "Arena/ArenaPlayerInterface.hpp"
#include <mutex>
#include <atomic>
#include <map>

class AntUnit;
class AntPool;

class MainThread
{

	
public:
	//game variables
	PlayerTurnOrders					m_turnOrders;
	int									m_lastTurnProcessed;

	//threading variables
	bool								m_running;
	std::mutex							m_turnLock;
	std::condition_variable				m_turnCV;
	std::atomic<int>					m_numActiveThreads;
	
	std::map<AgentID, AntUnit*>			m_hive;
	AntPool*							m_antPool;

	

public:	// STATIC public functions for Singleton
	static MainThread*	GetInstance();
	static void			DeleteInstance();

public:
	~MainThread();
	void ProcessTurn( ArenaTurnStateForPlayer& turn_state );

	//Part of PlayerImplem interface
	void Startup( const StartupInfo& info );
	void Shutdown( const MatchResults& results ); 
	void ThreadEntry( int threadIdx ); 
	void ReceiveTurnState( const ArenaTurnStateForPlayer& state );
	bool TurnOrderRequest( PlayerTurnOrders* orders ); 
	void AddOrder(AgentID agent, eOrderCode order);

	bool ContainsAnt(AgentID agent);
	
private:
	static MainThread* s_mainThreadInstance;

	
private:
	MainThread();
	
}; 