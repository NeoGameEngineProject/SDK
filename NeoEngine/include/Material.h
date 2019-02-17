#ifndef MATERIAL_H
#define MATERIAL_H

#include <Property.h>
#include <FixedString.h>

namespace Neo
{

enum BLENDING_MODES
{
	BLENDING_NONE = 0,
	BLENDING_ALPHA,
	BLENDING_ADD,
	BLENDING_SUB,
	BLENDING_LIGHT,
	BLENDING_PRODUCT
};

enum TEX_MODES
{
	TEX_DEPTH = 0,
	TEX_R = 1,
	TEX_RG = 2,
	TEX_RGB = 3,
	TEX_RGBA = 4
};

enum WRAP_MODES
{
	WRAP_REPEAT = 0,
	WRAP_CLAMP
};

enum CULL_MODES
{
	CULL_NONE = 0,
	CULL_FRONT,
	CULL_BACK,
	CULL_FRONT_BACK
};

class Texture;
class Material : public PropertySystem
{
public:
	// OLD
	// TODO Blend mode
	float opacity = 1;
	float shininess = 32;
	float customValue;
	Vector3 diffuseColor = Vector3(1, 1, 1);
	Vector3 specularColor = Vector3(1, 1, 1);
	Vector3 emitColor;
	Vector3 customColor;
	
	enum TEXTURE_TYPE
	{
		DIFFUSE = 0,
		NORMAL = 1,
		SPECULAR = 2,
		HEIGHT = 3,
		
		TEXTURE_MAX = 4
	};
	
	Texture* textures[8] {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	BLENDING_MODES blendMode = BLENDING_NONE;
	
	int getShader() const { return m_shader; }
	void setShader(int val) { m_shader = val; }
	
	const char* getShaderName() const { return m_shaderName.str(); }
	void setShaderName(const char* str) { m_shaderName = str; }
	
private:
	int m_shader = -1;
	FixedString<64> m_shaderName;
};

};

#endif
