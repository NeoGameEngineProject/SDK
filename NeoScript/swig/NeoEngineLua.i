%module Neo

%include "NeoEngine.i"

%{
extern "C"
{

#define MAKE_PUSHER(NAME) void push##NAME(lua_State* L, NAME* ptr) { SWIG_NewPointerObj(L, ptr, SWIGTYPE_p_Neo__##NAME, 1); }

using namespace Neo;
MAKE_PUSHER(Renderer)
MAKE_PUSHER(Behavior)
MAKE_PUSHER(Level)
MAKE_PUSHER(Window)
MAKE_PUSHER(Platform)

// Used to push arguments to Lua
/*
void pushRenderer(lua_State* L, Neo::Renderer* renderer)
{
	SWIG_NewPointerObj(L, renderer, SWIGTYPE_p_Neo__Renderer, 1);
}

void pushBehavior(lua_State* L, Neo::Behavior* behavior)
{
	SWIG_NewPointerObj(L, behavior, SWIGTYPE_p_Neo__Behavior, 1);
}

void pushLevel(lua_State* L, Level* level)
{
	SWIG_NewPointerObj(L, level, SWIGTYPE_p_Neo__Level, 1);
}


void pushWindow(lua_State*, Neo::Window*)
{

}

void pushPlatform(lua_State*, Neo::Platform*)
{

}*/

void pushGlobalGameState(lua_State* L, const char* name, Neo::GameState* state)
{
	SWIG_NewPointerObj(L, state, SWIGTYPE_p_Neo__GameState, 1);
	lua_setglobal(L, name);
}

}
%}
