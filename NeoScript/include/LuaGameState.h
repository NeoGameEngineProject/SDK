#ifndef NEO_LUAGAMESTATE_H
#define NEO_LUAGAMESTATE_H

#include <GameState.h>
#include <LuaScript.h>

namespace Neo 
{

class LuaGameState : public Neo::GameState
{
	LuaScript m_script;
	std::string m_file;
	
public:
	LuaGameState(const char* file);
	LuaGameState() = default;
	~LuaGameState() = default;
	
	void draw(Neo::Renderer& r) override;
	void update(Neo::Platform& p, float dt) override;
	void end() override;
	void begin(Neo::Platform& p, Neo::Window& w) override;
	
	void load(const char* file);
	void reload();
};

}

#endif // NEO_LUAGAMESTATE_H
