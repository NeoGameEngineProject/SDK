//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef __SOUND_H
#define __SOUND_H

#include <vector>
#include <string>

#include "NeoEngine.h"
#include "Handle.h"
#include "Array.h"

namespace Neo
{
enum SOUND_FORMAT
{
	SOUND_FORMAT_MONO8 = 0,
	SOUND_FORMAT_MONO16,
	SOUND_FORMAT_STEREO8,
	SOUND_FORMAT_STEREO16
};

class NEO_ENGINE_EXPORT Sound
{
	void* m_data;
	SOUND_FORMAT m_format;

	unsigned int m_sampleRate;
	unsigned int m_size;
	std::string m_name;

	handle_t m_buffer = -1;
	
public:
	Sound();
	Sound(Sound&& s) { *this = std::move(s); }
	
	~Sound();

	void create(const char* name, SOUND_FORMAT format, unsigned int size, unsigned int sampleRate);

	void* getData() { return m_data; }
	SOUND_FORMAT getFormat() const { return m_format; }
	unsigned int getSampleRate() const { return m_sampleRate; }
	unsigned int getSize() const { return m_size; }
	const char* getName() const { return m_name.c_str(); }
	
	handle_t getBufferHandle() { return m_buffer; }
	void setBufferHandle(handle_t h) { m_buffer = h; }
	
	void operator=(Sound&& s)
	{
		m_data = s.m_data;
		m_format = s.m_format;
		m_sampleRate = s.m_sampleRate;
		m_size = s.m_size;
		m_name = std::move(s.m_name);
		
		m_buffer = s.m_buffer;
		
		s.m_data = nullptr;
		s.m_size = 0;
		s.m_sampleRate = 0;
		s.m_name = "";
		s.m_buffer = -1;
	}
};

typedef Handle<Sound, std::vector<Sound>> SoundHandle;
typedef Handle<Sound, Array<Sound>> SoundHandleArray;

}
#endif
