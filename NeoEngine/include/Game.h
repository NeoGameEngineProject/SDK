#ifndef NEO_GAME_H
#define NEO_GAME_H

#include <GameState.h>
#include <Window.h>
#include <Platform.h>
#include <string>

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
	
	unsigned int m_initialWidth, m_initialHeight;
	std::string m_title;
	
public:
	Game(Game&& g):
		m_currentGame(std::move(g.m_currentGame)),
		m_requestedGame(std::move(g.m_requestedGame)),
		m_platform(std::move(g.m_platform)),
		m_window(std::move(g.m_window)),
		m_running(g.m_running),
		m_lastFrameTime(g.m_lastFrameTime),
		m_initialWidth(g.m_initialWidth),
		m_initialHeight(g.m_initialHeight),
		m_title(std::move(g.m_title)){}
	
	Game(unsigned int width, unsigned int height, const char* title);
	void changeState(GameStateRef&& game, bool now = false); // now: Should this be done now or in the next frame?
	
	template<typename T>
	void changeState()
	{
		changeState(std::move(std::make_unique<T>()));
	}
	
	void begin();
	void update();
	void draw();
	
	void stop() { m_running = false; }
	int run(int argc, char** argv);
	
	Platform& getPlatform() { return m_platform; }
};

}

#endif // NEO_GAME_H
