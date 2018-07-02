#ifndef NEO_GAME_H
#define NEO_GAME_H

#include <GameState.h>
#include <Window.h>
#include <Platform.h>

namespace Neo
{

class Game
{
	GameStateRef m_currentGame;
	GameStateRef m_requestedGame;
	
	Platform m_platform;
	std::unique_ptr<Window> m_window;
	
	bool m_running = true;
	double m_lastFrameTime = 0.0;
public:
	Game(unsigned int width, unsigned int height, const char* title);
	void changeState(GameStateRef&& game, bool now = false); // now: Shold this be done now or in the next frame?
	
	template<typename T>
	void changeState()
	{
		changeState(std::move(std::make_unique<T>()));
	}
	
	void update();
	void draw();
	
	void stop() { m_running = false; }
	int run();
};

}

#endif // NEO_GAME_H
