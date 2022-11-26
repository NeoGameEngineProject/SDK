#ifndef __PHYS_STREAM_H__
#define __PHYS_STREAM_H__

#include <streambuf>
#include <iostream>
#include <ostream>

#include <physfs.h>

namespace Neo
{


class PhysBuffer : public std::basic_streambuf<char, std::char_traits<char>>
{
	typedef std::char_traits<char> Traits;

	static const size_t BUF_SIZE = 512;
	char m_buffer[BUF_SIZE];

	PHYSFS_File* m_file = nullptr;

public:
	PhysBuffer()
	{
		char* end = m_buffer + BUF_SIZE;
		setg(end, end, end);
		setp(m_buffer, end);
	}

	~PhysBuffer() override
	{
		sync();

		if(m_file)
			PHYSFS_close(m_file);
	}

	void setFile(PHYSFS_File* file)
	{
		if(m_file)
		{
			sync();
			PHYSFS_close(m_file);
		}

		m_file = file;
	}

protected:
	
	int_type underflow()
	{
		if(PHYSFS_eof(m_file))
			return traits_type::eof();

		auto bytesRead = PHYSFS_readBytes(m_file, m_buffer, BUF_SIZE);
		if (bytesRead < 0)
			return traits_type::eof();

		setg(m_buffer, m_buffer, m_buffer + bytesRead);
		return (unsigned char) *gptr();
	}

	pos_type seekoff(off_type pos, std::ios_base::seekdir dir, std::ios_base::openmode mode)
	{
		switch (dir)
		{
		case std::ios_base::beg:
			PHYSFS_seek(m_file, pos);
			break;
		case std::ios_base::cur:
			PHYSFS_seek(m_file, (PHYSFS_tell(m_file) + pos) - (egptr() - gptr()));
			break;
		case std::ios_base::end:
			PHYSFS_seek(m_file, PHYSFS_fileLength(m_file) + pos);
			break;
		}

		if(mode & std::ios_base::in)
			setg(egptr(), egptr(), egptr());

		if(mode & std::ios_base::out)
			setp(m_buffer, m_buffer);

		return PHYSFS_tell(m_file);
	}

	pos_type seekpos(pos_type pos, std::ios_base::openmode mode)
	{
		PHYSFS_seek(m_file, pos);
		
		if (mode & std::ios_base::in)
			setg(egptr(), egptr(), egptr());
		
		if (mode & std::ios_base::out)
			setp(m_buffer, m_buffer);

		return PHYSFS_tell(m_file);
	}

	int_type overflow(int_type c = traits_type::eof())
	{
		if(pptr() == pbase() && c == traits_type::eof())
			return 0;

		if(PHYSFS_writeBytes(m_file, pbase(), pptr() - pbase()) < 1)
			return traits_type::eof();

		if (c != traits_type::eof())
		{
			if(PHYSFS_writeBytes(m_file, &c, 1) < 1)
				return traits_type::eof();
		}

		setp(m_buffer, m_buffer + BUF_SIZE);
		return 0;
	}

	int sync()
	{
		return overflow();
	}
};

class PhysStream : public std::iostream
{
public:
	PhysStream():
		std::iostream(&m_buffer)
	{
		set_rdbuf(&m_buffer);
	}

	PhysStream(const char* file, std::ios_base::openmode mode = std::ios_base::in):
		std::iostream(&m_buffer)
	{
		open(file, mode);
		set_rdbuf(&m_buffer);
	}

	PhysStream(PHYSFS_File* file):
		std::iostream(&m_buffer)
	{
		m_buffer.setFile(file);
		set_rdbuf(&m_buffer);
	}

	void close()
	{
		m_buffer.setFile(nullptr);
	}

	void open(const char* file, std::ios_base::openmode mode = std::ios_base::in)
	{
		PHYSFS_file* f = nullptr;

		switch(mode)
		{
			case app: f = PHYSFS_openAppend(file); break;
			case out: f = PHYSFS_openWrite(file); break;
			case in: f = PHYSFS_openRead(file); break;
			default: break;
		}

		if(!f)
			clear(rdstate() & ~goodbit);
		else
			clear();

		m_buffer.setFile(f);
	}



private:
	PhysBuffer m_buffer;
};


}

#endif
