#include <AssimpScene.h>

#include <Matrix4x4.h>

#include <assimp/scene.h>
#include <iostream>
#include <cassert>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/cfileio.h>
#include <assimp/pbrmaterial.h>

#include <assimp/cexport.h>

#include <Object.h>
#include <Level.h>
#include <FileTools.h>

#include <behaviors/LightBehavior.h>
#include <behaviors/MeshBehavior.h>
#include <behaviors/CameraBehavior.h>
#include <behaviors/StaticRenderBehavior.h>
#include <behaviors/SceneLinkBehavior.h>

#include <TextureLoader.h>

using namespace Neo;

// REGISTER_LEVEL_LOADER(AssimpScene)

#define GET_FILE(ai) ((File*) (ai->UserData))
static const int aiSeekTable[] = { SEEK_SET, SEEK_CUR, SEEK_END };

size_t aiRead(aiFile* file, char* data, size_t sz, size_t count)
{
	return M_fread(data, sz, count, GET_FILE(file));
}

aiReturn aiSeek(aiFile* file, size_t offset, aiOrigin origin)
{
	return (aiReturn) M_fseek(GET_FILE(file), offset, aiSeekTable[origin]);
}

size_t aiTell(aiFile* f)
{
	return M_ftell(GET_FILE(f));
}

size_t aiSize(aiFile* f)
{
	return M_fsize(GET_FILE(f));
}

size_t aiWrite(aiFile* file, const char* data, size_t sz, size_t count)
{
	return M_fwrite(data, sz, count, GET_FILE(file));
}

void aiClose(aiFileIO* io, aiFile* file)
{
	M_fclose(GET_FILE(file));
	delete file;
}

void aiFlush(aiFile* file)
{
	
}

aiFile* aiOpen(aiFileIO* io, const char* path, const char* mode)
{
	File* f = M_fopen(path, mode);
	if(!f)
		return NULL;
	
	aiFile* file = new aiFile;
	file->UserData = (aiUserData) f;
	file->FlushProc = aiFlush;
	file->ReadProc = aiRead;
	file->SeekProc = aiSeek;
	file->TellProc = aiTell;
	file->WriteProc = aiWrite;
	file->FileSizeProc = aiSize;
	
	return file;
}

static void loadMatrix(Matrix4x4& neoMat, aiMatrix4x4& aiMat)
{
	neoMat = Matrix4x4((float*) &aiMat);
	neoMat.transpose();
}

static Texture* findTexture(Level& level, const aiScene* scene, const std::string& basepath, const aiString& path, const std::string& materialName)
{
	auto* embedded = scene->GetEmbeddedTexture(path.C_Str());
	if(path.C_Str()[0] != '/' && !embedded)
		return level.loadTexture((basepath + path.C_Str()).c_str());
	else if(embedded)
		return level.loadTexture((materialName + path.C_Str()).c_str());
	else
		return level.loadTexture(path.C_Str());
}

constexpr static Neo::Material::TEXTURE_TYPE texTypeFromAssimp(aiTextureType type)
{
	using TYPE = Neo::Material::TEXTURE_TYPE;
	switch(type)
	{
		case aiTextureType_BASE_COLOR: return TYPE::ALBEDO;
		case aiTextureType_DIFFUSE_ROUGHNESS: return TYPE::ROUGHNESS;
		case aiTextureType_METALNESS: return TYPE::METALNESS;

		case aiTextureType_NORMALS: return TYPE::NORMAL;
		case aiTextureType_DISPLACEMENT: return TYPE::HEIGHT;
		
		case aiTextureType_DIFFUSE: return TYPE::DIFFUSE;
		case aiTextureType_SPECULAR: return TYPE::SPECULAR;
	}

	return TYPE::TEXTURE_MAX;
}

static Texture* createTexture(aiTextureType type, Level& level, const aiScene* scene, const aiMaterial* aiMat, const std::string& basepath, const std::string& materialName)
{
	aiString path;
	aiTextureMapping mapping;
	unsigned int uvindex = 0;
	float blend = 0.0f;
	aiTextureOp op;

	// FIXME: Bug in Assimp: They use 64bit integers to save the aiTextureMapMode
	// but enums are always 32bit leading to a stack corruption!
	//aiTextureMapMode mapmode;
	long long mapmode = 0;

	if(AI_SUCCESS == aiMat->GetTexture(type, 0, &path, &mapping, &uvindex, &blend, &op, (aiTextureMapMode*) &mapmode))
	{
		LOG_DEBUG("Loading texture: " << basepath << path.C_Str());
		
		Texture* texture = findTexture(level, scene, basepath, path, materialName);
		assert(texture);
		if(mapmode == aiTextureMapMode_Clamp)
		{
			//texture->setUWrapMode(WRAP_CLAMP);
			//texture->setVWrapMode(WRAP_CLAMP);
		}
		
		return texture;
	}

	return nullptr;
}

static void traverseAssimpScene(Level* level,
				ObjectHandle neoRoot, 
				aiNode* root, 
				const std::vector<MeshHandle>& meshes, 
				const std::unordered_map<std::string, LightBehavior>& lights,
				const std::unordered_map<std::string, std::pair<Matrix4x4, CameraBehavior>>& cameras)
{
	// Alloc RAM and iterate over every child
	auto& parentList = neoRoot->getChildren();
	parentList.reserve(root->mNumChildren);
	for(int i = 0; i < root->mNumChildren; i++)
	{
		aiNode* child = root->mChildren[i];
		ObjectHandle neoChild = neoRoot->addChild(level->addObject(child->mName.C_Str()));

		// No mesh means it is either a camera or light!
		if(child->mNumMeshes  == 0)
		{
			auto light = lights.find(child->mName.C_Str());
			auto camera = cameras.find(child->mName.C_Str());

			// If light with the name was found, add it to the scene
			if(light != lights.end())
			{
				neoChild->addBehavior(std::make_unique<LightBehavior>(light->second));
				loadMatrix(neoChild->getTransform(), child->mTransformation);
				neoChild->updateFromMatrix();
				
				traverseAssimpScene(level, neoChild, child, meshes, lights, cameras);
			}
			else if(camera != cameras.end())
			{
				neoChild->addBehavior(std::make_unique<CameraBehavior>(camera->second.second));
				loadMatrix(neoChild->getTransform(), child->mTransformation);
				neoChild->getTransform() = camera->second.first * neoChild->getTransform();
				neoChild->updateFromMatrix();
				
				traverseAssimpScene(level, neoChild, child, meshes, lights, cameras);
			}
			else
			{
				std::cerr << "Unknown object type of object " << child->mName.C_Str() << std::endl;
			}
			
			continue;
		}
		
		// Insert into parent list
		auto meshBehavior = std::make_unique<MeshBehavior>();
		auto& subMeshes = meshBehavior->getMeshes();
		subMeshes.reserve(child->mNumMeshes);
		
		for(size_t j = 0; j < child->mNumMeshes; j++)
		{
			assert(child->mMeshes[j] < meshes.size());
			subMeshes.push_back(meshes[child->mMeshes[j]]);
		}
		
		meshBehavior->updateBoundingBox();
		neoChild->addBehavior(std::move(meshBehavior));

		// Make it renderable
		// TODO Select type of renderer!
		neoChild->addBehavior(std::make_unique<StaticRenderBehavior>());
		
		loadMatrix(neoChild->getTransform(), child->mTransformation);
		neoChild->updateFromMatrix();

		// Insert children into new parent
		traverseAssimpScene(level, neoChild, child, meshes, lights, cameras);
	}
}

bool AssimpScene::loadFile(Level& level, const std::string& file, ObjectHandle root)
{
	if(root.empty())
	{
		root = level.getRoot();
	}
	else
	{
		root->addBehavior<Neo::SceneLinkBehavior>()->setFilename(file);
	}

	aiFileIO iostruct;
	iostruct.OpenProc = aiOpen;
	iostruct.CloseProc = aiClose;
	
	// Import scene from the given file!
	const aiScene* scene = aiImportFileEx(file.c_str(), 0, &iostruct);
	if(!scene || !aiApplyPostProcessing(scene,
		// aiProcess_JoinIdenticalVertices
		aiProcess_SortByPType
		| aiProcess_Triangulate
		| aiProcess_FindInstances
		| aiProcess_ImproveCacheLocality
		| aiProcess_SplitLargeMeshes
		| aiProcess_CalcTangentSpace
		| aiProcess_GenNormals))
	{
		std::cerr << "Could not load level: " << aiGetErrorString() << std::endl;
		return false;
	}
	
	// assert(!scene->HasTextures() && "No embedded textures please!");
	
	std::string basepath(file);
#ifndef WIN32
	basepath.erase(basepath.find_last_of('/') + 1);
#else
	basepath.erase(basepath.find_last_of('\\') + 1);
#endif
	
	// First, load all meshes into the resource cache
	std::vector<MeshHandle> meshes;
	meshes.reserve(scene->mNumMeshes);
	
	std::vector<Material> materials;
	materials.reserve(scene->mNumMaterials);
	
	for(size_t i = 0; i < scene->mNumMaterials; i++)
	{
		auto aiMat = scene->mMaterials[i];
		materials.emplace_back();
		Material& material = materials.back();
		std::string materialName = aiMat->GetName().C_Str();
	
		int blendMode;
		if(AI_SUCCESS == aiGetMaterialInteger(aiMat, AI_MATKEY_BLEND_FUNC, &blendMode))
		{
			switch(blendMode)
			{
				default:
				case aiBlendMode_Default: material.blendMode = BLENDING_ALPHA; break;
				case aiBlendMode_Additive: material.blendMode = BLENDING_ADD; break;
			}
		}

		aiColor4D color;
		if(AI_SUCCESS == aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &color))
			material.setProperty("Diffuse", Vector3(color.r, color.g, color.b));
		
		if(AI_SUCCESS == aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_SPECULAR, &color))
			material.setProperty("Specular", Vector3(color.r, color.g, color.b));
		
		if(AI_SUCCESS == aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_EMISSIVE, &color))
			material.setProperty("Emit", Vector3(color.r, 
							color.g,
							color.b));

		if(AI_SUCCESS == aiGetMaterialFloat(aiMat, AI_MATKEY_OPACITY, &material.getProperty("Opacity")->get<float>())) {}

		// auto* prop = material.getProperty("Diffuse");
		std::string shaderPrefix = "glTF";
		bool isPBR = true;
		float value;
		if(AI_SUCCESS == aiGetMaterialFloat(aiMat, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, &value))
		{
			material.registerProperty<float>("Roughness");
			material.setProperty("Roughness", value);
		}
		else
		{
			isPBR = false;
			shaderPrefix = "";
			if(AI_SUCCESS == aiGetMaterialFloat(aiMat, AI_MATKEY_SHININESS, &material.getProperty("Shininess")->get<float>()))
			{
				// material.getProperty("Shininess")->get<float>() *= 0.025; // Need to quarter values since Assimp multiplies with 4 for some reason.
			}
		}

		if(AI_SUCCESS == aiGetMaterialFloat(aiMat, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, &value))
		{
			material.registerProperty<float>("Metalness");
			material.setProperty("Metalness", value);
		}
	
		// Textures
		{
			if(scene->HasTextures())
			{
				LOG_DEBUG("Loading embedded textures for material " << materialName);
				for(int i = 0; i < scene->mNumTextures; i++)
				{
					auto* aiTex = scene->mTextures[i];
					Texture tex;

					if(aiTex->mHeight == 0)
					{
						LOG_DEBUG("Compressed texture");
						if(aiTex->CheckFormat("png"))
						{
							LOG_DEBUG("Found PNG texture!");
							TextureLoader::load(tex, "png", (unsigned char*) aiTex->pcData, aiTex->mWidth);
						}
						else
						{
							LOG_ERROR("Unsupported texture!");
							continue;
						}
					}
					else
					{
						// TODO Check channel count!
						tex.create<unsigned char>(aiTex->mWidth, aiTex->mHeight, 3);
						memcpy(tex.getData(), aiTex->pcData, tex.getStorageSize());
					}

					level.loadTexture((materialName + '*' + std::to_string(i)).c_str(), std::move(tex));
				}
			}

			aiString path;
			aiTextureMapping mapping;
			unsigned int uvindex = 0;
			float blend = 0.0f;
			aiTextureOp op;

			// FIXME: Bug in Assimp: They use 64bit integers to save the aiTextureMapMode
			// but enums are always 32bit leading to a stack corruption!
			//aiTextureMapMode mapmode;
			long long mapmode = 0;
	
			// Default: Diffuse shading with a single color
			material.textures[Material::NORMAL] = createTexture(aiTextureType_NORMALS, level, scene, aiMat, basepath, materialName);
			material.textures[Material::HEIGHT] = createTexture(aiTextureType_DISPLACEMENT, level, scene, aiMat, basepath, materialName);

			std::string shaderName;
			if(isPBR) // Even though PBR support is quite poor right now.
			{
				bool albedo = material.textures[Material::ALBEDO] = createTexture(aiTextureType_DIFFUSE, level, scene, aiMat, basepath, materialName);
				bool roughness = material.textures[Material::ROUGHNESS] = createTexture(aiTextureType_DIFFUSE_ROUGHNESS, level, scene, aiMat, basepath, materialName);
				bool metalness = material.textures[Material::METALNESS] = createTexture(aiTextureType_METALNESS, level, scene, aiMat, basepath, materialName);

				shaderName = "pbr";
				if(albedo) shaderName += "Albedo";
				else shaderName += "Color";
				
				if(roughness) shaderName += "Roughness";
				if(metalness) shaderName += "Metalness";
			}
			else
			{
				bool diffuse = material.textures[Material::DIFFUSE] = createTexture(aiTextureType_DIFFUSE, level, scene, aiMat, basepath, materialName);
				bool specular = material.textures[Material::SPECULAR] = createTexture(aiTextureType_SPECULAR, level, scene, aiMat, basepath, materialName);

				if(diffuse) shaderName += "Diffuse";
				else shaderName += "Color";

				if(specular) shaderName += "Specular";
			}

			if(material.textures[Material::NORMAL]) shaderName += "Normal";
			if(material.textures[Material::HEIGHT]) shaderName += "Height";

			material.setShaderName(shaderName);
		}
	}
	
	for(size_t i = 0; i < scene->mNumMeshes; i++)
	{
		Mesh subMesh;
		const aiMesh* mesh = scene->mMeshes[i];
		subMesh.set(mesh->mNumVertices, 
			    (Vector3*) mesh->mVertices, 
			    (Vector3*) mesh->mNormals,
			    (Vector3*) mesh->mTangents,
			    (Vector3*) mesh->mBitangents,
			    0, nullptr);

		auto& textureChannels = subMesh.getTextureChannels();
		textureChannels.reserve(4);
		
		// Set texture channels
		for(size_t j = 0; j < 8; j++)
		{
			auto aiChannel = mesh->mTextureCoords[j];
			if(aiChannel)
			{
				Array<Vector2> coords;
				coords.alloc(mesh->mNumVertices);
				for(size_t p = 0; p < mesh->mNumVertices; p++)
				{
					coords[p] = Vector2(aiChannel[p].x, 1.0f - aiChannel[p].y);
				}
				
				textureChannels.push_back(std::move(coords));
			}
		}

		if(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
		{
			subMesh.setFormat(MESH_FORMAT::TRIANGLES);
		}
		else if(mesh->mPrimitiveTypes & aiPrimitiveType_LINE)
		{
			subMesh.setFormat(MESH_FORMAT::LINES);
		}
		else if(mesh->mPrimitiveTypes & aiPrimitiveType_POINT)
		{
			subMesh.setFormat(MESH_FORMAT::POINTS);
		}

		auto& indices = subMesh.getIndices();
		indices.resize(mesh->mNumFaces*3);
		for(size_t j = 0; j < mesh->mNumFaces; j++)
		{
			assert(mesh->mFaces[j].mNumIndices == 3);
			const size_t idx = j*3;
			indices[idx] = mesh->mFaces[j].mIndices[0];
			indices[idx+1] = mesh->mFaces[j].mIndices[1];
			indices[idx+2] = mesh->mFaces[j].mIndices[2];
		}
		
		subMesh.setMaterial(materials[mesh->mMaterialIndex]);
		subMesh.setName(scene->mMeshes[i]->mName.C_Str());
		meshes.push_back(level.addMesh(std::move(subMesh)));
	}
	
	// We need to find all lights in the scene.
	// Assimp does not provide lights in the tree itself so we need to match names here.
	// There are several ways to do it, linear search for each light node or a tree traverse.
	// Using a hash map allows us to look up lights in constant time instead so no search is
	// needed whatsoever!
	std::unordered_map<std::string, LightBehavior> lights;
	lights.reserve(scene->mNumLights);
	for(size_t i = 0; i < scene->mNumLights; i++)
	{
		const auto ailight = scene->mLights[i];
		LightBehavior& light = lights[ailight->mName.C_Str()];
		
		if(ailight->mType != aiLightSourceType::aiLightSource_POINT)
		{
			light.exponent = ailight->mAngleInnerCone;
			light.angle = ailight->mAngleOuterCone;
		}
		
		light.attenuation = ailight->mAttenuationQuadratic;
		light.diffuse = Vector3(ailight->mColorDiffuse.r, ailight->mColorDiffuse.g, ailight->mColorDiffuse.b);
		light.specular = Vector3(ailight->mColorSpecular.r, ailight->mColorSpecular.g, ailight->mColorSpecular.b);
		light.brightness = 1.0;
	}
	
	std::unordered_map<std::string, std::pair<Matrix4x4, CameraBehavior>> cameras;
	cameras.reserve(scene->mNumCameras);
	for(size_t i = 0; i < scene->mNumCameras; i++)
	{
		const auto aicam = scene->mCameras[i];
		std::pair<Matrix4x4, CameraBehavior>& camera = cameras[aicam->mName.C_Str()];
		
		camera.second.setFov(AI_RAD_TO_DEG(aicam->mHorizontalFOV));
		camera.second.setNear(aicam->mClipPlaneNear);
		camera.second.setFar(aicam->mClipPlaneFar);
		
		camera.first.lookAt(
			Vector3(aicam->mLookAt.x, aicam->mLookAt.y, aicam->mLookAt.z),
			Vector3(aicam->mPosition.x, aicam->mPosition.y, aicam->mPosition.z),
			Vector3(aicam->mUp.x, aicam->mUp.y, aicam->mUp.z));
		
		camera.first.invert();
	}
	
	// Second, load all scene nodes and place them
	// Kick it off!
	traverseAssimpScene(&level, root, scene->mRootNode, meshes, lights, cameras);
	aiReleaseImport(scene);
	
	return true;
}

bool AssimpScene::saveFile(Level& level, const std::string& file, ObjectHandle root)
{
	return false;
}

bool AssimpScene::supportsExtension(const std::string& ext)
{
	return aiIsExtensionSupported(("." + ext).c_str());
}
