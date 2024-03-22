#pragma once
#include "Config.hpp"
#include <cstddef>
 #include <memory.h>

    template <class T1, class T2>
	inline void Swap(T1& a, T2& b)
	{
		T1 c(a);
		a = b;
		b = c;
	}

//! Sinks an element into the heap.
template<class T>
inline void heapsink(T*array, s32 element, s32 max)
{
	while ((element<<1) < max) // there is a left child
	{
		s32 j = (element<<1);

		if (j+1 < max && array[j] < array[j+1])
			j = j+1; // take right child

		if (array[element] < array[j])
		{
			T t = array[j]; // swap elements
			array[j] = array[element];
			array[element] = t;
			element = j;
		}
		else
			return;
	}
}


//! Sorts an array with size 'size' using heapsort.
template<class T>
inline void heapsort(T* array_, s32 size)
{
	// for heapsink we pretent this is not c++, where
	// arrays start with index 0. So we decrease the array pointer,
	// the maximum always +2 and the element always +1

	T* virtualArray = array_ - 1;
	s32 virtualSize = size + 2;
	s32 i;

	// build heap

	for (i=((size-1)/2); i>=0; --i)
		heapsink(virtualArray, i+1, virtualSize-1);

	// sort array, leave out the last element (0)
	for (i=size-1; i>0; --i)
	{
		T t = array_[0];
		array_[0] = array_[i];
		array_[i] = t;
		heapsink(virtualArray, 1, i + 1);
	}
}

template<class T>
inline void quicksort(T* array, s32 size, s32 (*comp)(T, T))
{
	if (size < 2)
		return;

	T p = array[size>>1];
	T* l = array;
	T* r = array + size - 1;
	
	while (l <= r)
	{
		if (comp(*l, p) < 0)
		{
			++l;
			continue;
		}

		if (comp(*r, p) > 0)
		{
			--r;
			continue;
		}

		T t = *l;
		*l = *r;
		*r = t;
		++l;
		--r;
	}

	quicksort(array, r - array + 1, comp);
	quicksort(l, array + size - l, comp);
}


template<class T>
inline void heapify(T* array_, s32 root, s32 end,s32 (*comp)(const T&, const T&))
{
		s32 largest = root;
		s32 left = 2 * root + 1;
		s32 right = 2 * root + 2;

		if (left < end && comp(array_[left], array_[largest]) > 0)
			largest = left;

		if (right < end && comp(array_[right], array_[largest]) > 0)
			largest = right;

		if (largest != root) {
			Swap(array_[root], array_[largest]);
			heapify(array_,largest, end,comp);
		}

}

template<class T>
inline void heapsort(T* array_, s32 size,s32 (*comp)(const T&, const T&))
{
	

    // Construir o heap
    for (s32 i = size / 2 - 1; i >= 0; --i)
        heapify(array_,i, size,comp);

    // Ordenar o heap
    for (s32 i = size - 1; i > 0; --i) 
	{
        Swap(array_[0], array_[i]);
        heapify(array_,0, i,comp);
    }
	

}

// //! Sorts an array with size 'size' using quicksort.
// template<class T>
// inline void quicksort(T* array, s32 size)
// {
// 	if (size < 2)
// 		return;

// 	T p = array[size>>1];
// 	T* l = array;
// 	T* r = array + size - 1;
	
// 	while (l <= r)
// 	{
// 		if (*l < p)
// 		{
// 			++l;
// 			continue;
// 		}

// 		if (*r > p)
// 		{
// 			--r;
// 			continue;
// 		}

// 		T t = *l;
// 		*l = *r;
// 		*r = t;
// 		++l;
// 		--r;
// 	}

// 	quicksort(array, r - array + 1);
// 	quicksort(l, array + size - l);
// }

// //! Sorts an array with size 'size' using insertion sort.
// template<class T>
// inline void insertionsort(T* array, s32 size)
// {
// 	for (s32 i=0; i<size; ++i)
// 	{
// 		T t = array[i];
// 		s32 j = i;
// 		while((j>0) && (array[j-1]>t))
// 		{
// 			array[j] = array[j-1];
// 			--j;
// 		}
// 		array[j] = t;
// 	}
// }

// //! Sorts an array with size 'size' using selection sort.
// template<class T>
// inline void selectionsort(T* array, s32 size)
// {
// 	for (s32 i=0; i<size; ++i)
// 	{
// 		s32 min = i;
// 		for (s32 j=i+1; j<size; ++j)
// 			if (array[j] < array[min])
// 				min = j;

// 		if (min != i)
// 		{
// 			T t = array[i];
// 			array[i] = array[min];
// 			array[min] = t;
// 		}
// 	}
// }

// //! Sorts an array with size 'size' using bubble sort.
// template<class T>
// inline void bubblesort(T* array, s32 size)
// {
// 	for (s32 i=0; i<size; ++i)
// 		for (s32 j=size-1; j>i; --j)
// 			if (array[j] < array[j-1])
// 			{
// 				T t = array[j];
// 				array[j] = array[j-1];
// 				array[j-1] = t;
// 			}
// }

// //! Sorts an array with size 'size' using radix.

// template<class T>
// inline void radixsort(T* array, s32 size)
// {
// 	// find the maximum number to know number of digits
// 	T m = array[0];
// 	for (s32 i=1; i<size; ++i)
// 		if (array[i] > m)
// 			m = array[i];

// 	T* b = new T[size];
// 	s32 exp = 1;

// 	while (m/exp > 0)
// 	{
// 		s32 bucket[10] = { 0 };

// 		for (s32 i=0; i<size; ++i)
// 			bucket[(array[i]/exp)%10]++;

// 		for (s32 i=1; i<10; ++i)
// 			bucket[i] += bucket[i - 1];

// 		for (s32 i=size-1; i>=0; --i)
// 			b[--bucket[(array[i]/exp)%10]] = array[i];

// 		for (s32 i=0; i<size; ++i)
// 			array[i] = b[i];

// 		exp *= 10;
// 	}

// 	delete [] b;
// }

// //! Sorts an array with size 'size' using shell sort.
// template<class T>
// inline void shellsort(T* array, s32 size)
// {
// 	for (s32 gap = size / 2; gap > 0; gap /= 2)
// 	{
// 		for (s32 i = gap; i < size; ++i)
// 		{
// 			T temp = array[i];
// 			s32 j;
// 			for (j = i; j >= gap && array[j - gap] > temp; j -= gap)
// 				array[j] = array[j - gap];
// 			array[j] = temp;
// 		}
// 	}
// }



template<typename T>
class Allocator
{
public:
	T* allocate(size_t cnt)
	{
		return (T*)operator new(cnt* sizeof(T));
		//return static_cast<T*>(std::aligned_alloc(alignof(T), cnt * sizeof(T)));
	}
	void deallocate(T* ptr)
	{
		operator delete(ptr);
		//std::free(ptr);
	}
	void construct(T* ptr, const T&e)
	{
		new ((void*)ptr) T(e);
		 
	}

	void destruct(T* ptr)
	{
		ptr->~T();
		  
	}
};
