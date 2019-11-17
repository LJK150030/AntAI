#include "Architecture/FifoIterator.hpp"

FifoIterator::FifoIterator(const Queue collection): QueueIterator(), m_collection (collection) {}
FifoIterator::~FifoIterator() {}

void FifoIterator::Push(const short el)
{
	//m_collection.m_list.push_back(el);
}

void FifoIterator::Pop()
{
	//m_collection.m_list.pop_front();
}

short FifoIterator::Peak()
{
	//return m_collection.m_list.front();
	return -1;
}
