#pragma once
#include "Blackboard.hpp"

class AbstractRequest
{
public:
	virtual void Process() = 0;
	virtual void Finish() = 0;

	JobCategory GetCategory() const { return m_jobCategory; }
	void SetCategory (const JobCategory category) { m_jobCategory = category; }

private:
	JobCategory m_jobCategory = JOB_GENERAL;
};

