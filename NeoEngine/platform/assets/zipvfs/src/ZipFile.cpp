#include "ZipFile.h"
#include <iostream>
#include <cassert>

using namespace Neo;

bool ZipOpenHook::openPackage()
{
	int err = 0;
	m_zip = zip_open(m_package.c_str(), ZIP_RDONLY, &err);
	if(!m_zip)
	{
		std::cerr << "Could not open ZIP package: " << m_package << std::endl;
		return false;
	}
	
	return true;
}

File* ZipOpenHook::open(const char* path, const char* mode)
{
	if(!m_zip)
		return nullptr;
	
	ZipFile* file = new ZipFile(*this);
	file->open(path, mode);
	return file;
}

void ZipFile::destroy()
{
	close();
	delete this;
}

bool ZipFile::isOpen()
{
	return m_file != nullptr;
}

void ZipFile::rewind()
{
	close();
	open(m_path.c_str(), "");
}

long int ZipFile::tell()
{
	return zip_ftell(m_file);
}

int ZipFile::seek(long int offset, int whence)
{
	return zip_fseek(m_file, offset, whence);
}

int ZipFile::print(const char* format, va_list args)
{
	errno = ENOTSUP;
	return 0;
}

int ZipFile::print(const char* format, ...)
{
	errno = ENOTSUP;
	return 0;
}

size_t ZipFile::write(const void* str, size_t size, size_t count)
{
	errno = ENOTSUP;
	return 0;
}

size_t ZipFile::size()
{
	zip_stat_t st;
	zip_stat(m_zip.getZip(), m_path.c_str(), 0, &st);
	return st.size;
}

size_t ZipFile::read(void* dest, size_t size, size_t count)
{
	return zip_fread(m_file, dest, size*count);
}

int ZipFile::close()
{
	if(!m_file) return 0;
	auto retval = zip_fclose(m_file);
	m_file = nullptr;
	return retval;
}

void ZipFile::open(const char* path, const char* mode)
{
	close();
	m_path = path;
	m_file = zip_fopen(m_zip.getZip(), path, ZIP_FL_ENC_RAW);
}
