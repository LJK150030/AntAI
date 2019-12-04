#include "Arena\ArenaPlayerInterface.hpp"
#include "Blackboard.hpp"

// info collection
int GiveCommonInterfaceVersion() {	return COMMON_INTERFACE_VERSION_NUMBER; }
const char* GivePlayerName() {	return "jak_sin(Pi)"; }
const char* GiveAuthorName() {	return "Lawrence Klinkert"; }


// setup -- copy game rules over
void PreGameStartup( const StartupInfo& info )
{
	MainThread* the_player = MainThread::GetInstance();
	the_player->Startup( info );
}


// any clean up or reporting
void PostGameShutdown( const MatchResults& results )
{
	MainThread* the_player = MainThread::GetInstance();
	the_player->Shutdown( results );

	while(the_player->m_numActiveThreads != 0)
	{
		std::this_thread::yield();
	}

	MainThread::DeleteInstance();
}


// a thread entry -- do work here
// we are given a thread index, however, this is more so the threads entry point
// get the threads, and add them to a pool
void PlayerThreadEntry( int yourThreadIdx )
{
	// this Ai is "bad" - I'm only going to use one worker thread
	if (yourThreadIdx == 0) 
	{
		MainThread* the_player = MainThread::GetInstance();
		the_player->ThreadEntry( yourThreadIdx );
	}
}


// Turn -- copy stat, kick off work
void ReceiveTurnState( const ArenaTurnStateForPlayer& state )
{
	MainThread* the_player = MainThread::GetInstance();
	the_player->ReceiveTurnState( state );
}


// return if orders was filled (this is a valid turn)
bool TurnOrderRequest( int turnNumber, PlayerTurnOrders* ordersToFill )
{
	MainThread* the_player = MainThread::GetInstance();
	return the_player->TurnOrderRequest( ordersToFill );
}