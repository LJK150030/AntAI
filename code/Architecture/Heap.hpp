#pragma once
#include "Blackboard.hpp"

TODO("This is over engineered. Use heap memeory since these are going to be big objecs")
template <typename T, int Size = MAX_PATH>
class MinHeap
{
public:
	MinHeap();

	//mutators
	void	Push(T value);
	T		Pop();
	void	DeleteAtIdx(int idx);
	void	UpdateAtIdx(int idx, T new_val);

	//accessors
	int		GetSize() const;
	bool	Contains(T value) const;
	int		GetIndex(T value) const;
	const T	GetReferenceAt(int idx) const;

	
private:
	int GetLeftChildIdx(int idx);
	int GetRightChildIdx(int idx);
	int GetParentIdx(int idx);
	void Swap(int idx_a, int idx_b);
	void MinHeapify(int idx);
	
private:
	T m_heap[Size];
	int m_size = 0;
};


template <typename T, int Size>
MinHeap<T, Size>::MinHeap() { }


template <typename T, int Size>
void MinHeap<T, Size>::Push(T value)
{
	if (m_size == Size) 
	{ 
		return; 
	}

	++m_size;
	int current_idx = m_size - 1; 
	int parent_idx = GetParentIdx(current_idx);
	m_heap[current_idx] = value;

	while (current_idx != 0 && m_heap[parent_idx] > m_heap[current_idx]) 
	{ 
		Swap(current_idx, parent_idx); 
		current_idx = GetParentIdx(current_idx);
		parent_idx = GetParentIdx(current_idx);
	}
}


template <typename T, int Size>
T MinHeap<T, Size>::Pop()
{
	if (m_size <= 0)
	{
		return T();
	}

	if (m_size == 1) 
	{ 
		m_size--; 
		return m_heap[0]; 
	} 

	// Store the minimum value, and remove it from heap 
	T root = m_heap[0]; 
	m_heap[0] = m_heap[m_size-1]; 
	m_size--; 
	MinHeapify(0); 

	return root; 
}


template <typename T, int Size>
void MinHeap<T, Size>::DeleteAtIdx(int idx)
{
	UpdateAtIdx(idx, T()); 
	Pop(); 
}


template <typename T, int Size>
void MinHeap<T, Size>::UpdateAtIdx(int idx, T new_val)
{
	int updated_idx = idx;
	m_heap[updated_idx] = new_val;
	
	while (updated_idx != 0 && m_heap[GetParentIdx(updated_idx)] > m_heap[updated_idx]) 
	{ 
		Swap(updated_idx, GetParentIdx(updated_idx)); 
		updated_idx = GetParentIdx(updated_idx); 
	} 
}


template <typename T, int Size>
int MinHeap<T, Size>::GetSize() const
{
	return m_size;
}


template <typename T, int Size>
bool MinHeap<T, Size>::Contains(T value) const
{
	for(int idx = 0; idx < m_size; ++idx)
	{
		if(value == m_heap[idx]) return true;
	}
	
	return false;
}


template <typename T, int Size>
int MinHeap<T, Size>::GetIndex(T value) const
{
	for(int idx = 0; idx < m_size; ++idx)
	{
		if(value == m_heap[idx]) return idx;
	}

	return -1;
}


template <typename T, int Size>
const T MinHeap<T, Size>::GetReferenceAt(int idx) const
{
	return m_heap[idx];
}


template <typename T, int Size>
int MinHeap<T, Size>::GetLeftChildIdx(int idx)
{
	return 2 * idx;
}


template <typename T, int Size>
int MinHeap<T, Size>::GetRightChildIdx(int idx)
{
	return 2 * idx + 1;
}


template <typename T, int Size>
int MinHeap<T, Size>::GetParentIdx(int idx)
{
	return idx / 2;
}


template <typename T, int Size>
void MinHeap<T, Size>::Swap(int idx_a, int idx_b)
{
	T temp = m_heap[idx_a]; 
	m_heap[idx_a] = m_heap[idx_b]; 
	m_heap[idx_b] = temp;
}


template <typename T, int Size>
void MinHeap<T, Size>::MinHeapify(int idx)
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
