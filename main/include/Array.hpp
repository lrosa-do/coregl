#pragma once
#include "Config.hpp"
#include "Allocator.hpp"
#include <cstring> 
#include <algorithm>

template <class T, typename TAlloc = Allocator<T> > 
class Array
{

public:


	Array()
		: data(0), allocated(0), used(0), is_sorted(true)
	{
	}

	Array(u32 start_count)
      : data(0), allocated(0), used(0), is_sorted(true)
	{
		reallocate(start_count);
	}

	Array(const Array<T, TAlloc>& other) : data(0)
	{
		*this = other;
	}

	~Array()
	{
		clear();
	}


	//! Reallocates the Array, make it bigger or smaller.
	/** \param new_size New size of Array.
	\param canShrink Specifies whether the Array is reallocated even if
	enough space is available. Setting this flag to false can speed up
	Array usage, but may use more memory than required by the data.
	*/
	void reallocate(u32 new_size, bool canShrink=true)
	{
		if (allocated==new_size)
			return;
		if (!canShrink && (new_size < allocated))
			return;

		T* old_data = data;

		data = allocator.allocate(new_size); 
		allocated = new_size;
		s32 end = used < new_size ? used : new_size;

		for (s32 i=0; i<end; ++i)
		{
			allocator.construct(&data[i], old_data[i]);
		}
		for (u32 j=0; j<used; ++j)
			allocator.destruct(&old_data[j]);

		if (allocated < used)
			used = allocated;

		allocator.deallocate(old_data); 
	}


	void resize(u32 new_size)
	{
		reallocate(new_size);
		used = new_size;
	}


	void push_back(const T& element)
	{
		insert(element, used);
	}



	void push_front(const T& element)
	{
		insert(element);
	}



	void insert(const T& element, u32 index=0)
	{
		DEBUG_BREAK_IF(index > used) // access violation

		if (used + 1 > allocated)
		{
			const T e(element);

			// Aumentar a capacidade em blocos maiores
			u32 newAlloc = (allocated == 0) ? 1 : allocated * 2;
			
			reallocate(newAlloc);


		//	std::memmove(data + index + 1, data + index, (used - index) * sizeof(T));
			std::copy(data + index, data + used, data + index + 1);


			allocator.construct(&data[index], e);
		}
		else
		{

			allocator.construct(&data[index], element);
		}

		++used;
		is_sorted = false;
	}




	void clear()
	{
		
        for (u32 i=0; i<used; ++i)
            allocator.destruct(&data[i]);

		allocator.deallocate(data); 
		
		data = 0;
		used = 0;
		allocated = 0;
		is_sorted = true;
	}


	//! Sets pointer to new Array, using this as new workspace.
	/**
	\param newPointer: Pointer to new Array of elements.
	\param size: Size of the new Array.
	\param _is_sorted Flag which tells whether the new Array is already
	sorted.
 */
	void set_pointer(T* newPointer, u32 size, bool _is_sorted=false)
	{
		clear();
		data = newPointer;
		allocated = size;
		used = size;
		is_sorted = _is_sorted;

	}





	//! Sets the size of the Array and allocates new elements if necessary.
	/** Please note: This is only secure when using it with simple types,
	because no default constructor will be called for the added elements.
	\param usedNow Amount of elements now used. */
	void set_used(u32 usedNow)
	{
		if (allocated < usedNow)
			reallocate(usedNow);

		used = usedNow;
	}


	const Array<T, TAlloc>& operator=(const Array<T, TAlloc>& other)
	{
		if (this == &other)
			return *this;


		if (data)
			clear();


		if (other.allocated == 0)
			data = 0;
		else
			data = allocator.allocate(other.allocated); 

		used = other.used;

		is_sorted = other.is_sorted;
		allocated = other.allocated;

		for (u32 i=0; i<other.used; ++i)
			allocator.construct(&data[i], other.data[i]); 

		return *this;
	}


	//! Equality operator
	bool operator == (const Array<T, TAlloc>& other) const
	{
		if (used != other.used)
			return false;

		for (u32 i=0; i<other.used; ++i)
			if (data[i] != other[i])
				return false;
		return true;
	}


	//! Inequality operator
	bool operator != (const Array<T, TAlloc>& other) const
	{
		return !(*this==other);
	}


	//! Direct access operator
	T& operator [](u32 index)
	{
		DEBUG_BREAK_IF(index>=used) // access violation

		return data[index];
	}


	//! Direct const access operator
	const T& operator [](u32 index) const
	{
		DEBUG_BREAK_IF(index>=used) // access violation

		return data[index];
	}


	//! Gets last element.
	T& getLast()
	{
		DEBUG_BREAK_IF(!used) // access violation

		return data[used-1];
	}


	//! Gets last element
	const T& getLast() const
	{
		DEBUG_BREAK_IF(!used) // access violation

		return data[used-1];
	}



	T* pointer()
	{
		return data;
	}


	const T* const_pointer() const
	{
		return data;
	}

	u32 size() const
	{
		return used;
	}



	const T*  Data() const
	{
		return data;
	}

	T* Data()
	{
		return data;
	}

	//! Get amount of memory allocated.
	/** \return Amount of memory allocated. The amount of bytes
	allocated would be allocated_size() * sizeof(ElementTypeUsed); */
	u32 allocated_size() const
	{
		return allocated;
	}


	bool empty() const
	{
		return used == 0;
	}





	s32 binary_search(const T& element) const
	{
		if (is_sorted)
			return binary_search(element, 0, used-1);
		else
			return linear_search(element);
	}

	s32 binary_search(const T& element, s32 left, s32 right) const
	{
		// if (!used)
		// 	return -1;

		// s32 m;

		// do
		// {
		// 	m = (left+right)>>1;

		// 	if (element < data[m])
		// 		right = m - 1;
		// 	else
		// 		left = m + 1;

		// } while((element < data[m] || data[m] < element) && left<=right);

		// if (!(element < data[m]) && !(data[m] < element))
		// 	return m;
		if (!used || left > right)
				return -1;

			while (left < right) 
			{
				s32 m = (left + right) >> 1;

				if (element < data[m])
					right = m;
				else
					left = m + 1;
			}

			if (element == data[left])
				return left;

		return -1;
	}

	s32 linear_search(const T& value) const
	{
	
		const T* ptr = data;
		const T* end = data + used;
		for (; ptr != end; ++ptr) 
		{
			if (*ptr == value) 
			{
				return ptr - data; 
			}
		}
		return -1;
	}


	s32 find(const T& value) const 
	{
		
		if (is_sorted)
		{
			return binary_search(value);
		}
		return linear_search(value); 
	}


	//! Sorts the Array using heapsort.
	/** There is no additional memory waste and the algorithm performs
	O(n*log n) in worst case. */
	void Sort()
	{
		if (!is_sorted && used>1)
			heapsort(data, used);
		is_sorted = true;
	}

	void Sort(s32(*compare)(const T&, const T&))
	{
		if (!is_sorted && used>1)
			heapsort(data, used, compare);
		is_sorted = true;
	}

	void QuickSort(s32(*compare)(T, T))
	{
		if (!is_sorted && used>1)
			quicksort(data, used, compare);
		is_sorted = true;
	}


	s32 linear_reverse_search(const T& element) const
	{
		for (s32 i=used-1; i>=0; --i)
			if (data[i] == element)
				return i;

		return -1;
	}

	void erase(u32 index)
	{
		DEBUG_BREAK_IF(index>=used) // access violation

		for (u32 i=index+1; i<used; ++i)
		{
			allocator.destruct(&data[i-1]);
			allocator.construct(&data[i-1], data[i]); 
		}

		allocator.destruct(&data[used-1]);

		--used;
	}


	void erase(u32 index, s32 count)
	{
		if (index>=used || count<1)
			return;
		if (index+count>used)
			count = used-index;

		u32 i;
		for (i=index; i<index+count; ++i)
			allocator.destruct(&data[i]);

		for (i=index+count; i<used; ++i)
		{
			if (i-count >= index+count)	// not already destructed before loop
				allocator.destruct(&data[i-count]);

			allocator.construct(&data[i-count], data[i]); // data[i-count] = data[i];

			if (i >= used-count)	// those which are not overwritten
				allocator.destruct(&data[i]);
		}

		used-= count;
	}



	void swap(Array<T, TAlloc>& other)
	{
		Swap(data, other.data);
		Swap(allocated, other.allocated);
		Swap(used, other.used);
		Swap(allocator, other.allocator);	// memory is still released by the same allocator used for allocation
		bool helper_is_sorted(is_sorted);
		is_sorted = other.is_sorted;
		other.is_sorted = helper_is_sorted;
	}


private:
	T* data;
	u32 allocated;
	u32 used;
	TAlloc allocator;
	bool is_sorted:1;
};


