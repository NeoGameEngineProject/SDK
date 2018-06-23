//========================================================================
// Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
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

#include "TextureLoader.h"
#include <Texture.h>
#include <FileTools.h>
#include <iostream>

#include <png.h>

using namespace Neo;

void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	M_fread(data, 1, length, (File *)png_get_io_ptr(png_ptr));
}

bool TextureLoader::load(Texture& image, const char* filename)
{
	File* file = M_fopen(filename, "rb");
	if (!file)
	{
		std::cerr << "ERROR Load PNG: Unable to open " << filename << std::endl;;
		return false;
	}

	unsigned char* header = new unsigned char[8];
	M_fread(header, sizeof(char), 8, file);
	if (png_sig_cmp(header, 0, 8))
	{
		M_fclose(file);
		delete [] header;
		return false;
	}
	M_rewind(file);
	delete [] header;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		return false;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return false;
	}

	png_set_read_fn(png_ptr, (void *)file, user_read_data);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

	unsigned int components;
	switch(color_type)
	{
		case PNG_COLOR_TYPE_RGB:
		components = 3;
		break;
		case PNG_COLOR_TYPE_RGBA:
		components = 4;
		break;
		default:
			std::cerr << "ERROR Load PNG: Unsupported color space in " << filename << std::endl;
		return false;
	}

	unsigned char* out = image.create<unsigned char>(width, height, components);
	
	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	for (unsigned int i = 0; i < height; i++)
		memcpy(out + (row_bytes * i), row_pointers[i], row_bytes);

	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	M_fclose(file);
	return true;
}
