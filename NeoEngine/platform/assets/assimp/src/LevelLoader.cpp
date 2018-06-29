#include "LevelLoader.h"
#include <Matrix4x4.h>

#include <assimp/scene.h>
#include <iostream>
#include <cassert>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <Object.h>
#include <Level.h>

#include <behaviors/LightBehavior.h>
#include <behaviors/MeshBehavior.h>
#include <behaviors/CameraBehavior.h>
#include <behaviors/StaticRenderBehavior.h>

using namespace Neo;

static void loadMatrix(Matrix4x4& neoMat, aiMatrix4x4& aiMat)
{
	neoMat = Matrix4x4((float*) &aiMat);
	neoMat.transpose();
}

static void traverseAssimpScene(Level* level,
				Object* neoRoot, 
				aiNode* root, 
				const std::vector<SubMesh>& meshes, 
				const std::unordered_map<std::string, LightBehavior>& lights,
				const std::unordered_map<std::string, std::pair<Matrix4x4, CameraBehavior>>& cameras)
{
	// Alloc RAM and iterate over every child
	auto& parentList = neoRoot->getChildren();
	parentList.reserve(root->mNumChildren);
	for(int i = 0; i < root->mNumChildren; i++)
	{
		aiNode* child = root->mChildren[i];
		Object* neoChild = neoRoot->addChild(level->addObject(child->mName.C_Str()));

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
		auto& subMeshes = meshBehavior->getSubMeshes();
		subMeshes.reserve(child->mNumMeshes);
		
		for(size_t j = 0; j < child->mNumMeshes; j++)
		{
			assert(child->mMeshes[j] < meshes.size());
			subMeshes.push_back(meshes[child->mMeshes[j]]);
		}
		
		neoChild->addBehavior(std::move(meshBehavior));
		// Make it renderable
		// TODO Selet type of renderer!
		neoChild->addBehavior(std::make_unique<StaticRenderBehavior>());
		
		loadMatrix(neoChild->getTransform(), child->mTransformation);
		neoChild->updateFromMatrix();

		// Insert children into new parent
		traverseAssimpScene(level, neoChild, child, meshes, lights, cameras);
	}
}


bool LevelLoader::load(Level& level, const char* file)
{
	// Import scene from the given file!
	const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality 
							| aiProcess_Triangulate 
							| aiProcess_OptimizeMeshes 
							| aiProcess_ImproveCacheLocality
							| aiProcess_SplitLargeMeshes);

	if(!scene)
	{
		std::cerr << "Could not load level: " << aiGetErrorString() << std::endl;
		return false;
	}
	
	assert(!scene->HasTextures() && "No embedded textures please!");
	
	std::string basepath(file);
#ifndef WIN32
	basepath.erase(basepath.find_last_of('/') + 1);
#else
	basepath.erase(basepath.find_last_of('\\') + 1);
#endif
	
	// First, load all meshes into the resource cache
	std::vector<SubMesh> meshes;
	meshes.reserve(scene->mNumMeshes);
	
	std::vector<Material> materials;
	materials.reserve(scene->mNumMaterials);
	
	for(size_t i = 0; i < scene->mNumMaterials; i++)
	{
		auto aiMat = scene->mMaterials[i];
		Material material;
		
		aiColor4D color;
		if(AI_SUCCESS == aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &color))
			material.diffuse = Vector3(color.r, color.g, color.b);
		
		if(AI_SUCCESS == aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_SPECULAR, &color))
			material.specular = Vector3(color.r, color.g, color.b);
		
		if(AI_SUCCESS == aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_EMISSIVE, &color))
			material.emit = Vector3(color.r, color.g, color.b);
	
		if(AI_SUCCESS == aiGetMaterialFloat(aiMat, AI_MATKEY_OPACITY, &material.opacity))
		{}
		
		if(AI_SUCCESS == aiGetMaterialFloat(aiMat, AI_MATKEY_SHININESS, &material.shininess))
		{
			material.shininess *= 0.025; // Need to quarter values since Assimp multiplies with 4 for some reason.
		}
		
		// Textures
		{
			aiString path;
			aiTextureMapping mapping;
			unsigned int uvindex;
			float blend;
			aiTextureOp op;

			// FIXME: Bug in Assimp: They use 64bit integers to save the aiTextureMapMode
			// but enums are always 32bit leading to a stack corruption!
			//aiTextureMapMode mapmode;
			long long mapmode = 0;

			if(AI_SUCCESS == aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path, &mapping, &uvindex, &blend, &op, (aiTextureMapMode*) &mapmode))
			{
				Texture* texture = level.loadTexture((basepath + path.C_Str()).c_str());
				if(mapmode == aiTextureMapMode_Clamp)
				{
					//texture->setUWrapMode(WRAP_CLAMP);
					//texture->setVWrapMode(WRAP_CLAMP);
				}
				
				assert(uvindex < 4 && material.textures[uvindex] == nullptr);
				material.textures[uvindex] = texture;
				//material.addTexturePass(texture, TEX_COMBINE_MODULATE, uvindex);
			}
		
			/*if(AI_SUCCESS == aiMat->GetTexture(aiTextureType_SPECULAR, 0, &path, &mapping, &uvindex, &blend, &op, (aiTextureMapMode*) &mapmode))
			{
				getGlobalFilename(globalPath, meshRep, path.C_Str());
				TextureRef * texRef = level->loadTexture(globalPath, true);
				Texture * texture = mesh->addNewTexture(texRef);
				if(mapmode == aiTextureMapMode_Clamp){
					texture->setUWrapMode(WRAP_CLAMP);
					texture->setVWrapMode(WRAP_CLAMP);
				}
				
				while(material.getTexturesPassNumber() < 1)
					material.addTexturePass(NULL, TEX_COMBINE_MODULATE, 0);
				
				material.addTexturePass(texture, TEX_COMBINE_MODULATE, uvindex);
			}
		
			if(AI_SUCCESS == aiMat->GetTexture(aiTextureType_NORMALS, 0, &path, &mapping, &uvindex, &blend, &op, (aiTextureMapMode*) &mapmode))
			{
				getGlobalFilename(globalPath, meshRep, path.C_Str());
				TextureRef * texRef = level->loadTexture(globalPath, true);
				Texture * texture = mesh->addNewTexture(texRef);
				if(mapmode == aiTextureMapMode_Clamp){
					texture->setUWrapMode(WRAP_CLAMP);
					texture->setVWrapMode(WRAP_CLAMP);
				}
				
				while(material.getTexturesPassNumber() < 2)
					material.addTexturePass(NULL, TEX_COMBINE_MODULATE, 0);
					
				material.addTexturePass(texture, TEX_COMBINE_MODULATE, uvindex);
			}
		
			if(AI_SUCCESS == aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &path, &mapping, &uvindex, &blend, &op, (aiTextureMapMode*) &mapmode))
			{
				getGlobalFilename(globalPath, meshRep, path.C_Str());
				TextureRef * texRef = level->loadTexture(globalPath, true);
				Texture * texture = mesh->addNewTexture(texRef);
				if(mapmode == aiTextureMapMode_Clamp){
					texture->setUWrapMode(WRAP_CLAMP);
					texture->setVWrapMode(WRAP_CLAMP);
				}
				
				while(material.getTexturesPassNumber() < 3)
					material.addTexturePass(NULL, TEX_COMBINE_MODULATE, 0);
				
				material.addTexturePass(texture, TEX_COMBINE_MODULATE, uvindex);
			}*/
		}
		
		materials.push_back(material);
	}
	
	for(size_t i = 0; i < scene->mNumMeshes; i++)
	{
		SubMesh subMesh;
		const aiMesh* mesh = scene->mMeshes[i];
		subMesh.set(mesh->mNumVertices, 
			    (Vector3*) mesh->mVertices, 
			    (Vector3*) mesh->mNormals,  
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
		
		auto& indices = subMesh.getIndices();
		indices.resize(mesh->mNumFaces*3);
		for(size_t i = 0; i < mesh->mNumFaces; i++)
		{
			assert(mesh->mFaces[i].mNumIndices == 3);
			const size_t idx = i*3;
			indices[idx] = mesh->mFaces[i].mIndices[0];
			indices[idx+1] = mesh->mFaces[i].mIndices[1];
			indices[idx+2] = mesh->mFaces[i].mIndices[2];
		}
		
		subMesh.setMaterial(materials[mesh->mMaterialIndex]);
		meshes.push_back(std::move(subMesh));
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
		light.brightness = 1.0;
		light.diffuse = Vector3(ailight->mColorDiffuse.r, ailight->mColorDiffuse.g, ailight->mColorDiffuse.b);
		light.specular = Vector3(ailight->mColorSpecular.r, ailight->mColorSpecular.g, ailight->mColorSpecular.b);
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
	traverseAssimpScene(&level, level.getRoot(), scene->mRootNode, meshes, lights, cameras);
	aiReleaseImport(scene);
	
	return true;
}
