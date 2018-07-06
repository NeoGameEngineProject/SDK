//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2018 Yannick Pflanzer <www.neo-engine.de>
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

#include "SoundLoader.h"
#include <Sound.h>
#include <FileTools.h>
#include <iostream>
#include <cstring>

using namespace Neo;

#include "stb_vorbis.hpp"

bool loadVorbisSound(const char * filename, Sound* sound)
{
	unsigned int size = 0;
	void* file = readBinaryFile(filename, &size);
	
	if(!file)
		return false;
	
	int channels, sample_rate;
	short* output;
	
	size_t samples = stb_vorbis_decode_memory((const unsigned char*) file, size, &channels, &sample_rate, &output);
	free(file);
	file = nullptr;
	
	SOUND_FORMAT format;
	if(channels == 2) format = SOUND_FORMAT_STEREO16;
	else if(channels == 1) format = SOUND_FORMAT_MONO16;
	else 
	{
		std::cerr << "Error: Could not load sound: Invalid channel count (" << channels << ")" << std::endl;
		free(output);
		return false;
	}
	
	std::cout << "Loaded vorbis: channels=" << channels << " sample_rate=" << sample_rate << " samples=" << samples << std::endl;
	
	// FIXME: We need to copy. Allow moving the pointer in!
	sound->create(filename, SOUND_FORMAT_STEREO16, channels * samples * 2, sample_rate);
	memcpy(sound->getData(), output, sound->getSize());
	
	free(output);
	return true;
}

bool loadWAVSound(const char * filename, Sound* sound)
{
	File * file = M_fopen(filename, "rb");
	if(!file)
	{
		printf("ERROR Load Sound : unable to read %s file\n", filename);
		return false;
	}
	
	// read header
	char header[4];
	M_fread(header, sizeof(char), 4, file);
	
	// RIFF - WAV
	if(strncmp(header, "RIFF", 4) == 0)
	{
		unsigned int size;
		M_fread(&size, sizeof(int), 1, file);
		M_fread(header, sizeof(char), 4, file);
		
		if(strncmp(header, "WAVE", 4) == 0)
		{
			short format_tag, channels, block_align, bits_per_sample;
			unsigned int format_length, sample_rate, avg_bytes_sec, data_size;
			
			M_fread(header, sizeof(char), 4, file); // "fmt ";
			M_fread(&format_length, sizeof(int),1,file);
			M_fread(&format_tag, sizeof(short), 1, file);
			M_fread(&channels, sizeof(short),1,file);
			M_fread(&sample_rate, sizeof(int), 1, file);
			M_fread(&avg_bytes_sec, sizeof(int), 1, file);
			M_fread(&block_align, sizeof(short), 1, file);
			M_fread(&bits_per_sample, sizeof(short), 1, file);
			M_fread(header, sizeof(char), 4, file); // "data"
			M_fread(&data_size, sizeof(int), 1, file);
			
			if((!(bits_per_sample == 8 || bits_per_sample == 16)) || (!(channels > 0 && channels <= 2)) || (format_tag != 1))
			{
				std::cerr << "Load Sound: Unsupported RIFF file (bits_per_sample=" << bits_per_sample 
						<< ", channels=" << channels 
						<< ", format_tag=" << format_tag << ")" << std::endl;
				M_fclose(file);
				return false;
			}
			
			SOUND_FORMAT format;
			switch(bits_per_sample)
			{
				case 8:
				{
					switch(channels)
					{
						case 1:
							format = SOUND_FORMAT_MONO8;
							break;
						case 2:
							format = SOUND_FORMAT_STEREO8;
							break;
					}
					break;
				}
				case 16:
				{
					switch(channels)
					{
						case 1:
							format = SOUND_FORMAT_MONO16;
							break;
						case 2:
							format = SOUND_FORMAT_STEREO16;
							break;
					}
					break;
				}
			}
			
			// create sound
			sound->create(filename, format, data_size, sample_rate);
			
			// read sound data
			M_fread((char*) sound->getData(), sizeof(char), data_size, file);
			
			M_fclose(file);
			return true;
		}
		else
		{
			std::cerr << "Load Sound: Unsupported RIFF file" << std::endl;
		}
	}
	
	M_fclose(file);
	return false;
}

bool SoundLoader::load(Sound& sound, const char* filename)
{
	return (loadWAVSound(filename, &sound) || loadVorbisSound(filename, &sound));
}
