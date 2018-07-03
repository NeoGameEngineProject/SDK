#include "VFSFile.h"
#include <iostream>
#include <cassert>

using namespace Neo;

bool VFSOpenHook::openPackage()
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

File* VFSOpenHook::open(const char* path, const char* mode)
{
	if(!m_zip)
		return nullptr;
	
	VFSFile* file = new VFSFile(*this);
	file->open(path, mode);
	return file;
}

void VFSFile::destroy()
{
	close();
	delete this;
}

bool VFSFile::isOpen()
{
	return m_file != nullptr;
}

void VFSFile::rewind()
{
	close();
	open(m_path.c_str(), "");
}

long int VFSFile::tell()
{
	return zip_ftell(m_file);
}

int VFSFile::seek(long int offset, int whence)
{
	return zip_fseek(m_file, offset, whence);
}

int VFSFile::print(const char* format, va_list args)
{
	errno = ENOTSUP;
	return 0;
}

int VFSFile::print(const char* format, ...)
{
	errno = ENOTSUP;
	return 0;
}

size_t VFSFile::write(const void* str, size_t size, size_t count)
{
	errno = ENOTSUP;
	return 0;
}

size_t VFSFile::size()
{
	zip_stat_t st;
	zip_stat(m_zip.getZip(), m_path.c_str(), 0, &st);
	return st.size;
}

size_t VFSFile::read(void* dest, size_t size, size_t count)
{
	return zip_fread(m_file, dest, size*count);
}

int VFSFile::close()
{
	if(!m_file) return 0;
	auto retval = zip_fclose(m_file);
	m_file = nullptr;
	return retval;
}

void VFSFile::open(const char* path, const char* mode)
{
	close();
	m_path = path;
	m_file = zip_fopen(m_zip.getZip(), path, ZIP_FL_ENC_RAW);
}
