#ifndef ARRAY_H
#define ARRAY_H

#include <cstdlib>
#include <cstring>

namespace Neo
{

/**
 * @brief Implements a simple resizable array.
 * @author Yannick Pflanzer
 */
template<typename T> 
struct Array
{
	Array() = default;
	
	/**
	 * @brief Copy constructor.
	 */
	Array(const Array& a)
	{
		*this = a;
	}
	
	/**
	 * @brief Move constructor.
	 */
	Array(Array&& a) : data(a.data), count(a.count)
	{
		a.data = nullptr;
		a.count = 0;
	}
	
	/**
	 * @brief Assignment operator.
	 * 
	 * Copies the array by allocating new memory and copying all content into it.
	 * 
	 * @param a The array to copy.
	 * @return A reference to \b this
	 */
	Array<T>& operator=(const Array<T>& a)
	{
		set(a.data, a.count);
		return *this;
	}

	~Array()
	{
		if(data)
		{
			for(size_t i = 0; i < count; i++)
			{
				// Call destructors
				data[i].~T();
			}
			
			free(data);
		}
	}

	/**
	 * @brief Allocates a certain amount of memory and constructs objects.
	 * 
	 * Resizing is allowed. If the new size is smaller than the old size
	 * elements will be cut off at the end.
	 * 
	 * @todo Error handling!
	 * @param count The number of elements in the array.
	 */
	void alloc(unsigned int count)
	{
		data = reinterpret_cast<T*>(realloc(data, count * sizeof(T)));
		for(size_t i = this->count; i < count; i++)
			new (data + i) T;

		this->count = count;
	}
	
	/**
	 * @brief Sets the contents of the array.
	 * 
	 * This will erase all existing data, resize the memory and copy
	 * the data.
	 * 
	 * @param d The array of elements to copy from.
	 * @param count The number of elements to copy.
	 */
	void set(const T* d, unsigned int count)
	{
		if(!d || !count)
			return;

		alloc(count);
		memcpy(data, d, sizeof(T) * count);
	}
	
	/**
	 * @brief Clears all internal state and releases taken memory.
	 */
	void clear()
	{
		delete data;
		count = 0;
		data = nullptr;
	}
	
	/**
	 * @brief Fetches a reference to the element at the given index.
	 */
	T& operator[](const size_t idx) { return data[idx]; }
	
	T* data = nullptr;
	unsigned int count = 0;
};

}

#endif
