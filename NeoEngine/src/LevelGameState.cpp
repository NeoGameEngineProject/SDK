#include "LevelGameState.h"

using namespace Neo;

void LevelGameState::draw(Neo::Renderer& r)
{
	m_level.draw(r);
}

void LevelGameState::update(Neo::Platform& p, float dt)
{
	m_level.update(p, dt);
}

void LevelGameState::end()
{
	m_level.end();
}

void LevelGameState::begin(Neo::Platform& p, Neo::Window& w)
{
	m_level.begin(p, *w.getRenderer());
}
