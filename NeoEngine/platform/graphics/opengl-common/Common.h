#ifndef COMMON_H
#define COMMON_H

#include <Property.h>
#include <Renderer.h>
#include <FixedString.h>

#include <vector>
#include <string>
#include <memory>

namespace Neo
{
class Texture;
class LightBehavior;
class Level;
class Material;
}

class NEO_ENGINE_EXPORT Common : public Neo::Renderer
{
public:
	virtual ~Common();
	
	int loadShader(const char* path);
	int loadShader(const std::string& vert, const std::string& frag);
	int loadShader(const char* vert, const char* frag);

	int createTexture(Neo::Texture* tex);
	void setupMaterial(Neo::Material& material, const char* shaderName);

	void useShader(unsigned int id);
	virtual void enableMaterial(Neo::Material& material, const Neo::Matrix4x4& ModelView, const Neo::Matrix4x4& ModelViewProjection, const Neo::Matrix4x4& Normal);
	void enableMaterialTransform(Neo::Material& material, const Neo::Matrix4x4& transform, const Neo::Matrix4x4& view, const Neo::Matrix4x4& proj);

protected:
	std::string preprocess(const char* path);
	void compileShaders() override;
	
	struct NEO_ENGINE_EXPORT Shader
	{
		Shader() = default;
		Shader(Shader&&) = default;
		Shader(int id, const char* name): id(id), name(name) {}
		Shader(const Shader& sh): 
			id(sh.id), 
			name(sh.name),
			vertexSource(sh.vertexSource),
			fragmentSource(sh.fragmentSource),
			uModelView(sh.uModelView),
			uNormal(sh.uNormal),
			uTime(sh.uTime),
			uNumLights(sh.uNumLights),
			uboLights(sh.uboLights)
		{
			for(auto& p : sh.uniforms)
				uniforms.emplace_back(std::unique_ptr<Neo::IProperty>(p.first->clone()), p.second);
		}
		
		int id = -1;
		Neo::FixedString<64> name;
		
		typedef std::pair<std::unique_ptr<Neo::IProperty>, int> Uniform;
		std::vector<Uniform> uniforms;

		Uniform* findUniform(const char* name)
		{
			for(auto& u : uniforms)
				if(u.first->getName() == name)
					return &u;
			return nullptr;
		}

		std::string vertexSource, fragmentSource;
		
		// Builtin default uniforms
		int uModelView = -1, uModelViewProj = -1, uNormal = -1, uTime = -1;
		int uNumLights = -1, uboLights = -1;
		unsigned int uTextureFlags[8];
	};
	
	Shader* getShader(unsigned int id) { return &m_shaders[id]; }
	std::vector<Shader>& getShaders() { return m_shaders; }

private:
	
	std::vector<Shader> m_shaders;
	std::vector<unsigned int> m_textures;
	
	int findShader(const char* name);
	void gatherUniforms(const std::string& code, Shader& shader);
};


#endif
