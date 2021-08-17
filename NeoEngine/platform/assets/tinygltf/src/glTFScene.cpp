#include <glTFScene.h>
#include <FileTools.h>
#include <Log.h>
#include <Level.h>

#include <behaviors/CameraBehavior.h>
#include <behaviors/MeshBehavior.h>
#include <behaviors/StaticRenderBehavior.h>
#include <behaviors/SceneLinkBehavior.h>

#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
// #define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_FS
#define TINYGLTF_USE_CPP14

// TODO Use rapidjson!
// #define TINYGLTF_USE_RAPIDJSON
#include <tinygltf/tiny_gltf.h>

using namespace Neo;

template<typename T>
static const T& indexArray(const std::vector<unsigned char>& data, unsigned int offset, unsigned int stride, unsigned int idx)
{
	return *reinterpret_cast<const T*>(&data.at(offset + stride * idx));
}

static Vector3 readVector3(const std::vector<double>& vct)
{
	if(vct.size() < 3)
		return Vector3();

	return Vector3(vct[0], vct[1], vct[2]);
}

static Quaternion readQuaternion(const std::vector<double>& vct)
{
	if(vct.size() < 4)
		return Quaternion();

	return Quaternion(vct[0], vct[1], vct[2], vct[3]);
}

static Matrix4x4 readMatrix(const std::vector<double>& vct)
{
	Matrix4x4 mtx;

	std::cout << vct.size() << std::endl;

	if(vct.size() < 16)
		return mtx;

	for(unsigned int i = 0; i < 16; i++)
	{
		mtx.entries[i] = vct[i];
	}
	return mtx;
}

static MESH_FORMAT gltf2neo(int mode)
{
	switch(mode)
	{
		case TINYGLTF_MODE_POINTS: return MESH_FORMAT::POINTS;
		case TINYGLTF_MODE_LINE: return MESH_FORMAT::LINES;
		case TINYGLTF_MODE_LINE_LOOP: return MESH_FORMAT::LINE_LOOP;
		case TINYGLTF_MODE_LINE_STRIP: return MESH_FORMAT::LINE_STRIP;
		case TINYGLTF_MODE_TRIANGLE_STRIP: return MESH_FORMAT::TRIANGLE_STRIP;
		case TINYGLTF_MODE_TRIANGLE_FAN: return MESH_FORMAT::TRIANGLE_FAN;

		default: LOG_WARNING("Unknown geometry mode: " << mode);
		case TINYGLTF_MODE_TRIANGLES:
			return MESH_FORMAT::TRIANGLES;
	}
}

static void processNode(Level& level, ObjectHandle root, const tinygltf::Model& model, const tinygltf::Node& node)
{
	auto object = level.addObject(node.name.c_str());
	root->addChild(object);
	object->setParent(root);

	object->setActive(true);

	if(!node.matrix.empty())
	{
		object->getTransform() = readMatrix(node.matrix);
		object->updateFromMatrix();
	}
	else
	{
		object->setPosition(readVector3(node.translation));
		object->setRotation(readQuaternion(node.rotation));
		object->setScale(Vector3(1, 1, 1) + readVector3(node.scale));
		object->updateMatrix();
	}

	if(node.camera != -1)
	{
		auto& cam = model.cameras[node.camera];
		auto* b = object->addBehavior<CameraBehavior>();

		if(cam.type == "orthographic")
		{
			LOG_WARNING("Ortho cameras not yet implemented!");
		}
		else
		{
			b->setFov(cam.perspective.yfov);
			b->setNear(cam.perspective.znear);
			b->setFar(cam.perspective.zfar);
		}
	}

	if(node.mesh != -1)
	{
		auto* b = object->addBehavior<MeshBehavior>();
		object->addBehavior<StaticRenderBehavior>();

		auto& gltfMesh = model.meshes[node.mesh];
		auto& meshes = b->getMeshes();
		
		for(auto& p : gltfMesh.primitives)
		{
			Neo::Mesh mesh;
			auto& vertices = mesh.getVertices();
			mesh.setFormat(gltf2neo(p.mode));

			{
				auto& indices = mesh.getIndices();
				auto& accessor = model.accessors[p.indices];

				auto count = accessor.count;
				if(count % 3 != 0)
					LOG_WARNING("Mesh is not triangular!");

				indices.resize(count);

				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				const auto stride = accessor.ByteStride(bufferView);
				LOG_INFO(accessor.componentType);
				unsigned short* idxArray = (unsigned short*) buffer.data.data();
				for(unsigned int i = 0; i < count; i++)
				{
					if(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						indices[i] = static_cast<unsigned int>(indexArray<unsigned short>(buffer.data, accessor.byteOffset + bufferView.byteOffset, stride, i));
					else if(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
						indices[i] = static_cast<unsigned int>(indexArray<unsigned int>(buffer.data, accessor.byteOffset + bufferView.byteOffset, stride, i));
					else
					{
						LOG_ERROR("Unknown index format: " << accessor.componentType);
						break;
					}
				}
			}

			// auto& vertices = model.accessors[p.]

			// Get attributes
			auto verticesIter = p.attributes.find("POSITION");
			auto normalsIter = p.attributes.find("NORMAL");

			if(verticesIter != p.attributes.end())
			{
				auto& accessor = model.accessors[verticesIter->second];
				auto count = accessor.count;
				vertices.resize(count);

				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				const auto stride = accessor.ByteStride(bufferView);

				for(unsigned int i = 0; i < count; i++)
				{
					auto& v = vertices[i];
					v.position = indexArray<Vector3>(buffer.data, accessor.byteOffset + bufferView.byteOffset, stride, i);
				}
			}
			else
			{
				LOG_WARNING("Mesh with no vertices ignored ('" << gltfMesh.name << "')");
				continue;
			}

			if(normalsIter != p.attributes.end())
			{
				auto& accessor = model.accessors[normalsIter->second];
				auto count = accessor.count;
				
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				const auto stride = accessor.ByteStride(bufferView);
				for(unsigned int i = 0; i < count; i++)
				{
					auto& v = vertices[i];
					v.normal = indexArray<Vector3>(buffer.data, accessor.byteOffset + bufferView.byteOffset, stride, i);
				}
			}
			else
			{
				LOG_WARNING("Mesh with no normals ignored ('" << gltfMesh.name << "')");
				continue;
			}

			for(int i = 0; i < 4; i++)
			{
				auto texcoordIter = p.attributes.find("TEXCOORD_" + std::to_string(i));

				if(texcoordIter != p.attributes.end())
				{
					auto& accessor = model.accessors[texcoordIter->second];
					auto count = accessor.count;
					vertices.resize(count);

					auto& bufferView = model.bufferViews[accessor.bufferView];
					auto& buffer = model.buffers[bufferView.buffer];

					const auto stride = accessor.ByteStride(bufferView);

					Neo::Array<Vector2> texcoords;
					texcoords.alloc(count);

					for(unsigned int i = 0; i < count; i++)
					{
						texcoords[i] = indexArray<Vector2>(buffer.data, accessor.byteOffset + bufferView.byteOffset, stride, i);
					}

					mesh.getTextureChannels().push_back(std::move(texcoords));
				}
			}

			Neo::Material mat;
			auto& gltfMat = model.materials[p.material];

			mat.registerProperty<float>("Roughness");
			mat.registerProperty<float>("Metalness");

			LOG_INFO("Loading material: " << gltfMat.name);

			mat.setProperty<float>("Roughness", gltfMat.pbrMetallicRoughness.roughnessFactor);
			mat.setProperty<float>("Metalness", gltfMat.pbrMetallicRoughness.metallicFactor);
			mat.setProperty<Vector3>("Diffuse", readVector3(gltfMat.pbrMetallicRoughness.baseColorFactor));
			mat.setProperty<Vector3>("Emit", readVector3(gltfMat.emissiveFactor));
			mat.setProperty<float>("Opacity", 1.0f);
		
			#define LOAD_TEX(t) ((t).index == -1 ? nullptr : level.loadTexture(model.images[model.textures[(t).index].source].name.c_str()))
			#define PRINT_TEX(t) if((t).index == -1) LOG_INFO("Loading " << model.images[model.textures[(t).index].source].name)

			bool albedo = mat.textures[Neo::Material::ALBEDO] = LOAD_TEX(gltfMat.pbrMetallicRoughness.baseColorTexture);
			bool roughness = mat.textures[Neo::Material::ROUGHNESS] = LOAD_TEX(gltfMat.pbrMetallicRoughness.metallicRoughnessTexture);
			bool normal = mat.textures[Neo::Material::NORMAL] = LOAD_TEX(gltfMat.normalTexture);

			#undef PRINT_TEX
			#undef LOAD_TEX

			std::string shaderName = "pbr";
			if(albedo) shaderName += "Albedo";
			else shaderName += "Color";
			
			if(roughness) shaderName += "Roughness";
			mat.setShaderName(shaderName);

			mesh.setMaterial(mat);
			meshes.push_back(level.addMesh(std::move(mesh)));
		}

		b->updateBoundingBox();
	}

	// Find out if a light is involved
	auto lightExt = node.extensions.find("KHR_lights_punctual");
	if(lightExt != node.extensions.end())
	{
		auto& value = lightExt->second;
		auto& light = model.lights[value.Get("light").GetNumberAsInt()];
		
		LOG_INFO("Found a light: " << node.name << " " << light.name);
		
		auto* l = object->addBehavior<LightBehavior>();
		l->specular = l->diffuse = Vector3(light.color[0], light.color[1], light.color[2]);
		l->brightness = light.intensity;

		// https://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
		// https://github.com/KhronosGroup/glTF/blob/b505b52f552d4a7512c7a3e8e3fa4953d95af050/extensions/2.0/Khronos/KHR_lights_punctual/README.md
		l->attenuation = (light.range == 0.0f ? 0.0f : 1.0 / (light.range*light.range * 0.001f));

		if(light.type == "spot")
		{
			l->exponent = light.spot.innerConeAngle;
			l->angle = light.spot.outerConeAngle;
		}
	}

	for(const auto& nodeId : node.children)
	{
		const auto& node = model.nodes[nodeId];
		processNode(level, object, model, node);
	}
}

bool glTFScene::loadFile(Level& level, const std::string& file, ObjectHandle root)
{
	if(root.empty())
		root = level.getRoot();
	else
		root->addBehavior<Neo::SceneLinkBehavior>()->setFilename(file);

	unsigned int size = 0;
	char* content = readBinaryFile(file.c_str(), &size);

	if(!content)
		return false;

	bool success = false;
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string errstr, warnstr;

	auto ext = findExtension(file);
	if(ext == "glb")
	{
		success = loader.LoadBinaryFromMemory(&model, &errstr, &warnstr, (unsigned char*) content, size, findPath(file));
	}
	else if(ext == "gltf")
	{
		success = loader.LoadASCIIFromString(&model, &errstr, &warnstr, content, size, findPath(file));
	}
	delete[] content;

	if(!success)
	{
		LOG_ERROR("Could not load glTF file: " << errstr);
		return false;
	}

	if(!warnstr.empty())
		LOG_WARNING("glTF Warning: " << warnstr);

	// Fill empty names to be unique
	for(unsigned int i = 0; i < model.images.size(); i++)
	{
		if(model.images[i].name.empty())
			model.images[i].name = "image_" + std::to_string(i);
	}

	for(unsigned int i = 0; i < model.textures.size(); i++)
	{
		if(model.textures[i].name.empty())
			model.textures[i].name = "texture_" + std::to_string(i);
	}

	for(unsigned int i = 0; i < model.cameras.size(); i++)
	{
		if(model.cameras[i].name.empty())
			model.cameras[i].name = "camera_" + std::to_string(i);
	}

	for(unsigned int i = 0; i < model.materials.size(); i++)
	{
		if(model.materials[i].name.empty())
			model.materials[i].name = "material_" + std::to_string(i);
	}

	// Load textures, yay!
	for(auto& t : model.images)
	{
		const std::string& name = t.name;

		LOG_DEBUG("Loading internal texture: name='" << name << "' size=" << t.width << "x" << t.height << "@" << t.bits);

		Texture texture;
		switch(t.bits)
		{
		case 8: texture.create<unsigned char>(t.width, t.height, t.component); break;
		case 16: texture.create<unsigned short>(t.width, t.height, t.component); break;
		case 32: texture.create<unsigned int>(t.width, t.height, t.component); break;
		}
		
		memcpy(texture.getData(), t.image.data(), texture.getStorageSize());

		texture.setMipMap(true);
		level.loadTexture(name.c_str(), std::move(texture));
	}

	// Fill in root node!
	for(auto& scn : model.scenes)
	{
		for(auto nid : scn.nodes)
		{
			processNode(level, root, model, model.nodes[nid]);
		}
	}

	return true;
}

bool glTFScene::saveFile(Level& level, const std::string& file, ObjectHandle root)
{
	return false;
}

bool glTFScene::supportsExtension(const std::string& ext)
{
	return ext == "glb" || ext == "gltf";
}
