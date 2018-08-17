#include "SplashScreen.h"
#include <FileTools.h>
#include <iostream>
#include <TextureLoader.h>
#include <Texture.h>
#include <Window.h>
#include <Game.h>

#ifdef NEO_OPENGL
#include <nanovg.h>
#include <nanovg_gl.h>
extern "C" NVGcontext* nvgCreateGL3(int);
extern "C" void nvgDeleteGL3(NVGcontext*);
#endif

using namespace Neo;
using namespace States;

void SplashScreen::draw(Neo::Renderer& r)
{
	r.clear(0, 0, 0, true);
	
	nvgBeginFrame(m_nv, m_width, m_height, 1.0f);
	nvgSave(m_nv);
	
	nvgFillColor(m_nv, nvgRGB(0, 0, 0));
	nvgBeginPath(m_nv);
	nvgRect(m_nv, 0, 0, m_width, m_height);
	nvgFill(m_nv);
	
	int currentTex = m_slideshowTextures[m_currentSlide];
	int w, h;
	nvgImageSize(m_nv, currentTex, &w, &h);

	auto paint = nvgImagePattern(m_nv, (float) w/2 + m_width/2, (float) h/2 + m_height/2, w, h, 0, currentTex, m_alpha);
	
	nvgBeginPath(m_nv);
	
	nvgTranslate(m_nv, 0, 0);
	nvgRect(m_nv, 0, 0, w, h);

	nvgFillPaint(m_nv, paint);
	nvgFill(m_nv);
	
	nvgRestore(m_nv);
	nvgEndFrame(m_nv);
	
	r.swapBuffers();
}

void SplashScreen::update(Neo::Platform& p, float dt)
{
	switch(m_state)
	{
		case FADE_IN:
			m_alpha += 1.0f/m_transitionDelay * dt; // FIXME dt!
			if(m_alpha >= 1.0)
			{
				m_state = HOLD;
				m_holdTimer = 0.0f;
				m_alpha = 1.0f;
			}
		break;
		
		case HOLD:
			m_holdTimer += dt;
			if(m_holdTimer >= m_delay)
				m_state = FADE_OUT;
		break;
		
		case FADE_OUT:
			m_alpha -= 1.0f/m_transitionDelay * dt; // FIXME dt!
			if(m_alpha <= 0.0f)
			{
				m_alpha = 0.0f;
				m_state = NEXT_SLIDE;
			}
		break;
		
		case NEXT_SLIDE:
			if(m_currentSlide + 1 >= m_slideshow.size())
				getGame()->changeState(std::move(m_nextState));
			else
				m_currentSlide++;
			
			m_state = FADE_IN;
		break;
	};
}

void SplashScreen::end()
{
	for(auto k : m_slideshowTextures)
		nvgDeleteImage(m_nv, k);
	
#ifdef NEO_OPENGL
	nvgDeleteGL3(m_nv);
#else // BGFX
	nvgDelete(m_nv);
#endif
}

void SplashScreen::begin(Neo::Platform& p, Neo::Window& w)
{
#ifdef NEO_OPENGL
	m_nv = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#else // BGFX
	m_nv = nvgCreate(0, 2);
#endif

	nvgSave(m_nv);
	for(auto& k : m_slideshow)
	{
		Neo::Texture image;
		if(!Neo::TextureLoader::load(image, k.c_str()))
		{
			std::cerr << "Could not load image: " << k << std::endl;
			continue;
		}
		
		int texture = nvgCreateImageRGBA(m_nv, image.getWidth(), image.getHeight(), 0, (const unsigned char*) image.getData());
		if(texture == -1)
		{
			std::cerr << "Could not load image: " << k << std::endl;
			continue;
		}
		m_slideshowTextures.push_back(texture);
	}
	nvgRestore(m_nv);
	
	m_currentSlide = 0;
	m_width = w.getWidth();
	m_height = w.getHeight();
}
