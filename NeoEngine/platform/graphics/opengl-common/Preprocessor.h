#ifndef __PREPROCESSOR_H__
#define __PREPROCESSOR_H__

#include <string>
#include <vector>

namespace Neo
{

class Preprocessor
{
public:
	std::string process(const std::string& input);
	std::string processFile(const std::string& path);

	void addSearchPath(const std::string& str) { m_searchPath.push_back(str); }
	void addDefine(const std::string& str) { m_defines.push_back(str); }
	void removeDefine(const std::string& str);

private:
	std::vector<std::string> m_searchPath;
	std::vector<std::string> m_defines;
};

}

#endif
