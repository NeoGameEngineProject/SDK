#include "FixedString.h"

#include <cstring>
#include <cassert>

#include <ostream>
#include <istream>

using namespace Neo;

bool Neo::operator==(const char* str1, const IString& str2)
{
	return str2 == str1;
}

bool Neo::operator!=(const char* str1, const IString& str2)
{
	return str2 != str1;
}

bool Neo::operator==(const std::string& str1, const IString& str2)
{
	return str2 == str1;
}

bool Neo::operator!=(const std::string& str1, const IString& str2)
{
	return str2 != str1;
}

std::ostream& Neo::operator<<(std::ostream& out, const IString& str)
{
	out << str.str();
}

unsigned int IString::getLength() const
{
	return strlen(str());
}

void IString::set(const char* str)
{
	assert(strlen(str) < getSize() && "The string is too big to fit!");
	strncpy(getData(), str, getSize());
}

#include <Log.h>
void IString::serialize(std::ostream& out) const
{
	uint16_t size = getLength();
	out.write((char*) &size, sizeof(size));
	out.write(str(), size);
}

void IString::deserialize(std::istream& in)
{
	uint16_t size = 0;
	in.read((char*) &size, sizeof(size));
	
	assert(getSize() > size && "The string it too big to fit!");
	in.read(getData(), size);
	getData()[size] = 0;
}

IString& IString::operator=(const char* str)
{
	set(str);
	return *this;
}

IString& IString::operator=(const std::string& str)
{
	set(str.c_str());
	return *this;
}

IString& IString::operator=(const IString& str)
{
	set(str.str());
	return *this;
}

IString& IString::operator+=(const char* str)
{
	assert(getLength() + strlen(str) < getSize() && "The string is too big to fit!");
	strncat(getData(), str, getSize());
	return *this;
}

IString& IString::operator+=(const std::string& str)
{
	return *this += str.c_str();
}

IString& IString::operator+=(const IString& str)
{
	return *this += str.str();
}

bool IString::operator==(const IString& other) const
{
	return *this == other.str();
}

bool IString::operator!=(const IString& other) const
{
	return !(*this == other);
}

bool IString::operator==(const char* other) const
{
	return !strncmp(str(), other, getSize());
}

bool IString::operator!=(const char* other) const
{
	return !(*this == other);
}

bool IString::operator==(const std::string& other) const
{
	return *this == other.c_str();
}

bool IString::operator!=(const std::string& other) const
{
	return !(*this == other);
}

	
