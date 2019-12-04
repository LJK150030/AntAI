#include "Blackboard.hpp"
#include "MainThread.hpp"
#include "Math/RandomNumberGenerator.hpp"


RandomNumberGenerator g_randomNumberGenerator(15);

MatchInfo					g_matchInfo;
DebugInterface*				g_debugInterface = nullptr;
ArenaTurnStateForPlayer		g_turnState;

int			g_currentNumScouts = 0;
int			g_currentNumWorkers = 0;
int			g_currentNumSoldier = 0;
int			g_currentNumQueen = 0;

IntVec2 g_queenPos = IntVec2::NEG_ONE;