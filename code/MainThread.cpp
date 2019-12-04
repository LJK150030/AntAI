#include "MainThread.hpp"
#include "Blackboard.hpp"
#include "Geographer/Geographer.hpp"
#include "Character/AntUnit.hpp"
#include "Architecture/AntPool.hpp"

STATIC MainThread* MainThread::s_mainThreadInstance = nullptr;

STATIC MainThread* MainThread::GetInstance()
{
	if (s_mainThreadInstance == nullptr)
	{
		s_mainThreadInstance = new MainThread();
	}
	
	return s_mainThreadInstance;
}

STATIC void MainThread::DeleteInstance()
{
	if(s_mainThreadInstance != nullptr)
	{
		delete s_mainThreadInstance;
		s_mainThreadInstance = nullptr;
	}
}


MainThread::MainThread() = default;
MainThread::~MainThread() = default;


void MainThread::Startup( const StartupInfo& info )
{
	g_matchInfo = info.matchInfo;
	g_debugInterface = info.debugInterface;
	
	// Optional Todo: Can register into the dev-console system
	// of the server using info.RegisterEvent
	
	// setup the turn number
	g_turnState.turnNumber = -1; 
	m_lastTurnProcessed = -1; 
	m_numActiveThreads = 0;
	m_running = true;

	m_hive = std::map<AgentID, AntUnit*>();
	m_antPool = new AntPool();
	
	Geographer::Startup();
}


void MainThread::Shutdown( const MatchResults& results )
{	
	m_running = false;
	m_turnCV.notify_all();
	
	Geographer::Shutdown();

	delete m_antPool;
	m_antPool = nullptr;
}


void MainThread::ThreadEntry( int /*threadIdx*/ )
{
	// wait for data
	// process turn
	// mark turn as finished;
	++m_numActiveThreads;
	ArenaTurnStateForPlayer turn_state;
	
	while (m_running) 
	{
		std::unique_lock lk( m_turnLock ); 
		m_turnCV.wait( lk, [&]() { return !m_running || m_lastTurnProcessed != g_turnState.turnNumber; } ); 

		if (m_running) 
		{
			turn_state = g_turnState; 
			lk.unlock();

			Geographer::Update();
			
			// process a turn and then mark that the turn is ready; 
			ProcessTurn( turn_state ); 

			// notify the turn is ready; 
			m_lastTurnProcessed = turn_state.turnNumber;  
			g_debugInterface->LogText( "AIPlayer Turn Complete: %i", turn_state.turnNumber ); 
		}
	}
	--m_numActiveThreads;

}


// This has to finish in less than 1MS otherwise you will be faulted
void MainThread::ReceiveTurnState(const ArenaTurnStateForPlayer& state)
{
	// lock and copy information to thread
	{
		std::unique_lock lk( m_turnLock ); 
		g_turnState = state; 
	}

	// unlock, and notify
	m_turnCV.notify_one(); 
}


bool MainThread::TurnOrderRequest( PlayerTurnOrders* orders )
{
	std::unique_lock lk( m_turnLock ); 
	if (m_lastTurnProcessed == g_turnState.turnNumber) 
	{
		*orders = m_turnOrders; 
		return true; 
	} 
	else 
	{
		return false;
	}
}


void MainThread::ProcessTurn( ArenaTurnStateForPlayer& turn_state )
{
	// reset the orders
	m_turnOrders.numberOfOrders = 0;
	g_numRepaths = 0;

	const int agent_count = turn_state.numReports;

	// Decide
	for (int i = 0; i < agent_count; ++i)
	{
		AgentReport& report = turn_state.agentReports[i];

		if (ContainsAnt(report.agentID))
		{
			m_hive[report.agentID]->Decide(report);

			if (report.state == STATE_DEAD)
			{
				m_hive.erase(report.agentID);
			}
		}
		else
		{
			AntUnit* new_unit = m_antPool->AddAnt(report);
			m_hive.emplace(report.agentID, new_unit);
			m_hive[report.agentID]->Decide(report);
		}
		
	}

	// Act
	while(g_pathingRequests.GetSize() != 0)
	{
		RepathPriority current_pathing_job = g_pathingRequests.Pop();
		if (g_numRepaths <= MAX_REPATHING)
		{
			m_hive[current_pathing_job.m_id]->UpdatePath();
			++g_numRepaths;
		}
		m_hive[current_pathing_job.m_id]->ContinuePath();
	}
}


void MainThread::AddOrder(AgentID agent, eOrderCode order)
{
	TODO("Be sure that I don't double issue an order to an agent")
		// Only first order will be processed by the server and a fault will be 
		// issued for any bad order
		// Ants not given ordres are assumed to idle

	TODO("Make sure I'm not adding too many orders")
	const int agent_idx = m_turnOrders.numberOfOrders;

	m_turnOrders.orders[agent_idx].agentID = agent;
	m_turnOrders.orders[agent_idx].order = order;

	m_turnOrders.numberOfOrders++;
}


bool MainThread::ContainsAnt(AgentID agent)
{
	std::map<AgentID, AntUnit*>::iterator hive_iter;

	// Find the element with key 'hat'
	hive_iter = m_hive.find(agent);

	// Check if element exists in map or not
	if (hive_iter != m_hive.end())
	{
		return true;
	}

	return false;
}
