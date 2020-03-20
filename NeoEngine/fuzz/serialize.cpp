
#include <Level.h>
#include <string>
#include <ArrayStream.h>

#include <JsonScene.h>

using namespace Neo;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
	JsonScene scn;
	Level l;
	ArrayStream in((void*) Data, Size);

	try
	{
		scn.load(l, in);
	}
	catch(...)
	{
		return 0;
	}

	return 0;
}
