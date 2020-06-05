#include "Preprocessor.h"

#include <algorithm>
#include <regex>
#include <sstream>

#include <StringTools.h>
#include <Log.h>

using namespace Neo;

void Preprocessor::removeDefine(const std::string& str)
{
	m_defines.erase(std::remove(m_defines.begin(), m_defines.end(), str));
}

std::string Preprocessor::process(const std::string& input)
{

}

std::string Preprocessor::processFile(const std::string& path)
{
	char* file = readTextFile(path.c_str());
	if(!file)
	{
		LOG_ERROR("Could not open file: " << path);
		throw std::runtime_error("Could not open file: " + std::string(path));
	}

	std::stringstream in(file); // TODO Make readTextFile STL aware!
	delete file;
	
	std::string basePath(path);
	basePath.erase(basePath.find_last_of('/') + 1);
	
	std::stringstream out;
	std::string line;
	
	static const std::regex regex("#include(\\s)+(<|\")(.*)(>|\")");
	int linenum = 1;

	for(const auto& d : m_defines)
		out << "#define " << d << "\n";

	while(!in.eof())
	{
		std::smatch result;
		std::getline(in, line, '\n');
				
		std::regex_search(line, result, regex);
		
		if(!result.empty())
		{
			const std::string filepath = basePath + result[3].str();
			std::string file = processFile(filepath.c_str());
			if(file.empty())
			{
				LOG_ERROR("Could not load include file: " << basePath << result[3].str());
				throw std::runtime_error("Could not open file: " + basePath + result[3].str());
			}
			
			// out << "\n#line 1 \"" << filepath << "\"\n";
			out << file;
			// out << "\n#line " << linenum << " \"" << path << "\"\n";
			continue;
		}
		
		out << line << '\n';
		linenum++;
	}
	
	return out.str();
}
