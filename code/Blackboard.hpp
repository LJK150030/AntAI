#pragma once
#include "Arena/ArenaPlayerInterface.hpp"
#include "MainThread.hpp"
#include "Math/RandomNumberGenerator.hpp"
#include <cstring>

// Macro functions
#define STATIC

// Global type def
typedef unsigned int uint;

// Global variables that everyone can share
extern RandomNumberGenerator	g_randomNumberGenerator;
extern MainThread*				g_thePlayer;

extern MatchInfo				g_matchInfo;
extern DebugInterface*			g_debugInterface;
extern ArenaTurnStateForPlayer	g_turnState;

extern int g_workerBirthFrequency;