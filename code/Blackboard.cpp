#include "Blackboard.hpp"
#include "MainThread.hpp"
#include "Math/RandomNumberGenerator.hpp"


MainThread* g_thePlayer = nullptr; 
RandomNumberGenerator g_randomNumberGenerator(15);

MatchInfo					g_matchInfo;
DebugInterface*				g_debugInterface = nullptr;
ArenaTurnStateForPlayer		g_turnState;

