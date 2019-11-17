#pragma once
#include <list>

class QueueIterator;

//wrapper class for the std::list using short
class Queue
{
	friend QueueIterator;
	
public:
	//similar to all queue types
	void	Clear();
	bool	IsEmpty() const;

	QueueIterator* CreateFifoIterator() const;
	//QueueIterator* CreateLifoIterator();
	//QueueIterator* CreatePriorityIterator();

private:
	std::list<short>	m_list;
};
