#ifndef NEO_LEVELGAMESTATE_H
#define NEO_LEVELGAMESTATE_H

#include <GameState.h>
#include <Level.h>

namespace Neo 
{

class LevelGameState : public Neo::GameState
{
	Level m_level;
public:
	void draw(Neo::Renderer& r) override;
	void update(Neo::Platform& p, float dt) override;
	void end() override;
	void begin(Neo::Platform& p, Neo::Window& w) override;
	
	Level& getLevel() { return m_level; }
};

}

#endif // NEO_LEVELGAMESTATE_H
