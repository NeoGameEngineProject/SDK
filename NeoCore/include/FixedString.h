#ifndef NEO_FIXEDSTRING_H
#define NEO_FIXEDSTRING_H

#include <string>
#include "NeoCore.h"

namespace Neo 
{

/**
 * @brief Implements the interface of a string.
 */
class NEO_CORE_EXPORT IString
{
protected:
	/**
	 * @brief Retrieves a writable pointer to the internal buffer.
	 * @return The buffer.
	 */
	virtual char* getData() = 0;
public:
	/**
	 * @brief Returns the size of the internal buffer.
	 * @return The size of the buffer.
	 */
	virtual unsigned int getSize() const = 0;
	
	/**
	 * @brief Returns the held nul terminated string.
	 * @return The string
	 */
	virtual const char* str() const = 0;
	
	/**
	 * @brief Retrieves the number of characters in the string.
	 * @return The length of the string.
	 */
	unsigned int getLength() const;
	
	/**
	 * @brief Copies to the internal buffer.
	 * @param str The nul terminated string to copy.
	 */
	void set(const char* str);
	
	IString& operator=(const char* str);
	IString& operator=(const std::string& str);
	IString& operator=(const IString& str);
	
	IString& operator+=(const char* str);
	IString& operator+=(const std::string& str);
	IString& operator+=(const IString& str);
	
	bool operator==(const IString& other) const;
	bool operator!=(const IString& other) const;
	
	bool operator==(const char* other) const;
	bool operator!=(const char* other) const;
	
	bool operator==(const std::string& other) const;
	bool operator!=(const std::string& other) const;
	
	void serialize(std::ostream& out) const;
	void deserialize(std::istream& in);
};

NEO_CORE_EXPORT bool operator==(const char* str1, const IString& str2);
NEO_CORE_EXPORT bool operator!=(const char* str1, const IString& str2);

NEO_CORE_EXPORT bool operator==(const std::string& str1, const IString& str2);
NEO_CORE_EXPORT bool operator!=(const std::string& str1, const IString& str2);

NEO_CORE_EXPORT std::ostream& operator<<(std::ostream& out, const IString& str);

/**
 * @brief Implements a fixed size string.
 * @tparam SIZE The maximum number of characters to store.
 */
template <unsigned int SIZE = 32>
class FixedString : public IString
{
	char m_data[SIZE + 1]; // +1 for the null byte
protected:
	char* getData() override { return m_data; }
	
public:
	FixedString() 
	{
		static_assert(SIZE > 0, "A string needs to have a positive size!");
		m_data[0] = 0;
		
		// This is an element the parent class will never access and thus the string is ALWAYS nul terminated!
		m_data[SIZE] = 0;
	}
	
	FixedString(const char* data) 
	{ 
		set(data);
	}
	
	FixedString(const std::string& data) 
	{
		set(data.c_str());
	}
	
	FixedString(const IString& data)
	{
		set(data.str());
	}
	
	unsigned int getSize() const override { return SIZE; }
	const char* str() const override { return m_data; }
};

}

#endif // NEO_FIXEDSTRING_H
