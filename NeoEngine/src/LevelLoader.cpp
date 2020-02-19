#include "LevelLoader.h"
#include <Level.h>
#include <Log.h>

using namespace Neo;

std::vector<SceneFile*> LevelLoader::s_loaders;

bool LevelLoader::supportsExtension(const char* ext)
{
	for(auto* f : s_loaders)
	{
		if(f->supportsExtension(ext))
			return true;
	}

	return false;
}

void LevelLoader::registerLoader(SceneFile* ldr)
{
	LOG_DEBUG("Registering loader: " << ldr->getName());
	s_loaders.push_back(ldr);
}

bool LevelLoader::load(Level& level, const char* file, ObjectHandle rootNode)
{
	const auto ext = SceneFile::findExtension(file);
	for(auto* f : s_loaders)
	{
		if(f->supportsExtension(ext) && f->load(level, file, rootNode))
			return true;
	}

	return false;
}

bool LevelLoader::load(Level& level, const char* file, const char* rootNode)
{
	ObjectHandle root;
	if(rootNode)
		root = level.find(rootNode);

	return load(level, file, root);
}


bool LevelLoader::save(Level& level, const char* file, ObjectHandle rootNode)
{
	for(auto* f : s_loaders)
	{
		if(f->supportsExtension(SceneFile::findExtension(file)) && f->save(level, file, rootNode))
			return true;
	}

	return false;
}

bool LevelLoader::save(Level& level, const char* file, const char* rootNode)
{
	ObjectHandle root;
	if(rootNode)
		root = level.find(rootNode);

	return save(level, file, root);
}

std::string SceneFile::findExtension(const std::string& f)
{
	return f.substr(f.find_last_of('.') + 1);
}
