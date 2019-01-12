#include "LuaBehavior.h"
#include <StringTools.h>
#include <Log.h>

using namespace Neo;

REGISTER_BEHAVIOR(LuaBehavior)

#define PARENT_GLOBNAME "parent"

LuaBehavior::LuaBehavior(const char* file)
{
	fileName = file;
	m_script.doFile(file);
	
	m_hasUpdate = m_script.hasFunction("onUpdate");
	m_hasDraw = m_script.hasFunction("onDraw");
	
	std::string name = file;
	name = name.substr(name.find_last_of('/') + 1);
	
	auto idx = name.find_last_of('.');
	if(idx != std::string::npos)
		name.erase(idx);
	
	this->name = name;
}

const char* LuaBehavior::getName() const
{
	return (name.getLength() ? name.str() : "LuaBehavior");
}

Neo::Behavior* LuaBehavior::getNew() const
{
	return new LuaBehavior(*this);
}

std::unique_ptr<Neo::Behavior> LuaBehavior::clone() const
{
	return std::unique_ptr<Neo::Behavior>(getNew());
}

void LuaBehavior::begin(Neo::Platform& p, Neo::Renderer& render, Neo::Level& level)
{
	if(m_script.hasFunction("onBegin"))
	{
		m_script.startCallFunction("onBegin");
		pushObject(m_script.getState(), getParent());
		pushPlatform(m_script.getState(), &p);
		pushRenderer(m_script.getState(), &render);
		pushLevel(m_script.getState(), &level);
		m_script.endCallFunction(4);
	}
}

void LuaBehavior::update(Neo::Platform& p, float dt)
{
	if(m_hasUpdate)
	{
		// We need to do that since the pointer might change...
		m_script.startCallFunction("onUpdate");
		pushObject(m_script.getState(), getParent());
		pushPlatform(m_script.getState(), &p);
		lua_pushnumber(m_script.getState(), dt);
		m_script.endCallFunction(3);
		
		lua_gc(m_script.getState(), LUA_GCCOLLECT, 0);
	}
}

void LuaBehavior::draw(Neo::Renderer& render)
{
	if(m_hasDraw)
	{
		m_script.startCallFunction("onDraw");
		pushObject(m_script.getState(), getParent());
		pushRenderer(m_script.getState(), &render);
		m_script.endCallFunction(2);
	}

}

void LuaBehavior::end()
{
	if(m_script.hasFunction("onEnd"))
		m_script("onEnd");
}

void LuaBehavior::serialize(std::ostream& out)
{
	lua_State* L = m_script.getState();
	lua_pushnil(L);
	
	while (lua_next(L, LUA_GLOBALSINDEX) != 0) 
	{
		// If the key is not a string
		if(lua_type(L, -2) != LUA_TSTRING) 
		{
			lua_pop(L, 1);
			continue;
		}

		const int type = lua_type(L, -1);
		
		// Only trivially serializable types are supported
		if (type != LUA_TNUMBER &&
			type != LUA_TBOOLEAN &&
			type != LUA_TSTRING) 
		{
			lua_pop(L, 1);
			continue;
		}

		const char* key = lua_tostring(L, -2);
		
		// Ignore private variables
		if (key[0] == '_') 
		{
			lua_pop(L, 1);
			continue;
		}

		const char* value = nullptr;
		
		// If we have a bool: Convert it to string
		if(type == LUA_TBOOLEAN)
		{
			value = (lua_toboolean(L, -1) == 0 ? "false" : "true");
		}
		else // Everything else should be auto-convertible to string (number and strings in this case)
		{
			lua_pushvalue(L, -1);
			value = lua_tostring(L, -1);
			lua_pop(L, 1);
		}

		out << key << " = ";
		if(type == LUA_TSTRING)
			out << "\"" << value << "\"\n";
		else
			out << value << "\n";

		lua_pop(L, 1);
	}
	
	out << '\0';
}

void LuaBehavior::deserialize(Neo::Level&, std::istream& in)
{
	std::string source;
	int c;
	while(in.good() && (c = in.get()) != 0)
	{
		source += c;
	}
	
	m_script.doString(source);
}
