#ifndef NEO_VFSFILE_H
#define NEO_VFSFILE_H

#include <FileTools.h>
#include <File.h>
#include <zip.h>

#include <string>

namespace Neo
{

class VFSOpenHook;
class VFSFile : public File
{
	VFSOpenHook& m_zip;
	zip_file_t* m_file = nullptr;
	std::string m_path;
	
public:
	VFSFile(VFSOpenHook& hook):
		m_zip(hook) {}
	
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
	size_t size();

};

class VFSOpenHook : public FileOpenHook
{
	zip* m_zip = nullptr;
	std::string m_package;
	
	bool openPackage();
public:
	VFSOpenHook() = default;
	VFSOpenHook(const char* name) : m_package(name) {}
	File* open(const char * path, const char * mode) override;
	
	zip* getZip() { return m_zip; }
	
	static FileOpenHook* mount(const char* pkg)
	{
		VFSOpenHook* hook = new VFSOpenHook(pkg);
		if(!hook->openPackage())
		{
			delete hook;
			return nullptr;
		}
			
		M_registerFileOpenHook(hook);
		return hook;
	}
	
	static FileOpenHook* mount()
	{
		return mount("data.neo");
	}
};

}

#endif // NEO_ZIPFILE_H
