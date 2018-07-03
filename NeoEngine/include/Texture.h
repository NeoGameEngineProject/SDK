#ifndef NEO_TEXTURE_H
#define NEO_TEXTURE_H

#include <Array.h>

namespace Neo 
{

class Texture
{
	bool m_mipmap = false;
	unsigned int m_componentSize = 0;
	unsigned int m_components = 0;
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	unsigned int m_textureId = -1;
	
	Array<unsigned char> m_data;
	
public:
	Texture() = default;
	Texture(Array<unsigned char>&& data, 
		unsigned int components,
		unsigned int compSize,
		unsigned int width,
		unsigned int height);
	
	void setMipMap(bool v) { m_mipmap = v; }
	bool hasMipMap() const { return m_mipmap; }
	unsigned int getComponents() const { return m_components; }
	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
	void setWidth(unsigned int width){ m_width = width; }
	void setHeight(unsigned int height){ m_height = height; }
	void setID(unsigned int textureId){ m_textureId = textureId; }
	unsigned int getID() const { return m_textureId; }
	
	void* getData() { return m_data.data; }
	
	template<typename componentType>
	componentType* create(unsigned int width, unsigned int height, unsigned int components)
	{
		m_componentSize = sizeof(componentType);
		m_components = components;
		m_width = width;
		m_height = height;
		
		m_data.alloc(width * height * m_componentSize * components);
		return m_data.data;
	}
	
	size_t getStorageSize() const { return m_width * m_height * m_componentSize * m_components; }
};

}

#endif // NEO_TEXTURE_H
