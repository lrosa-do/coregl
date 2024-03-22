#pragma once
#include "Config.hpp"


//! Doubly linked list template.
template <class T>
class List
{
private:

	//! List element node with pointer to previous and next element in the list.
	struct SKListNode
	{
		SKListNode(const T& e) : Next(0), Prev(0), Element(e) {}

		SKListNode* Next;
		SKListNode* Prev;
		T Element;
	};

public:
	class ConstIterator;
	class Iterator
	{
	public:
		Iterator() : Current(0) {}

		Iterator& operator ++()    { Current = Current->Next; return *this; }
		Iterator& operator --()    { Current = Current->Prev; return *this; }
		Iterator  operator ++(s32) { Iterator tmp = *this; Current = Current->Next; return tmp; }
		Iterator  operator --(s32) { Iterator tmp = *this; Current = Current->Prev; return tmp; }

		Iterator& operator +=(s32 num)
		{
			if(num > 0)
			{
				while (num-- && this->Current != 0) ++(*this);
			}
			else
			{
				while(num++ && this->Current != 0) --(*this);
			}
			return *this;
		}

		Iterator  operator + (s32 num) const { Iterator tmp = *this; return tmp += num; }
		Iterator& operator -=(s32 num) { return (*this)+=(-num); }
		Iterator  operator - (s32 num) const { return (*this)+ (-num); }

		bool operator ==(const Iterator&      other) const { return Current == other.Current; }
		bool operator !=(const Iterator&      other) const { return Current != other.Current; }
		bool operator ==(const ConstIterator& other) const { return Current == other.Current; }
		bool operator !=(const ConstIterator& other) const { return Current != other.Current; }



		T & operator * () { return Current->Element; }
		T * operator ->() { return &Current->Element; }

	private:
		explicit Iterator(SKListNode* begin) : Current(begin) {}

		SKListNode* Current;

		friend class List<T>;
		friend class ConstIterator;
	};


	class ConstIterator
	{
	public:

		ConstIterator() : Current(0) {}
		ConstIterator(const Iterator& iter) : Current(iter.Current)  {}

		ConstIterator& operator ++()    { Current = Current->Next; return *this; }
		ConstIterator& operator --()    { Current = Current->Prev; return *this; }
		ConstIterator  operator ++(s32) { ConstIterator tmp = *this; Current = Current->Next; return tmp; }
		ConstIterator  operator --(s32) { ConstIterator tmp = *this; Current = Current->Prev; return tmp; }

		ConstIterator& operator +=(s32 num)
		{
			if(num > 0)
			{
				while(num-- && this->Current != 0) ++(*this);
			}
			else
			{
				while(num++ && this->Current != 0) --(*this);
			}
			return *this;
		}

		ConstIterator  operator + (s32 num) const { ConstIterator tmp = *this; return tmp += num; }
		ConstIterator& operator -=(s32 num) { return (*this)+=(-num); }
		ConstIterator  operator - (s32 num) const { return (*this)+ (-num); }

		bool operator ==(const ConstIterator& other) const { return Current == other.Current; }
		bool operator !=(const ConstIterator& other) const { return Current != other.Current; }
		bool operator ==(const Iterator&      other) const { return Current == other.Current; }
		bool operator !=(const Iterator&      other) const { return Current != other.Current; }

		const T & operator * () { return Current->Element; }
		const T * operator ->() { return &Current->Element; }

		ConstIterator & operator =(const Iterator & iterator) { Current = iterator.Current; return *this; }

	private:
		explicit ConstIterator(SKListNode* begin) : Current(begin) {}

		SKListNode* Current;

		friend class Iterator;
		friend class List<T>;
	};

	//! Default constructor for empty list.
	List()
		: First(0), Last(0), Size(0) {}


	//! Copy constructor.
	List(const List<T>& other) : First(0), Last(0), Size(0)
	{
		*this = other;
	}


	//! Destructor
	~List()
	{
		clear();
	}


	//! Assignment operator
	void operator=(const List<T>& other)
	{
		if(&other == this)
		{
			return;
		}

		clear();

		SKListNode* node = other.First;
		while(node)
		{
			push_back(node->Element);
			node = node->Next;
		}
	}


	//! Returns amount of elements in list.
	/** \return Amount of elements in the list. */
	u32 size() const
	{
		return Size;
	}
	u32 getSize() const
	{
		return Size;
	}

    Iterator find(const T& element)
    {
        for (Iterator it = begin(); it != end(); ++it)
        {
            if (*it == element)
                return it;
        }
        return end();
    }


	//! Clears the list, deletes all elements in the list.
	/** All existing iterators of this list will be invalid. */
	void clear()
	{
		while(First)
		{
			SKListNode * next = First->Next;
			allocator.destruct(First);
			allocator.deallocate(First);
			First = next;
		}

		//First = 0; handled by loop
		Last = 0;
		Size = 0;
	}


	//! Checks for empty list.
	/** \return True if the list is empty and false if not. */
	bool empty() const
	{
		return (First == 0);
	}


	//! Adds an element at the end of the list.
	/** \param element Element to add to the list. */
	void push_back(const T& element)
	{
		SKListNode* node = allocator.allocate(1);
		allocator.construct(node, element);

		++Size;

		if (First == 0)
			First = node;

		node->Prev = Last;

		if (Last != 0)
			Last->Next = node;

		Last = node;
	}


	//! Adds an element at the begin of the list.
	/** \param element: Element to add to the list. */
	void push_front(const T& element)
	{
		SKListNode* node = allocator.allocate(1);
		allocator.construct(node, element);

		++Size;

		if (First == 0)
		{
			Last = node;
			First = node;
		}
		else
		{
			node->Next = First;
			First->Prev = node;
			First = node;
		}
	}


	//! Gets first node.
	/** \return A list iterator pointing to the beginning of the list. */
	Iterator begin()
	{
		return Iterator(First);
	}


	//! Gets first node.
	/** \return A const list iterator pointing to the beginning of the list. */
	ConstIterator begin() const
	{
		return ConstIterator(First);
	}


	//! Gets end node.
	/** \return List iterator pointing to null. */
	Iterator end()
	{
		return Iterator(0);
	}


	//! Gets end node.
	/** \return Const list iterator pointing to null. */
	ConstIterator end() const
	{
		return ConstIterator(0);
	}


	//! Gets last element.
	/** \return List iterator pointing to the last element of the list. */
	Iterator getLast()
	{
		return Iterator(Last);
	}


	//! Gets last element.
	/** \return Const list iterator pointing to the last element of the list. */
	ConstIterator getLast() const
	{
		return ConstIterator(Last);
	}


	//! Inserts an element after an element.
	/** \param it Iterator pointing to element after which the new element
	should be inserted.
	\param element The new element to be inserted into the list.
	*/
	void insert_after(const Iterator& it, const T& element)
	{
		SKListNode* node = allocator.allocate(1);
		allocator.construct(node, element);

		node->Next = it.Current->Next;

		if (it.Current->Next)
			it.Current->Next->Prev = node;

		node->Prev = it.Current;
		it.Current->Next = node;
		++Size;

		if (it.Current == Last)
			Last = node;
	}


	//! Inserts an element before an element.
	/** \param it Iterator pointing to element before which the new element
	should be inserted.
	\param element The new element to be inserted into the list.
	*/
	void insert_before(const Iterator& it, const T& element)
	{
		SKListNode* node = allocator.allocate(1);
		allocator.construct(node, element);

		node->Prev = it.Current->Prev;

		if (it.Current->Prev)
			it.Current->Prev->Next = node;

		node->Next = it.Current;
		it.Current->Prev = node;
		++Size;

		if (it.Current == First)
			First = node;
	}


	//! Erases an element.
	/** \param it Iterator pointing to the element which shall be erased.
	\return Iterator pointing to next element. */
	Iterator erase(Iterator& it)
	{
		// suggest changing this to a const Iterator& and
		// working around line: it.Current = 0 (possibly with a mutable, or just let it be garbage?)

		Iterator returnIterator(it);
		++returnIterator;

		if(it.Current == First)
		{
			First = it.Current->Next;
		}
		else
		{
			it.Current->Prev->Next = it.Current->Next;
		}

		if(it.Current == Last)
		{
			Last = it.Current->Prev;
		}
		else
		{
			it.Current->Next->Prev = it.Current->Prev;
		}

		allocator.destruct(it.Current);
		allocator.deallocate(it.Current);
		it.Current = 0;
		--Size;

		return returnIterator;
	}

	//! Swap the content of this list container with the content of another list
	/** Afterwards this object will contain the content of the other object and the other
	object will contain the content of this object. Iterators will afterwards be valid for
	the swapped object.
	\param other Swap content with this object	*/
	void swap(List<T>& other)
	{
		Swap(First, other.First);
		Swap(Last, other.Last);
		Swap(Size, other.Size);
		Swap(allocator, other.allocator);	// memory is still released by the same allocator used for allocation
	}


private:

	SKListNode* First;
	SKListNode* Last;
	u32 Size;
	Allocator<SKListNode> allocator;

};

