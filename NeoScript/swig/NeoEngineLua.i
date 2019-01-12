%module Neo

%include "NeoEngine.i"

%{
extern "C"
{

#define MAKE_PUSHER(NAME) void push##NAME(lua_State* L, NAME* ptr) { SWIG_NewPointerObj(L, ptr, SWIGTYPE_p_Neo__##NAME, 0); }

using namespace Neo;
MAKE_PUSHER(Renderer)
MAKE_PUSHER(Behavior)
MAKE_PUSHER(Level)
MAKE_PUSHER(Window)
MAKE_PUSHER(Platform)
MAKE_PUSHER(Object)

void pushGlobalGameState(lua_State* L, const char* name, Neo::GameState* state)
{
	SWIG_NewPointerObj(L, state, SWIGTYPE_p_Neo__GameState, 0);
	lua_setglobal(L, name);
}

void pushGlobalObject(lua_State* L, const char* name, Neo::Object* state)
{
	SWIG_NewPointerObj(L, state, SWIGTYPE_p_Neo__Object, 0);
	lua_setglobal(L, name);
}

}
%}
