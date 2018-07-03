#include "VFSFile.h"
#include <iostream>
#include <cassert>

using namespace Neo;

VFSOpenHook::VFSOpenHook() {}
VFSOpenHook::~VFSOpenHook() 
{
	PHYSFS_deinit();
}

FileOpenHook* VFSOpenHook::mount(const char* pkg, const char* arg0)
{
	PHYSFS_init(arg0);
	if(!PHYSFS_mount(pkg, "/", 1))
	{
		std::cerr << "Could not mount package: " << pkg << std::endl;
		return nullptr;
	}
	
	VFSOpenHook* hook = new VFSOpenHook();
	M_registerFileOpenHook(hook);
	return hook;
}

File* VFSOpenHook::open(const char* path, const char* mode)
{
	VFSFile* file = new VFSFile();
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
	return PHYSFS_tell(m_file);
}

int VFSFile::seek(long int offset, int whence)
{
	switch(whence)
	{
		case SEEK_SET: return PHYSFS_seek(m_file, offset);
		case SEEK_CUR: return PHYSFS_seek(m_file, tell() + offset);
		case SEEK_END: return PHYSFS_seek(m_file, offset + size());
	}
	
	return -1;
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
	return PHYSFS_fileLength(m_file);
}

size_t VFSFile::read(void* dest, size_t size, size_t count)
{
	return PHYSFS_readBytes(m_file, dest, size*count);
}

int VFSFile::close()
{
	if(!m_file)
		return 0;
	
	int retval = PHYSFS_close(m_file);
	m_file = nullptr;
	return retval;
}

void VFSFile::open(const char* path, const char* mode)
{
	close();
	m_path = path;
	
	m_file = PHYSFS_openRead(path);
}
