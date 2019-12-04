#include "Architecture/AntPool.hpp"
#include "ErrorWarningAssert.hpp"

AntPool::AntPool() {}
AntPool::~AntPool() {}

AntUnit* AntPool::AddAnt(AgentReport report)
{
	for (int i = 0; i < POOL_SIZE; i++)
	{
		if (!m_pool[i].InUse())
		{
			m_pool[i].Init(report);
			return &m_pool[i];
		}
	}

	//TODO: Get rid of this during tournament
	ASSERT_OR_DIE(false, "RAN OUT OF SAPCE IN ANT POOL!");
}

