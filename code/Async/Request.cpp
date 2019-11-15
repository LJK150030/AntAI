#include "Async/Request.hpp"
#include "Async/Dispatcher.hpp"

//-----------------------------------------------------------------

void RequestPrint::SetValue(const int value)
{
	this->m_value = value;
}


void RequestPrint::SetOutput(const LogFunction fnc)
{
	m_outFnc = fnc;
}


void RequestPrint::Process()
{
	m_outFnc("Starting processing request " + std::to_string(m_value) + "... \n");
}


void RequestPrint::Finish()
{
	m_outFnc("Finished request " + std::to_string(m_value) + "\n");
}

//-----------------------------------------------------------------