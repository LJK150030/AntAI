#pragma once
#include "Character/AntUnit.hpp"

struct AgentReport;

class AntPool
{	
public:
	AntPool();
	~AntPool();
	AntUnit* AddAnt(AgentReport report); //AllocBlock
	// void RemoveAnt(AgentReport report); //FreeBlock

private:
	static const int POOL_SIZE = 200;
	AntUnit m_pool[POOL_SIZE];
};
