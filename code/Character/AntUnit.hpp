#pragma once
#include "Arena/ArenaPlayerInterface.hpp"
#include "Math/IntVec2.hpp"
#include "Blackboard.hpp"

struct IntVec2;
struct ArenaTurnStateForPlayer;

class AntUnit
{
public:
	AntUnit();
	~AntUnit();

	void ProcessTurn(AgentReport& report);
	

	void UpdateScout();
	void UpdateWorker();
	void UpdateSoldier();
	void UpdateQueen();

	
	// Helpers
	void MoveRandom( AgentID agent );
	void MoveGreedy( AgentID agent, const IntVec2& start, const IntVec2& goal );

	// Obj Pool
	void Init(AgentReport& report);
	bool InUse() const;

private:
	AgentReport		m_report = AgentReport();
	IntVec2			m_currentCoord = IntVec2::NEG_ONE;
	
	//helper functions
	IntVec2			m_goalCoord = IntVec2::NEG_ONE;
	eOrderCode		m_pathOrders[MAX_PATH] = { ORDER_HOLD };

	// used for obj pooling
	bool			m_isGarbage = true;
};
