#include "Texture.h"
#include <utility>

using namespace Neo;

Texture::Texture(Array<unsigned char>&& data, 
			unsigned int components,
			unsigned int compSize,
			unsigned int width,
			unsigned int height):
	m_data(std::move(data)),
	m_components(components),
	m_componentSize(compSize),
	m_width(width),
	m_height(height) {}
