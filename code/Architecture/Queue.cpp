#include "Architecture/Queue.hpp"
#include "Architecture/FifoIterator.hpp"

void Queue::Clear()
{
	m_list.clear();
}


bool Queue::IsEmpty() const
{
	return m_list.empty();
}

QueueIterator* Queue::CreateFifoIterator() const
{
	//return new FifoIterator( *this );
	return nullptr;
}


