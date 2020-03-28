#ifndef __ARRAY_STREAM_H__
#define __ARRAY_STREAM_H__

#include <iostream>

namespace Neo
{
class ArrayStream: public std::istream
{
public:
	ArrayStream(void* data, size_t size):
		std::istream(&m_buffer),
		m_buffer(data, size)
	{
		rdbuf(&m_buffer);
	}

private:
	class ArrayBuffer: public std::basic_streambuf<char>
	{
	public:
		ArrayBuffer(void* data, size_t size)
		{
			setg((char*) data, (char*) data, ((char*) data) + size);
		}
	};

	ArrayBuffer m_buffer;
};
}

#endif
