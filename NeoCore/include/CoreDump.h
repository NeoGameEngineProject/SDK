#ifndef __CORE_DUMP_H__
#define __CORE_DUMP_H__

#include "NeoCore.h"

namespace Neo
{
void NEO_CORE_EXPORT RegisterCrashHandler(const char* prefix = "Neo");
}

#endif
