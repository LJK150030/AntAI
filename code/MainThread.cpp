#include "MainThread.hpp"
#include "Blackboard.hpp"
#include "Geographer.hpp"


void MainThread::Startup( const StartupInfo& info )
{
	g_matchInfo = info.matchInfo;
	g_debugInterface = info.debugInterface;
	
	// Optional Todo: Can register into the dev-console system
	// of the server using info.RegisterEvent
	
	// setup the turn number
	g_turnState.turnNumber = -1; 
	m_lastTurnProcessed = -1; 
	m_running = true;

	
	Geographer::Startup();
}


void MainThread::Shutdown( const MatchResults& results )
{
	m_running = false;
	m_turnCV.notify_all();
	
	Geographer::Shutdown();
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

	// for each ant I know about, give him something to do
	const int agent_count = turn_state.numReports; 

	// you will be given one report per agent you own (or did own the previous frame
	// in case of death).  For any live agent, you may make a command
	// this turn
	for (int i = 0; i < agent_count; ++i) 
	{
		AgentReport& report = turn_state.agentReports[i];

		if(report.state != STATE_DEAD)
		{
			m_agentLastKnownLocation[report.agentID] = IntVec2(report.tileX, report.tileY);
		}
		else
		{
			switch(report.type)
			{
				case AGENT_TYPE_SCOUT:
				{
					--m_currentNumScouts;
					break;
				}
				case AGENT_TYPE_WORKER:
				{
					--m_currentNumWorkers;
					break;
				}
				case AGENT_TYPE_SOLDIER:
				{
					--m_currentNumSoldier;
					break;
				}
				case AGENT_TYPE_QUEEN:
				{
					--m_currentNumQueen;
					break;
				}
			}
			
			m_agentLastKnownLocation.erase(report.agentID);
			continue;
		}
		
		if (report.exhaustion == 0) 
		{
			// agent is alive and ready to get an order, so do something
			TODO("replace this with strategy pattern")
			switch (report.type) 
			{
				// scout will randomly walk
				case AGENT_TYPE_SCOUT:
				{
					//UpdateScout(report);
					break;
				}

				// moves randomly, but if they fall on food, will pick it up if hands are free
				case AGENT_TYPE_WORKER:
				{
					//UpdateWorker(report);
					break;
				}
				
				// Soldier randomly walks
				case AGENT_TYPE_SOLDIER:
				{
					//UpdateSoldier(report);
					break; 
				}

				
				// queen either moves or spawns randomly
				case AGENT_TYPE_QUEEN: 
				{
					UpdateQueen(report);
					break; 	
				}
				default: { break; }
			}
		}
	}
}


void MainThread::UpdateScout(AgentReport& report)
{
	if(report.result == AGENT_WAS_CREATED)
	{
		++m_currentNumScouts;
	}
}


void MainThread::UpdateWorker(AgentReport& report)
{
	if(report.result == AGENT_WAS_CREATED)
	{
		++m_currentNumWorkers;
	}

	if (report.state == STATE_HOLDING_FOOD) 
	{
		const IntVec2 queen_coord(m_agentLastKnownLocation[m_queenID].x,
			m_agentLastKnownLocation[m_queenID].y);

		if(report.tileX == queen_coord.x && report.tileY == queen_coord.y)
		{
			AddOrder( report.agentID, ORDER_DROP_CARRIED_OBJECT ); 
		}
		else
		{
			if(report.result == AGENT_ORDER_ERROR_MOVE_BLOCKED_BY_TILE)
			{
				MoveRandom( report.agentID );
			}
			else
			{
				MoveGreedy( report.agentID );
			}
		}
	}
	else 
	{
		if (Geographer::DoesCoordHaveFood(IntVec2(report.tileX, report.tileY))) 
		{
			AddOrder( report.agentID, ORDER_PICK_UP_FOOD ); 
		} 
		else 
		{
			MoveRandom( report.agentID );
		}
	}
}


void MainThread::UpdateSoldier(AgentReport& report)
{
	if(report.result == AGENT_WAS_CREATED)
	{
		++m_currentNumSoldier;
	}

	//					MoveRandom( report.agentID );
}


void MainThread::UpdateQueen(AgentReport& report)
{
	if(report.result == AGENT_WAS_CREATED)
	{
		++m_currentNumQueen;
	}

	m_queenID = report.agentID;
	IntVec2 coord(report.tileX, report.tileY);
	std::vector<eOrderCode> pathing = Geographer::PathfindDijkstra(coord, IntVec2::ONE);
	AddOrder(report.agentID, pathing.front());
// 	if(m_currentNumWorkers < MIN_NUM_WORKERS)
// 	{
// 		AddOrder( report.agentID, ORDER_BIRTH_WORKER );
// 	}
// 	else if(m_currentNumSoldier < MIN_NUM_SOLDIERS)
// 	{
// 		AddOrder( report.agentID, ORDER_BIRTH_SOLDIER );
// 	}

	
}


void MainThread::MoveRandom( AgentID agent )
{
	const int offset = rand() % 4;
	const eOrderCode order = static_cast<eOrderCode>(ORDER_MOVE_EAST + offset); 

	AddOrder( agent, order ); 
}

void MainThread::MoveGreedy( AgentID agent )
{
	IntVec2 agent_coord(m_agentLastKnownLocation[agent].x, m_agentLastKnownLocation[agent].y);
	IntVec2 queen_coord(m_agentLastKnownLocation[m_queenID].x, m_agentLastKnownLocation[m_queenID].y);

	const eOrderCode order = Geographer::GreedyMovement(agent_coord,  queen_coord );

	AddOrder( agent, order ); 
}


void MainThread::AddOrder( AgentID agent, eOrderCode order )
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
