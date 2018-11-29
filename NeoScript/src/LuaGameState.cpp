#include "LuaGameState.h"
#include <Game.h>

using namespace Neo;

LuaGameState::LuaGameState(const char* file)
{
	load(file);
}

void LuaGameState::draw(Neo::Renderer& r)
{
	lua_State* L = m_script.getState();
	if(!m_script.startCallFunction("onDraw")) return;
	pushRenderer(L, &r);
	m_script.endCallFunction(1);
}

void LuaGameState::update(Neo::Platform& p, float dt)
{
	lua_State* L = m_script.getState();
	if(!m_script.startCallFunction("onUpdate")) return;
	pushPlatform(L, &p);
	lua_pushnumber(L, dt);
	m_script.endCallFunction(2); 
}

void LuaGameState::end()
{
	m_script("onEnd");
}

void LuaGameState::begin(Neo::Platform& p, Neo::Window& w)
{
	lua_State* L = m_script.getState();
	pushGlobalGameState(L, "GameState", this);
	if(!m_script.startCallFunction("onBegin")) return;
	pushPlatform(L, &p);
	pushWindow(L, &w);
	m_script.endCallFunction(2);
}

void LuaGameState::load(const char* file)
{
	m_file = file;
	m_script.doFile(file);
}

void LuaGameState::reload()
{
	end();
	m_script.doFile(m_file.c_str());
	begin(getGame()->getPlatform(), *getGame()->getWindow());
}
