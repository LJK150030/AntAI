#include "Character/AntUnit.hpp"
#include "Geographer/Geographer.hpp"
#include "Blackboard.hpp"
#include "Math/MathUtils.hpp"
#include "Architecture/ErrorWarningAssert.hpp"

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


void AntUnit::Decide(AgentReport& report)
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
			m_goalCoord = g_queenPos;
			float priority = 1.0f - (m_currentOrderIndex * MAX_PATH_INVERSE);
			g_pathingRequests.Push(RepathPriority(m_report.agentID, priority));
			
			//std::vector<eOrderCode> pathing = Geographer::PathfindAstar(m_currentCoord, g_queenPos);
			//MainThread::GetInstance()->AddOrder(m_report.agentID, pathing.front());
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
				MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_EMOTE_CONFUSED );
			}
			else
			{		
				m_goalCoord = coord_to_go_to;
				float priority = 1.0f - (m_currentOrderIndex * MAX_PATH_INVERSE);
				g_pathingRequests.Push(RepathPriority(m_report.agentID, priority));

				//std::vector<eOrderCode> pathing = Geographer::PathfindAstar(m_currentCoord, coord_to_go_to);
				//MainThread::GetInstance()->AddOrder(m_report.agentID, pathing.front());
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
					m_goalCoord = IntVec2::NEG_ONE;
					//MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_EMOTE_ANGRY );
				}
			}
			// not at our destination
			else
			{
				// m_goalCoord = coord_to_go_to;
				float priority = 1.0f - (m_currentOrderIndex * MAX_PATH_INVERSE);
				g_pathingRequests.Push(RepathPriority(m_report.agentID, priority));

				//std::vector<eOrderCode> pathing = Geographer::PathfindAstar(m_currentCoord, m_goalCoord);
				//MainThread::GetInstance()->AddOrder(m_report.agentID, pathing.front());
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

	if(Geographer::HowManyEnemiesCanISee() > 0)
	{
		IntVec2 enemy_coord = Geographer::GetNextEnemyCoord();

		if(enemy_coord != IntVec2::NEG_ONE)
		{
			float priority = 0.1f;
			m_goalCoord = enemy_coord;
			g_pathingRequests.Push(RepathPriority(m_report.agentID, priority));
		}

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


	if(g_currentNumSoldier < g_turnState.numObservedAgents && g_currentNumSoldier < MAX_NUM_SOLDIERS)
	{
		MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_BIRTH_SOLDIER );
	}
	else if(g_currentNumWorkers < MIN_NUM_WORKERS && g_currentNumWorkers < Geographer::HowMuchFoodCanISee())
	{
		MainThread::GetInstance()->AddOrder(m_report.agentID, ORDER_BIRTH_WORKER );
	}

}


//------------------------------------------------------------------------------------


void AntUnit::MoveRandom()
{
	const int offset = rand() % 4;
	const eOrderCode order = static_cast<eOrderCode>(ORDER_MOVE_EAST + offset);

	MainThread::GetInstance()->AddOrder(m_report.agentID, order);
}


void AntUnit::MoveGreedy(const IntVec2& start, const IntVec2& goal)
{
	const eOrderCode order = Geographer::GreedyMovement(start, goal);

	MainThread::GetInstance()->AddOrder(m_report.agentID, order);
}

void AntUnit::UpdatePath()
{
	m_currentOrderIndex = 0;
	memcpy(&m_pathOrders, &DEFAULT_PATHING, sizeof(eOrderCode)*MAX_PATH );
	
	std::vector<eOrderCode> pathing;
	if(m_report.type == AGENT_TYPE_WORKER)
		pathing = Geographer::PathfindAstar(m_currentCoord, m_goalCoord, true);
	else
		pathing = Geographer::PathfindAstar(m_currentCoord, m_goalCoord, false);

	int max_num = Min(pathing.size(), MAX_PATH);
	
	for(int idx = 0; idx < max_num; ++idx)
	{
		m_pathOrders[idx] = pathing[idx];
	}	
}

void AntUnit::ContinuePath()
{
	ASSERT_OR_DIE(m_currentOrderIndex < MAX_PATH, "Reading outside of max path")
	
	MainThread::GetInstance()->AddOrder(m_report.agentID, m_pathOrders[m_currentOrderIndex]);
	++m_currentOrderIndex;
}

bool AntUnit::InUse() const
{
	return !m_isGarbage;
}


//------------------------------------------------------------------------------------
