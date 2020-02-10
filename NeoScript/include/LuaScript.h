#ifndef NEO_LUASCRIPT_H
#define NEO_LUASCRIPT_H

#include <string>
#include <lua.hpp>

#include "NeoScript.h"

namespace Neo 
{

class NEO_SCRIPT_EXPORT LuaScript
{
	lua_State* L = nullptr;
public:
	LuaScript();
	~LuaScript();
	
	bool doString(const std::string& source);
	bool doFile(const char* path);
	
	bool hasFunction(const char* name);
	bool operator()(const char* name);
	bool startCallFunction(const char* name);
	bool endCallFunction(int numArgs);
	
	lua_State* getState() { return L; }
};

class Renderer;
class Behavior;
class Level;
class GameState;
class Window;
class Platform;
class Object;
}


// In SWIG unit
extern "C" 
{
int luaopen_Neo(lua_State*);
void pushRenderer(lua_State*, Neo::Renderer*);
void pushBehavior(lua_State*, Neo::Behavior*);
void pushLevel(lua_State*, Neo::Level*);
void pushWindow(lua_State*, Neo::Window*);
void pushPlatform(lua_State*, Neo::Platform*);
void pushObject(lua_State*, Neo::Object*);

void pushGlobalObject(lua_State*, const char*, Neo::Object*);
void pushGlobalGameState(lua_State*, const char*, Neo::GameState*);

}

#endif // NEO_LUASCRIPT_H
