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
#elif defined(WIN32)
//a#include <minidumpapiset.h>
#include <Windows.h>
#include <minidumpapiset.h>
#include <csignal>

char s_dumpfile[128];
static LONG ExceptionFilter(EXCEPTION_POINTERS* einfo)
{
	HANDLE currentProcessHandle = GetCurrentProcess();
	DWORD processId = GetCurrentProcessId();
	MINIDUMP_TYPE type = MiniDumpNormal;

	HANDLE outputFileHandle = CreateFile(s_dumpfile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	
	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = einfo;
	exceptionInfo.ClientPointers = FALSE;
	
	if (outputFileHandle != INVALID_HANDLE_VALUE)
	{
		MiniDumpWriteDump(currentProcessHandle, processId, outputFileHandle, type, &exceptionInfo, 0, 0);
		CloseHandle(outputFileHandle);
	}

	return 0;
}

static void WindowsSignals(int signum)
{
	RaiseException(signum, EXCEPTION_NONCONTINUABLE, 0, NULL);
}

#endif

void Neo::RegisterCrashHandler(const char* prefix)
{
	#ifdef WIN32
		SetUnhandledExceptionFilter(ExceptionFilter);

		signal(SIGABRT, WindowsSignals);
		signal(SIGFPE, WindowsSignals);
		signal(SIGILL, WindowsSignals);
		signal(SIGSEGV, WindowsSignals);

		int pid = GetCurrentProcessId();
		snprintf(s_dumpfile, sizeof(s_dumpfile), "C:\\temp\\%s-%d.dmp", prefix, pid);

		LOG_INFO("Crash dump will be written to: " << s_dumpfile);

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
