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

#include "Utils.h"
#include "Sound.h"

using namespace Neo;

Sound::Sound(void):
m_data(NULL),
m_format(SOUND_FORMAT_MONO8),
m_sampleRate(0),
m_size(0)
{}

Sound::~Sound(void)
{
    SAFE_FREE(m_data);
}

void Sound::create(SOUND_FORMAT format, unsigned int size, unsigned int sampleRate)
{
	m_size = size;
	m_format = format;
	m_sampleRate = sampleRate;
    SAFE_FREE(m_data);

	m_data = malloc(m_size);
}
