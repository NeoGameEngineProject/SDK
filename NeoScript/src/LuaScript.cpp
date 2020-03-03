#include "LuaScript.h"
#include <Log.h>
#include <StringTools.h>

using namespace Neo;

LuaScript::LuaScript()
{
	L = luaL_newstate();

	// LOG_DEBUG("LuaScript " << this << " " << L);
	if(!L) return; // Error checking?

	luaL_openlibs(L);
	luaopen_Neo(L);
}

LuaScript::~LuaScript()
{
	// LOG_DEBUG("~LuaScript " << this << " " << L);
	if(L)
		lua_close(L);
}

std::string LuaScript::getGlobalString(const char* name)
{
	lua_getglobal(L, name);

	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return "";
	}

	std::string str = lua_tostring(L, -1);
	lua_pop(L, 1);
	
	return str;
}

bool LuaScript::hasFunction(const char* name)
{
	lua_getglobal(L, name);
	if(lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return true;
	}
	
	lua_pop(L, 1);
	return false;
}

bool LuaScript::startCallFunction(const char* name)
{
	lua_getglobal(L, name);
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return false;
	}
	return true;
}

bool LuaScript::endCallFunction(int numArgs)
{
	if(lua_pcall(L, numArgs, 0, 0) != 0)
	{
		LOG_ERROR("Lua Script: \n" << lua_tostring(L, -1));
		return false;
	}
	return true;
}

bool LuaScript::operator()(const char* name)
{
	if(startCallFunction(name))
		return endCallFunction(0);
	
	return false;
}

bool LuaScript::doString(const std::string& source)
{
	return luaL_dostring(L, source.c_str()) == 0;
}

bool LuaScript::doFile(const char* path)
{
	// TODO Use a binary version in case the module is pre-compiled!
	char* source = readTextFile(path);
	if(!source)
		return false;
	
	if(luaL_loadbuffer(L, source, strlen(source), path) || lua_pcall(L, 0, LUA_MULTRET, 0)) 
	{
		LOG_ERROR("Lua Script: " << lua_tostring(L, -1));
		free(source);
		return false;
	}
		
	free(source);
	return true;
}
