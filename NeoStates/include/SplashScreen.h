#ifndef NEO_STATES_SPLASHSCREEN_H
#define NEO_STATES_SPLASHSCREEN_H

#include <GameState.h>
#include <nanovg.h>
#include <vector>
#include <string>

namespace Neo
{
namespace States 
{

class SplashScreen : public Neo::GameState
{
	enum States
	{
		FADE_IN,
		FADE_OUT,
		HOLD,
		NEXT_SLIDE,
		END
	};
	
	NVGcontext* m_nv = nullptr;
	std::vector<std::string> m_slideshow;
	std::vector<int> m_slideshowTextures;
	
	unsigned int m_transitionDelay = 2;
	unsigned int m_delay = 0;
	GameStateRef m_nextState;
	
	unsigned int m_currentSlide = 0;
	unsigned int m_width, m_height;
	
	float m_alpha = 0.0f;
	unsigned int m_state = FADE_IN;
	float m_holdTimer = 0;
	
public:
	SplashScreen(GameStateRef&& nextState, unsigned int delay, unsigned int transitionDelay, std::initializer_list<std::string>&& slides):
		m_slideshow(std::move(slides)), m_delay(delay), m_transitionDelay(transitionDelay), m_nextState(std::move(nextState)) {}
	
	void draw(Neo::Renderer& r) override;
	void update(Neo::Platform& p, float dt) override;
	void end() override;
	void begin(Neo::Platform& p, Neo::Window& w) override;

};

}
}

#endif // NEO_STATES_SPLASHSCREEN_H
