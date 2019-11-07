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
	Geographer::Shutdown();
}


void MainThread::ThreadEntry( int /*threadIdx*/ )
{
	// wait for data
	// process turn
	// mark turn as finished;
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
			m_agentLastKnownLocation[report.agentID] = std::pair<short, short> (report.tileX, report.tileY);
		}
		else
		{
			if(report.type == AGENT_TYPE_WORKER)
			{
				--m_currentNumWorkers;
			}
			
			m_agentLastKnownLocation.erase(report.agentID);
			continue;
		}
		
		if (report.exhaustion == 0) 
		{
			// agent is alive and ready to get an order, so do something
			switch (report.type) 
			{
				// scout will randomly walk
				case AGENT_TYPE_SCOUT:
				{
//					MoveRandom( report.agentID );
					break; 
				}

				// moves randomly, but if they fall on food, will pick it up if hands are free
				case AGENT_TYPE_WORKER:
				{
					if(report.result == AGENT_WAS_CREATED)
					{
						++m_currentNumWorkers;
					}

					if (report.state == STATE_HOLDING_FOOD) 
					{
						short queen_x = m_agentLastKnownLocation[m_queenID].first;
						short queen_y = m_agentLastKnownLocation[m_queenID].second;
						
						if(report.tileX == queen_x && report.tileY == queen_y)
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
						if (Geographer::DoesCoordHaveFood(report.tileX, report.tileY)) 
						{
							AddOrder( report.agentID, ORDER_PICK_UP_FOOD ); 
						} 
						else 
						{
							MoveRandom( report.agentID );
						}
					}
					break;
				}
				

				// Soldier randomly walks
				case AGENT_TYPE_SOLDIER:
				{
//					MoveRandom( report.agentID );
					break; 
				}

				
				// queen either moves or spawns randomly
				case AGENT_TYPE_QUEEN: 
				{
					m_queenID = report.agentID;

					if(m_currentNumWorkers < MIN_NUM_WORKERS)
					{
						AddOrder( report.agentID, ORDER_BIRTH_WORKER );
					}
// 					const float random_spawn_chance = 0.9f; 
// 					if (g_randomNumberGenerator.GetRandomFloatZeroToOne() < random_spawn_chance)
// 					{
// 						// spawn
// 						const int type_offset = rand() % 3;
// 						const eOrderCode order = static_cast<eOrderCode>(ORDER_BIRTH_SCOUT + type_offset); 
// 						AddOrder( report.agentID, order ); 
// 					} 
// 					else 
// 					{
// 						MoveRandom( report.agentID ); 
// 					}

					break; 	
				}
				default: { break; }
			}
		}
	}
}


void MainThread::MoveRandom( AgentID agent )
{
	const int offset = rand() % 4;
	const eOrderCode order = static_cast<eOrderCode>(ORDER_MOVE_EAST + offset); 

	AddOrder( agent, order ); 
}

void MainThread::MoveGreedy( AgentID agent )
{
	short agent_x = m_agentLastKnownLocation[agent].first;
	short agent_y = m_agentLastKnownLocation[agent].second;
	short queen_x = m_agentLastKnownLocation[m_queenID].first;
	short queen_y = m_agentLastKnownLocation[m_queenID].second;
	
	const eOrderCode order = Geographer::GreedyMovement(agent_x, 
		agent_y, queen_x, queen_y);


	AddOrder( agent, order ); 
}


void MainThread::AddOrder( AgentID agent, eOrderCode order )
{
	// TODO: Be sure that I don't double issue an order to an agent
	// Only first order will be processed by the server and a fault will be 
	// issued for any bad order
	// Ants not given ordres are assumed to idle

	// TODO: Make sure I'm not adding too many orders
	const int agent_idx = m_turnOrders.numberOfOrders;

	m_turnOrders.orders[agent_idx].agentID = agent; 
	m_turnOrders.orders[agent_idx].order = order; 

	m_turnOrders.numberOfOrders++; 
}

