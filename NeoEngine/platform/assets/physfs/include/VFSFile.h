#ifndef NEO_VFSFILE_H
#define NEO_VFSFILE_H

#include <FileTools.h>
#include <File.h>
#include <Array.h>

#include <physfs.h>

#include <string>

namespace Neo
{

class VFSOpenHook;
class VFSFile : public File
{
	std::string m_path;
	PHYSFS_file* m_file = nullptr;
	
public:
	VFSFile() {}
	
	void destroy() override;
	bool isOpen() override;
	void rewind() override;
	long int tell() override;
	int seek(long int offset, int whence) override;
	int print(const char* format, va_list args) override;
	int print(const char* format, ...) override;
	size_t write(const void* str, size_t size, size_t count) override;
	size_t read(void* dest, size_t size, size_t count) override;
	int close() override;
	void open(const char* path, const char* mode) override;
	size_t size() override;

};

class VFSOpenHook : public FileOpenHook
{
public:
	VFSOpenHook();
	~VFSOpenHook();
	File* open(const char * path, const char * mode) override;
	
	static FileOpenHook* mount(const char* pkg, const char* arg0);
	static FileOpenHook* mount(const char* arg0)
	{
		return mount("data.neo", arg0);
	}
};

}

#endif // NEO_ZIPFILE_H
