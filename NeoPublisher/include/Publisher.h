#ifndef __PUBLISHER_H__
#define __PUBLISHER_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <functional>
#include <iostream>

namespace Neo
{

class Publisher
{
public:
	void publish(	const std::string& projectDirectory,
					const std::function<void(const std::string&)>& progressCb = [](const std::string& str)
					{
						std::cout << str << std::endl;
					});

	void loadConfig(const std::string& filename);
	bool isExcluded(const std::string& str) const;

private:
	std::unordered_map<std::string, std::string> m_includedFiles; ///< Maps src to dest
	std::vector<std::regex> m_excludedFiles;
};

}

#endif
