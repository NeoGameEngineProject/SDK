//========================================================================
// Copyright (c) 2012 Skaiware
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

#include <Log.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <mutex>

#ifdef ANDROID
#include <SDL_log.h>
#endif

using namespace Neo;

// static vars
namespace
{
thread_local Log* m_instance = NULL;
std::ostream* out = &std::cout;
std::ostream* err = &std::cerr;

std::mutex logMutex;
}

Log::Log()
{
	// open file stream
	m_logfstream.open("logfile.txt", std::fstream::out|std::fstream::trunc);
	
#if 0
	if(!m_logfstream.good())
	{
	#ifndef WIN32
		std::string path = getenv("HOME");
		path += "/.neoeditor/logfile.txt";

		m_logfstream.open(path.c_str(), std::fstream::out|std::fstream::trunc);
	#else
		std::string path = getenv("APPDATA");
		path += "\\neoeditor\\logfile.txt";

		m_logfstream.open(path.c_str(), std::fstream::out|std::fstream::trunc);
	#endif
		if(! m_logfstream.good())
		{
			fprintf(stderr, "Log: file fstream not good\n");
			return;
		}
	}
#endif
	
	char* mll = getenv("NEO_LOG_LEVEL");
	if(mll)
	{
		m_logfstream<<"Found env var NEO_LOG_LEVEL = "<<mll<<std::endl;
		int desired_loglevel = atoi(mll);
		
		if(desired_loglevel>=0 && desired_loglevel<=7) // MUST be between 0 & 7
			m_desired_loglevel=desired_loglevel;
	}
	
	m_logfstream<<"Log tracked at level :"<< m_desired_loglevel <<std::endl;
}

Log::~Log()
{
	m_logfstream.close();
}

static char severities_strings[][64]={{"[ Emerg ]"}, {"[ Alert ]"}, {"[ Critic ]"}, {"[ Error ]"}, {"[ Warn ]"}, {"[ Notice ]"}, {"[ Info ]"}, {"[ Debug ]"}};

void Log::log(int severity, const char * function, const char * filename, const int & line_no)
{
	if(severity > m_instance->m_desired_loglevel)
		return;

	if(!m_instance)
		m_instance = new Log();

	std::string sev="???";
	if(severity >= 0 && severity <= 7)
			sev = severities_strings[severity];

	// Don't buffer output when it's an error!
	if(severity <= 5)
	{
		logMutex.lock();
		*(err) << sev <<" "<< m_instance->m_string << " in " << (function?function:"?")
		<< " in "<< (filename?filename:"?") // do we add filename in console ?
		<< ":" << line_no
		<< std::endl;
		logMutex.unlock();
	}
	else
	{
		logMutex.lock();
		*(out) << sev <<" "<< m_instance->m_string << " in " << (function?function:"?")
		<< " in "<< (filename?filename:"?") // do we add filename in console ?
		<< ":" << line_no
		<< std::endl;
		logMutex.unlock();
	}

#ifdef ANDROID
	SDL_Log("%s %s in %s\n", sev.c_str(), m_string.c_str(), (function?function:"?"));
#endif

	if(m_instance->m_logfstream.good())
		m_instance->m_logfstream<<sev<<" "<<m_instance->m_string<< " in " << (function?function:"?") << " in "<< (filename?filename:"?") << ":" << line_no <<std::endl;
}

Log* Log::get()
{
	if(!m_instance)
		m_instance = new Log();

	return m_instance;
}

void Log::setOutStream(std::ostream& stream)
{
	out = &stream;
}

void Log::setErrStream(std::ostream& stream)
{
	err = &stream;
}

void Neo::infoLog(const char* s, const char* function, const char* filename, int line)
{
	m_instance->m_stringstream.str(std::string(""));
	m_instance->m_stringstream << s;
	m_instance->m_string = m_instance->m_stringstream.str();
	m_instance->log(6, function, filename, line);
}
