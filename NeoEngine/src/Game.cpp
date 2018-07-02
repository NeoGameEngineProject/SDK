#include "Game.h"

using namespace Neo;

Game::Game(unsigned int width, unsigned int height, const char* title)
{
	m_window = m_platform.createWindow(width, height, title);
}

void Game::changeState(GameStateRef&& game, bool now)
{
	if(!now)
	{
		m_requestedGame = std::move(game);
		return;
	}
	
	if(m_currentGame != nullptr)
		m_currentGame->end();
	
	// TODO Render loading screen!
	m_currentGame = std::move(game);
	m_currentGame->setGame(this);
	m_currentGame->begin(m_platform, *m_window);
	
	m_lastFrameTime = m_platform.getTime() / 1000.0;
}

void Game::update()
{
	if(m_requestedGame.get())
	{
		changeState(std::move(m_requestedGame), true);
	}
	
	double t0 = (double) m_platform.getTime() / 1000.0;
	float delta = (t0 - m_lastFrameTime);
	m_lastFrameTime = t0;
	
	m_currentGame->update(m_platform, delta);
}

void Game::draw()
{
	m_currentGame->draw(*m_window->getRenderer());
}

int Game::run()
{
	m_running = true;
	while(m_running && m_currentGame || m_requestedGame)
	{
		update();
		draw();
	}
	return 0;
}
