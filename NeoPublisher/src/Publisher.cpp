#include <Publisher.h>
#include <filesystem>
#include <zip.h>

#include <LevelLoader.h>

#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_HAS_CXX11_RANGE_FOR 1
#include <rapidjson/document.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace Neo;
namespace fs = std::filesystem;

static void addDirectoryToZip(zip_t* zip, Publisher& pub, const std::string& dir, const std::string& tgtDir, const std::function<void(const std::string&)>& cb)
{
	for(auto& p: fs::recursive_directory_iterator(dir))
	{
		if(p.is_directory() || pub.isExcluded(p.path().string()))
			continue;

		auto fullpath = p.path().string();
		auto path = tgtDir + fullpath.substr(dir.size());

		if(path[0] == '.')
			path = path.substr(1);

		cb(fullpath + " -> " + path);

		{
			auto* src = zip_source_file(zip, fullpath.c_str(), 0, -1);
			if(!src)
			{
				throw std::runtime_error("Could not read file: " + fullpath + ": " + zip_error_strerror(zip_get_error(zip)));
			}

			if(zip_file_add(zip, path.c_str(), src, ZIP_FL_ENC_UTF_8) == -1)
			{
				zip_source_close(src);
				throw std::runtime_error("Could not write file: " + path + ": " + zip_error_strerror(zip_get_error(zip)));
			}

			zip_source_close(src);
		}
	}
}

void Publisher::loadConfig(const std::string& filename)
{
	rapidjson::Document doc;
	
	{
		std::stringstream config;
		std::ifstream in(filename);

		if(!in)
			throw std::runtime_error("Could not open config file!");

		config << in.rdbuf();
		doc.Parse(config.str().c_str());
	}

	for(auto& entry : doc["include"].GetObject())
	{
		m_includedFiles[entry.name.GetString()] = entry.value.GetString();
	}

	for(auto& entry : doc["exclude"].GetArray())
	{
		m_excludedFiles.emplace_back(entry.GetString());
	}
}

bool Publisher::isExcluded(const std::string& str) const
{
	std::smatch match;
	for(const auto& r : m_excludedFiles)
	{
		if(std::regex_search(str, match, r))
			return true;
	}

	return false;
}

void Publisher::publish(const std::string& projectDirectory, const std::function<void(const std::string&)>& progressCb)
{
	auto outzip = projectDirectory + "/assets.zip";
	int errorp = 1;

	progressCb("Publishing " + projectDirectory + " to file " + outzip);

	auto* zip = zip_open(outzip.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &errorp);

	if(!zip)
	{
		zip_error_t ziperror;
		zip_error_init_with_code(&ziperror, errorp);
		throw std::runtime_error("Could not open ZIP at " + outzip + ": " + zip_error_strerror(&ziperror));
	}

	try
	{
		addDirectoryToZip(zip, *this, projectDirectory + "/assets", "/assets", progressCb);
		for(auto& p : m_includedFiles)
		{
			if(p.first[0] != '/')
				addDirectoryToZip(zip, *this, projectDirectory + "/" + p.first, p.second, progressCb);
			else
				addDirectoryToZip(zip, *this, p.first, p.second, progressCb);
		}
	}
	catch(...)
	{
		zip_close(zip);
		std::rethrow_exception(std::current_exception());
	}

	if(zip_close(zip))
	{
		throw std::runtime_error("Could not write file: " + outzip + ": " + zip_error_strerror(zip_get_error(zip)));
	}
}
