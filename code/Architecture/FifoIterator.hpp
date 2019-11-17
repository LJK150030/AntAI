#pragma once
#include "Architecture/QueueIterator.hpp"
#include "Architecture/Queue.hpp"

class FifoIterator : QueueIterator
{
public:
	explicit FifoIterator( Queue collection );
	~FifoIterator();
	
	void Push(short el) override;
	void Pop() override;
	short Peak() override;

private:
	Queue m_collection;
};
