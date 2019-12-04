#pragma once

template <typename Item>
class MinHeap
{
public:
	MinHeap(int size);
	~MinHeap();

	//mutators
	void	Push(Item value);
	Item	Pop();
	void	DeleteAtIdx(int idx);
	void	UpdateAtIdx(int idx, Item new_val);

	//accessors
	int		GetSize() const;
	bool	Contains(Item value) const;
	int		GetIndex(Item value) const;
	Item	GetReferenceAt(int idx) const;

	
private:
	int GetLeftChildIdx(int idx);
	int GetRightChildIdx(int idx);
	int GetParentIdx(int idx);
	void Swap(int idx_a, int idx_b);
	void MinHeapify(int idx);
	
private:
	Item* m_heap;
	int m_size;
};


template <typename Item>
MinHeap<Item>::MinHeap(const int size)
{
	m_heap = new Item[size];
	m_size = 0;
}

template <typename Item>
MinHeap<Item>::~MinHeap()
{
	delete[] m_heap;
}


template <typename Item>
void MinHeap<Item>::Push(Item value)
{
	++m_size;
	int current_idx = m_size - 1; 
	int parent_idx = GetParentIdx(current_idx);
	m_heap[current_idx] = (value);

	while (current_idx != 0 && m_heap[parent_idx] > m_heap[current_idx]) 
	{ 
		Swap(current_idx, parent_idx); 
		current_idx = GetParentIdx(current_idx);
		parent_idx = GetParentIdx(current_idx);
	}
}


template <typename Item>
Item MinHeap<Item>::Pop()
{
	if (m_size <= 0)
	{
		return Item();
	}

	if (m_size == 1) 
	{ 
		m_size--; 
		return m_heap[0]; 
	} 

	// Store the minimum value, and remove it from heap 
	Item root = m_heap[0]; 
	m_heap[0] = m_heap[m_size-1]; 
	m_size--; 
	MinHeapify(0); 

	return root; 
}


template <typename Item>
void MinHeap<Item>::DeleteAtIdx(const int idx)
{
	UpdateAtIdx(idx, Item()); 
	Pop(); 
}


template <typename Item>
void MinHeap<Item>::UpdateAtIdx(int idx, Item new_val)
{
	int updated_idx = idx;
	m_heap[updated_idx] = new_val;
	
	while (updated_idx != 0 && m_heap[GetParentIdx(updated_idx)] > m_heap[updated_idx]) 
	{ 
		Swap(updated_idx, GetParentIdx(updated_idx)); 
		updated_idx = GetParentIdx(updated_idx); 
	} 
}


template <typename Item>
int MinHeap<Item>::GetSize() const
{
	return m_size;
}


template <typename Item>
bool MinHeap<Item>::Contains(Item value) const
{
	for(int idx = 0; idx < m_size; ++idx)
	{
		if(value == m_heap[idx]) return true;
	}
	
	return false;
}


template <typename Item>
int MinHeap<Item>::GetIndex(Item value) const
{
	for(int idx = 0; idx < m_size; ++idx)
	{
		if(value == m_heap[idx]) return idx;
	}

	return -1;
}


template <typename Item>
Item MinHeap<Item>::GetReferenceAt(int idx) const
{
	return m_heap[idx];
}


template <typename Item>
int MinHeap<Item>::GetLeftChildIdx(int idx)
{
	return 2 * idx;
}


template <typename Item>
int MinHeap<Item>::GetRightChildIdx(int idx)
{
	return 2 * idx + 1;
}


template <typename Item>
int MinHeap<Item>::GetParentIdx(int idx)
{
	return idx / 2;
}


template <typename Item>
void MinHeap<Item>::Swap(int idx_a, int idx_b)
{
	Item temp = m_heap[idx_a]; 
	m_heap[idx_a] = m_heap[idx_b]; 
	m_heap[idx_b] = temp;
}


template <typename Item>
void MinHeap<Item>::MinHeapify(int idx)
{
	int left_child = GetLeftChildIdx(idx); 
	int right_child = GetRightChildIdx(idx); 
	int smallest_value_idx = idx;

	if (left_child < m_size && m_heap[left_child] < m_heap[idx])
	{
		smallest_value_idx = left_child;
	}

	if (right_child < m_size && m_heap[right_child] < m_heap[smallest_value_idx])
	{
		smallest_value_idx = right_child; 
	}

	if (smallest_value_idx != idx) 
	{ 
		Swap(idx, smallest_value_idx); 
		MinHeapify(smallest_value_idx); 
	} 
}
