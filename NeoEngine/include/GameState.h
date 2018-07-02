#ifndef NEO_GAMESTATE_H
#define NEO_GAMESTATE_H

#include <memory>

namespace Neo
{

class Platform;
class Window;
class Renderer;
class Game;
class GameState
{
	Game* m_game = nullptr;
public:
	virtual ~GameState() {}
	virtual void begin(Platform& p, Window& w) = 0;
	virtual void end() = 0;
	virtual void update(Platform& p, float dt) = 0;
	virtual void draw(Renderer& r) = 0;
	
	Game* getGame() { return m_game; }
	void setGame(Game* game) { m_game = game; }
};

typedef std::unique_ptr<GameState> GameStateRef;

}

#endif // NEO_GAMESTATE_H
