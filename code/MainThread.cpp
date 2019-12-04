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

	const int agent_count = turn_state.numReports;

	// you will be given one report per agent you own (or did own the previous frame
	// in case of death).  For any live agent, you may make a command
	// this turn
	for (int i = 0; i < agent_count; ++i)
	{
		AgentReport& report = turn_state.agentReports[i];

		if (ContainsAnt(report.agentID))
		{
			m_hive[report.agentID]->ProcessTurn(report);

			if (report.state == STATE_DEAD)
			{
				m_hive.erase(report.agentID);
			}
		}
		else
		{
			AntUnit* new_unit = m_antPool->AddAnt(report);
			m_hive.emplace(report.agentID, new_unit);
			m_hive[report.agentID]->ProcessTurn(report);
		}
		
	}


	// you will be given one report per agent you own (or did own the previous frame
	// in case of death).  For any live agent, you may make a command
	// this turn
// 	for (int i = 0; i < agent_count; ++i) 
// 	{
// 		AgentReport& report = turn_state.agentReports[i];
// 
// 		if(report.state == STATE_DEAD)
// 		{
// 			switch(report.type)
// 			{
// 				case AGENT_TYPE_SCOUT:
// 				{
// 					--m_currentNumScouts;
// 					break;
// 				}
// 				case AGENT_TYPE_WORKER:
// 				{
// 					--m_currentNumWorkers;
// 					if(report.result == AGENT_ORDER_SUCCESS_SUICIDE)
// 					{
// 						m_workerDead = false;
// 					}
// 					Geographer::RemoveAntFromFoodTile(Geographer::GetTileCoord(m_workerJobs[report.agentID]));
// 					m_workerJobs.erase(report.agentID);
// 
// 					break;
// 				}
// 				case AGENT_TYPE_SOLDIER:
// 				{
// 					--m_currentNumSoldier;
// 					break;
// 				}
// 				case AGENT_TYPE_QUEEN:
// 				{
// 					--m_currentNumQueen;
// 					break;
// 				}
// 			}
// 			
// 			continue;
// 		}
// 		
// 		if (report.exhaustion == 0) 
// 		{
// 			// agent is alive and ready to get an order, so do something
// 			TODO("replace this with strategy pattern")
// 			switch (report.type) 
// 			{
// 				// scout will randomly walk
// 				case AGENT_TYPE_SCOUT:
// 				{
// 					UpdateScout(report);
// 					break;
// 				}
// 
// 				// moves randomly, but if they fall on food, will pick it up if hands are free
// 				case AGENT_TYPE_WORKER:
// 				{
// 					UpdateWorker(report);
// 					break;
// 				}
// 				
// 				// Soldier randomly walks
// 				case AGENT_TYPE_SOLDIER:
// 				{
// 					UpdateSoldier(report);
// 					break; 
// 				}
// 
// 				
// 				// queen either moves or spawns randomly
// 				case AGENT_TYPE_QUEEN: 
// 				{
// 					UpdateQueen(report);
// 					break; 	
// 				}
// 				default: { break; }
// 			}
// 		}
// 	}
}


// void MainThread::UpdateWorker(AgentReport& report)
// {
// 	if(report.result == AGENT_WAS_CREATED)
// 	{
// 		++m_currentNumWorkers;
// 		m_workerJobs.emplace(report.agentID, -1);
// 	}
// 
// 	IntVec2 ant_coord(report.tileX, report.tileY);
// 
// 	
// 	if (report.state == STATE_HOLDING_FOOD) 
// 	{
// 		
// 		if(report.tileX == m_queenPos.x && report.tileY == m_queenPos.y)
// 		{
// 			AddOrder( report.agentID, ORDER_DROP_CARRIED_OBJECT );
// 		}
// 		else
// 		{
// 			//we need to hall ass to the queen
// 			std::vector<eOrderCode> pathing = Geographer::PathfindAstar(ant_coord, m_queenPos);
// 			AddOrder(report.agentID, pathing.front());
// 		}
// 	}
// 	else 
// 	{
// 		
// // 		if(m_currentNumWorkers > MIN_NUM_WORKERS && !m_workerDead)
// // 		{
// // 			m_workerDead = true;
// // 			AddOrder( report.agentID, ORDER_SUICIDE );
// // 		}
// // 		//ant needs work
// // 		else
// 		if(m_workerJobs[report.agentID] == -1) 
// 		{
// 			IntVec2 coord_to_go_to = Geographer::AddAntToFoodTile(report.agentID);
// 
// 			//if there is no work
// 			if(coord_to_go_to == IntVec2(-1, -1))
// 			{
// 				//MoveRandom(report.agentID);
// 				AddOrder( report.agentID, ORDER_EMOTE_CONFUSED );
// 			}
// 			else
// 			{		
// 				m_workerJobs[report.agentID] = Geographer::GetTileIndex(coord_to_go_to);
// 				std::vector<eOrderCode> pathing = Geographer::PathfindAstar(ant_coord, coord_to_go_to);
// 				AddOrder(report.agentID, pathing.front());
// 			}
// 		}
// 		else // ant has work
// 		{
// 			IntVec2 dest = Geographer::GetTileCoord(m_workerJobs[report.agentID]);
// 
// 			// are we at our destination?
// 			if (ant_coord == dest) 
// 			{
// 				Geographer::RemoveAntFromFoodTile(ant_coord);
// 
// 				// is there food here
// 				if(Geographer::DoesCoordHaveFood(IntVec2(report.tileX, report.tileY)))
// 				{
// 					AddOrder( report.agentID, ORDER_PICK_UP_FOOD );
// 				}
// 				// else the food has already been picked up
// 				{
// // 					IntVec2 coord_to_go_to = Geographer::AddAntToFoodTile(report.agentID);
// // 					m_workerJobs[report.agentID] = Geographer::GetTileIndex(coord_to_go_to);
// // 					std::vector<eOrderCode> pathing = Geographer::PathfindAstar(ant_coord, coord_to_go_to);
// // 					AddOrder(report.agentID, pathing.front());
//  					m_workerJobs[report.agentID] = -1;
// 					AddOrder( report.agentID, ORDER_EMOTE_ANGRY );
// 
// 				}
// 			}
// 			// not at our destination
// 			else
// 			{
// 				IntVec2 coord_to_go_to = Geographer::GetTileCoord(m_workerJobs[report.agentID]);
// 				std::vector<eOrderCode> pathing = Geographer::PathfindAstar(ant_coord, coord_to_go_to);
// 				AddOrder(report.agentID, pathing.front());
// 			}
// 		}
// 
// 	}
// }


// void MainThread::UpdateSoldier(AgentReport& report)
// {
// 	if(report.result == AGENT_WAS_CREATED)
// 	{
// 		++m_currentNumSoldier;
// 	}
// 
// // 	if(g_turnState.turnNumber < SPAWN_SOLDIERS_AFTER)
// // 	{
// // 		AddOrder( report.agentID, ORDER_SUICIDE );
// // 	}
// }


// void MainThread::UpdateQueen(AgentReport& report)
// {
// 	if(report.result == AGENT_WAS_CREATED)
// 	{
// 		++m_currentNumQueen;
// 	}
// 
// 	m_queenPos = IntVec2(report.tileX, report.tileY);
// 	Geographer::UpdateListOfFood(m_queenPos);
// 
// 	
// 	if(m_currentNumSoldier < MIN_NUM_SOLDIERS)
// 	{
// 		AddOrder( report.agentID, ORDER_BIRTH_SOLDIER );
// 	}
// 	else if(m_currentNumWorkers < MIN_NUM_WORKERS && m_currentNumWorkers < Geographer::HowMuchFoodCanISee())
// 	{
// 		AddOrder( report.agentID, ORDER_BIRTH_WORKER );
// 	}
// 	
// }

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
