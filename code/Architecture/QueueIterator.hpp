#pragma once
#include "Blackboard.hpp"

//traversing the Queue object
class QueueIterator
{
public:
	virtual void Push(short el) = 0;
	virtual void Pop() = 0;
	virtual short Peak() = 0;

	
};