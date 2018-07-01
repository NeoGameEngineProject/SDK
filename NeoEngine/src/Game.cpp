#include "Game.h"

using namespace Neo;

Game::Game(unsigned int width, unsigned int height, const char* title)
{
	m_window = m_platform.createWindow(width, height, title);
}

void Game::changeState(GameStateRef&& game)
{
	if(m_currentGame != nullptr)
		m_currentGame->end();
	
	// TODO Render loading screen!
	m_currentGame = std::move(game);
	m_currentGame->setGame(this);
	m_currentGame->begin(m_platform, *m_window);
}

void Game::update()
{
	if(m_currentGame != nullptr)
		m_currentGame->update(m_platform, 0.0f);
}

void Game::draw()
{
	if(m_currentGame != nullptr)
		m_currentGame->draw(*m_window->getRenderer());
}

int Game::run()
{
	m_running = true;
	while(m_running && m_currentGame)
	{
		m_currentGame->update(m_platform, 0.0f);
		m_currentGame->draw(*m_window->getRenderer());
	}
	return 0;
}
