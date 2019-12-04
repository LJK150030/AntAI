#pragma once
#include "Arena/ArenaPlayerInterface.hpp"
#include "MainThread.hpp"
#include "Math/RandomNumberGenerator.hpp"
#include <cstring>
#include "Math/IntVec2.hpp"

// Macro functions
#define STATIC

#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// the important bits
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
       " --------------------------------------------------------------------------------------\n" \
       "|  TODO :   " ##x "\n" \
       " --------------------------------------------------------------------------------------\n" )

#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" );

// Global type def
typedef unsigned int uint;

// Global Structs
struct RepathPriority
{
	AgentID m_id = UINT_MAX;
	float m_priority = FLT_MAX;
};

// Global variables that everyone can share
extern MatchInfo				g_matchInfo;
extern DebugInterface*			g_debugInterface;
extern ArenaTurnStateForPlayer	g_turnState;
extern RandomNumberGenerator	g_randomNumberGenerator;
extern MainThread*				g_thePlayer;


extern int g_currentNumScouts;
extern int g_currentNumWorkers;
extern int g_currentNumSoldier;
extern int g_currentNumQueen;
extern IntVec2 g_queenPos;


constexpr int MIN_NUM_WORKERS = 100;
constexpr int MIN_NUM_SOLDIERS = 1;
constexpr int SPAWN_SOLDIERS_AFTER = 150;
constexpr float POP_WORKER_TO_FOOD = 0.0017f;
constexpr float POP_WORKER_SURPLUS = 0.0071f;
constexpr int CONSERVE_WORKERS_AT = 150;
constexpr int MAX_CONTAINER_SIZE = 65'536;
//constexpr int MAX_TREE_DEPTH = 50;
//constexpr int MAX_PATH = MAX_TREE_DEPTH*MAX_TREE_DEPTH + (MAX_TREE_DEPTH + 1)*(MAX_TREE_DEPTH+1);
constexpr int MAX_PATH = 32;

enum JobCategory
{
	JOB_UNKNOWN = -1,
	JOB_GENERAL,
	JOB_MAIN,
	JOB_RENDER,
	JOB_PHYSICS,

	NUM_JOB_CATEGORIES
};

enum eQueueType
{
	UNKNOWN_QUEUE_TYPE = -1,

	QUEUE_FIFO,
	QUEUE_LIFO,
	QUEUE_PRIORITY,

	NUM_QUEUE_TYPES
};

