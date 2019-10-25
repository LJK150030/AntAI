#include "Arena\ArenaPlayerInterface.hpp"

// info collection
int GiveCommonInterfaceVersion()
{
	return COMMON_INTERFACE_VERSION_NUMBER;
}


const char* GivePlayerName()
{
	return "The LAW";
}


const char* GiveAuthorName()
{
	return "Lawrence(JAKE)";
}


// setup
void PreGameStartup( const StartupInfo& info )
{
	//copy game rules over
}


void PostGameShutdown( const MatchResults& results )
{
	// any clean up or reporting
}


void PlayerThreadEntry( int yourThreadIdx )
{
	// a thread entry - do work here
}



// Turn
void ReceiveTurnState( const ArenaTurnStateForPlayer& state )
{
	// copy stat, kick off work
}


bool TurnOrderRequest( int turnNumber, PlayerTurnOrders* ordersToFill )
{
	// return if orders was filled (this is a valid turn)
	ordersToFill->numberOfOrders = 0;
	return true;
}