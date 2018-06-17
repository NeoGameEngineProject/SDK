//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __CORE_H
#define __CORE_H

#ifdef WIN32
	#ifdef _MSC_VER
		#pragma warning(disable: 4251)
	#endif

	#if defined(NEO_CORE_DLL)
		#define NEO_CORE_EXPORT __declspec( dllexport )
	#elif defined(NEO_CORE_STATIC)
		#define NEO_CORE_EXPORT
	#else
		#define NEO_CORE_EXPORT __declspec( dllimport )
	#endif

#else
	#define NEO_CORE_EXPORT
#endif

namespace Neo
{

enum VAR_TYPES
{
	VAR_BOOL = 0,
	VAR_BYTE,
	VAR_UBYTE,
	VAR_SHORT,
	VAR_USHORT,
	VAR_INT,
	VAR_UINT,
	VAR_FLOAT,
	VAR_DOUBLE
};

}

#endif
