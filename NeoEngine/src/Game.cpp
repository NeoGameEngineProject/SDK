#include "Game.h"

#ifndef NEO_SINGLE_THREAD
#include <ThreadPool.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <VFSFile.h>
#include <iostream>

using namespace Neo;

#ifndef ASSET_MODE
#define ASSET_MODE 0
#endif

#ifdef __EMSCRIPTEN__
void doIteration();
#endif

Game::Game(unsigned int width, unsigned int height, const char* title):
	m_initialWidth(width),
	m_initialHeight(height),
	m_title(title)
{
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(doIteration, 60, 0);
	emscripten_pause_main_loop();
#endif
}

void Game::begin()
{
	m_window = m_platform.createWindow(m_initialWidth, m_initialHeight, m_title.c_str());
	m_running = true;
	
#if !defined(NEO_SINGLE_THREAD) && !defined(__EMSCRIPTEN__)
	if(!ThreadPool::threadCount())
		ThreadPool::start();
#endif
}

void Game::stop()
{ 
	// We don't need to stop threads since they auto cleanup on application exit
	m_running = false;
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
	
	if(m_currentGame == nullptr)
		return;
	
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
	
	m_platform.getInputContext().handleInput();
	m_currentGame->update(m_platform, delta);
}

void Game::draw()
{
	m_window->activateRendering();
	m_currentGame->draw(*m_window->getRenderer());
	m_window->swapBuffers();
}

#ifndef __EMSCRIPTEN__
int Game::run(int argc, char** argv)
{
	// Set loading mode
#if ASSET_MODE != 0
	const char* pkg = (ASSET_MODE == 1 ? argv[0] : "data.neo");
	if(!Neo::VFSOpenHook::mount(pkg, argv[0]))
	{
		std::cerr << "Could not open assets file!" << std::endl;
		return 1;
	}
#endif
	
	begin();
	while(m_running && (m_currentGame || m_requestedGame))
	{
		update();
		draw();
	}

	return 0;
}
#else
static std::shared_ptr<Neo::Game> game;
const char* arg0 = nullptr;
void doIteration()
{
	assert(game && "I need a game to run!!!");
	
	game->update();
	game->draw();
}

int Game::run(int argc, char** argv)
{
	arg0 = argv[0];
	
	// We need to move the game there since this will immediately return 0
	// since web browsers have an event loop that blocks if you don't.
	// We still need the date, so move it to a pointer that is sure to survive,
	// no matter what the caller did with this object.
	game = std::make_shared<Neo::Game>(std::move(*this));

	std::string fullpath = getenv("PWD");
	emscripten_async_wget((fullpath + "data.neo").c_str(), "data.neo",
		[](const char* s){
			std::cout << "Done downloading " << s << std::endl;
			if(!Neo::VFSOpenHook::mount("data.neo", arg0))
			{
				std::cerr << "Could not open assets file!" << std::endl;
				return;
			}

			// Now create the window. We should have all data now!
			game->begin();
			emscripten_resume_main_loop();
		},

		[](const char* s){
			std::cerr << "Error: Could not download " << s << std::endl;
			exit(1);
		});
	
	return 0;
}
#endif
