
// Emulate ordinary main using WinMain
#ifdef WIN32
#include <windows.h>

extern "C" int main(int argc, char** argv);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	int argc;
	wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
	
	char** argv = new char*[argc];
	char buffer[256];

	for (size_t i = 0; i < argc; i++)
	{
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, buffer, sizeof(buffer), 0, nullptr);
		argv[i] = strdup(buffer);
	}
	
	LocalFree(wargv);
	return main(argc, argv);
}
#endif
