#include "Character/AntUnit.hpp"
#include "Geographer/Geographer.hpp"
#include "Blackboard.hpp"

//------------------------------------------------------------------------------------

AntUnit::AntUnit() {}
AntUnit::~AntUnit() {}

void AntUnit::Init(AgentReport& report)
{
	m_report = report;
	m_currentCoord = IntVec2(report.tileX, report.tileY);
	m_isGarbage = false;
}

//------------------------------------------------------------------------------------


void AntUnit::ProcessTurn(AgentReport& report)
{
	m_report = report;
	m_currentCoord = IntVec2(m_report.tileX, m_report.tileY);

	if(m_report.state == STATE_DEAD)
	{
		switch (m_report.type)
		{
			case AGENT_TYPE_SCOUT:
			{
				--g_currentNumScouts;
				break;
			}

			case AGENT_TYPE_WORKER:
			{
				--g_currentNumWorkers;
				Geographer::RemoveAntFromFoodTile(m_goalCoord);
				break;
			}

			case AGENT_TYPE_SOLDIER:
			{
				--g_currentNumSoldier;
				break;
			}

			case AGENT_TYPE_QUEEN:
			{
				--g_currentNumQueen;
				break;
			}
			
			default: { break; }
		}

		m_isGarbage = true;
	}

	// agent is alive and ready to get an order, so do something
	else if (m_report.exhaustion == 0)
	{
		TODO("replace this with strategy pattern")
		switch (m_report.type)
		{
			// scout will randomly walk
			case AGENT_TYPE_SCOUT:
			{
				UpdateScout();
				break;
			}

			// moves randomly, but if they fall on food, will pick it up if hands are free
			case AGENT_TYPE_WORKER:
			{
				UpdateWorker();
				break;
			}

			// Soldier randomly walks
			case AGENT_TYPE_SOLDIER:
			{
				UpdateSoldier();
				break;
			}


			// queen either moves or spawns randomly
			case AGENT_TYPE_QUEEN:
			{
				UpdateQueen();
				break;
			}
			
			default: { break; }
		}
	}
}


void AntUnit::UpdateScout()
{
	if(m_report.result == AGENT_WAS_CREATED)
	{
		++g_currentNumScouts;
	}

	MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_SUICIDE );
}

void AntUnit::UpdateWorker()
{
	if(m_report.result == AGENT_WAS_CREATED)
	{
		++g_currentNumWorkers;
	}

	if (m_report.state == STATE_HOLDING_FOOD)
	{
		
		if(m_currentCoord == g_queenPos)
		{
			MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_DROP_CARRIED_OBJECT );
		}
		else
		{
			//we need to hall ass to the queen
			std::vector<eOrderCode> pathing = Geographer::PathfindAstar(m_currentCoord, g_queenPos);
			MainThread::GetInstance()->AddOrder(m_report.agentID, pathing.front());
		}
	}
	else 
	{
		if(m_goalCoord == IntVec2::NEG_ONE)
		{
			IntVec2 coord_to_go_to = Geographer::AddAntToFoodTile(m_report.agentID);

			//if there is no work
			if(coord_to_go_to == IntVec2(-1, -1))
			{
				MoveRandom(m_report.agentID);
				MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_EMOTE_CONFUSED );
			}
			else
			{		
				m_goalCoord = coord_to_go_to;
				std::vector<eOrderCode> pathing = Geographer::PathfindAstar(m_currentCoord, coord_to_go_to);
				MainThread::GetInstance()->AddOrder(m_report.agentID, pathing.front());
			}
		}
		else // ant has work
		{
			// are we at our destination?
			if (m_currentCoord == m_goalCoord)
			{
				Geographer::RemoveAntFromFoodTile(m_currentCoord);

				// is there food here
				if(Geographer::DoesCoordHaveFood(m_currentCoord))
				{
					MainThread::GetInstance()->AddOrder( m_report.agentID, ORDER_PICK_UP_FOOD );
				}
				// else the food has already been picked up
				{
// 					IntVec2 coord_to_go_to = Geographer::AddAntToFoodTile(report.agentID);
// 					m_workerJobs[report.agentID] = Geographer::GetTileIndex(coord_to_go_to);
// 					std::vector<eOrderCode> pathing = Geographer::PathfindAstar(ant_coord, coord_to_go_to);
// 					AddOrder(report.agentID, pathing.front());
					m_goalCoord = IntVec2::NEG_ONE;
					MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_EMOTE_ANGRY );

				}
			}
			// not at our destination
			else
			{
				std::vector<eOrderCode> pathing = Geographer::PathfindAstar(m_currentCoord, m_goalCoord);
				MainThread::GetInstance()->AddOrder(m_report.agentID, pathing.front());
			}
		}

	}
}

void AntUnit::UpdateSoldier()
{
	if(m_report.result == AGENT_WAS_CREATED)
	{
		++g_currentNumSoldier;
	}

	if(g_turnState.turnNumber < SPAWN_SOLDIERS_AFTER)
	{
		MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_SUICIDE );
	}
}

void AntUnit::UpdateQueen()
{
	if(m_report.result == AGENT_WAS_CREATED)
	{
		++g_currentNumQueen;
	}

	g_queenPos = IntVec2(m_report.tileX, m_report.tileY);
	Geographer::UpdateListOfFood(g_queenPos);


	if(g_currentNumSoldier < MIN_NUM_SOLDIERS)
	{
		MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_BIRTH_SOLDIER );
	}
	else if(g_currentNumWorkers < MIN_NUM_WORKERS && g_currentNumWorkers < Geographer::HowMuchFoodCanISee())
	{
		MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_BIRTH_WORKER );
	}

}


//------------------------------------------------------------------------------------


void AntUnit::MoveRandom(AgentID agent)
{
	const int offset = rand() % 4;
	const eOrderCode order = static_cast<eOrderCode>(ORDER_MOVE_EAST + offset);

	MainThread::GetInstance()->AddOrder(agent, order);
}


void AntUnit::MoveGreedy(AgentID agent, const IntVec2& start, const IntVec2& goal)
{
	const eOrderCode order = Geographer::GreedyMovement(start, goal);

	MainThread::GetInstance()->AddOrder(agent, order);
}

bool AntUnit::InUse() const
{
	return m_isGarbage;
}


//------------------------------------------------------------------------------------
