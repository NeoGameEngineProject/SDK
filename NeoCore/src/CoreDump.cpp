#include "CoreDump.h"
#include <Log.h>
#include <cstring>

using namespace Neo;

#ifndef WIN32
#include <unistd.h>
#include <csignal>

char s_dumpfile[128];
char s_dumpcommand[512];

// Only dumps out the current state.
static void SignalHandler(int signum)
{
	pid_t pid = getpid();
	int r = fork();
	if(r)
	{
		signal(signum, SIG_DFL);
		kill(getpid(), signum);
	}

	sleep(3);
	system(s_dumpcommand);
}
#endif

void Neo::RegisterCrashHandler(const char* prefix)
{
	#ifdef WIN32

	#else
		int pid = getpid();
		snprintf(s_dumpfile, sizeof(s_dumpfile), "/tmp/%s-%d.dmp", prefix, pid);

		// FIXME Using bash is discouraged, but easy. Better way?
		snprintf(s_dumpcommand, sizeof(s_dumpcommand),
					"coredumpctl -q -o %s dump %d "
					"&& zenity --error --no-wrap --text='Neo crashed, you can find a crash dump at \"%s\"'", s_dumpfile, pid, s_dumpfile);

		LOG_INFO("Crash dump will be written to: " << s_dumpfile);

		signal(SIGABRT, SignalHandler);
		signal(SIGFPE, SignalHandler);
		signal(SIGILL, SignalHandler);
		signal(SIGSEGV, SignalHandler);
	#endif
}
