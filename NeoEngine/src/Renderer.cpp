#include "Renderer.h"
#include <cstdlib>

using namespace Neo;

bool Renderer::GraphicsDebugging()
{
	auto* v = getenv("NEO_GRAPHICS_DEBUG");
	return (v && v[0] == '1' && v[1] == 0);
}
