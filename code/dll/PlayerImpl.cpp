#include "Arena\ArenaPlayerInterface.hpp"
#include "Blackboard.hpp"

// info collection
int GiveCommonInterfaceVersion() {	return COMMON_INTERFACE_VERSION_NUMBER; }
const char* GivePlayerName() {	return "jak_sin(Pi)"; }
const char* GiveAuthorName() {	return "Lawrence(JAKE)"; }


// setup -- copy game rules over
void PreGameStartup( const StartupInfo& info )
{
	g_thePlayer = new MainThread(); 
	g_thePlayer->Startup( info ); 
}


// any clean up or reporting
void PostGameShutdown( const MatchResults& results )
{
	g_thePlayer->Shutdown( results ); 
	delete g_thePlayer; 
}


// a thread entry -- do work here
// we are given a thread index, however, this is more so the threads entry point
// get the threads, and add them to a pool
void PlayerThreadEntry( int yourThreadIdx )
{
	// this Ai is "bad" - I'm only going to use one worker thread
	if (yourThreadIdx == 0) 
	{
		g_thePlayer->ThreadEntry( yourThreadIdx ); 
	}
}


// Turn -- copy stat, kick off work
void ReceiveTurnState( const ArenaTurnStateForPlayer& state )
{
	g_thePlayer->ReceiveTurnState( state ); 
}


// return if orders was filled (this is a valid turn)
bool TurnOrderRequest( int turnNumber, PlayerTurnOrders* ordersToFill )
{
	return g_thePlayer->TurnOrderRequest( ordersToFill ); 
}