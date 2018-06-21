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
	for(unsigned int i = 0; i < 4*4; i++)
		neoMat.entries[i] = *(aiMat[0] + i);
	
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
				//neoChild->updateDataFromMatrix();
				
				traverseAssimpScene(level, neoChild, child, meshes, lights, cameras);
			}
			else if(camera != cameras.end())
			{
				neoChild->addBehavior(std::make_unique<CameraBehavior>(camera->second.second));
				loadMatrix(neoChild->getTransform(), child->mTransformation);
				neoChild->getTransform() *= camera->second.first;
				//neoChild->updateDataFromMatrix();
				
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
		//neoChild->updateDataFromMatrix();

		// Insert children into new parent
		traverseAssimpScene(level, neoChild, child, meshes, lights, cameras);
	}
}


bool LevelLoader::loadLevel(Level& level, const char* file)
{
	// Import scene from the given file!
	const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

	if(!scene)
	{
		std::cerr << "Could not load level: " << aiGetErrorString() << std::endl;
		return false;
	}
	
	assert(!scene->HasTextures() && "No embedded textures please!");
	
	// First, load all meshes into the resource cache
	std::vector<SubMesh> meshes;
	meshes.reserve(scene->mNumMeshes);
	
	for(size_t i = 0; i < scene->mNumMeshes; i++)
	{
		SubMesh subMesh;
		auto& vertices = subMesh.getVertices();
		auto& normals = subMesh.getNormals();
		auto& texcoords = subMesh.getTexCoords();
		
		const aiMesh* mesh = scene->mMeshes[i];
		vertices.resize(mesh->mNumVertices);
		memcpy(vertices.data(), mesh->mVertices, sizeof(float)*3*mesh->mNumVertices);
		
		if(mesh->mNormals != nullptr)
		{
			normals.resize(mesh->mNumVertices);
			memcpy(normals.data(), mesh->mNormals, sizeof(float)*3*mesh->mNumVertices);
		}
		
		if(mesh->mTextureCoords[0] != nullptr)
		{
			texcoords.resize(mesh->mNumVertices);
			memcpy(texcoords.data(), mesh->mTextureCoords[0], sizeof(float)*2*mesh->mNumVertices);
		}
		
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
		
		light.angle = ailight->mAngleOuterCone;
		light.brightness = 1.0;
		light.exponent = ailight->mAngleInnerCone;
		light.diffuse = Vector3(ailight->mColorDiffuse.r, ailight->mColorDiffuse.g, ailight->mColorDiffuse.b);
		light.specular = Vector3(ailight->mColorSpecular.r, ailight->mColorSpecular.g, ailight->mColorSpecular.b);
	}
	
	std::unordered_map<std::string, std::pair<Matrix4x4, CameraBehavior>> cameras;
	cameras.reserve(scene->mNumCameras);
	for(size_t i = 0; i < scene->mNumCameras; i++)
	{
		const auto aicam = scene->mCameras[i];
		std::pair<Matrix4x4, CameraBehavior>& camera = cameras[aicam->mName.C_Str()];
		
		camera.second.fov = AI_RAD_TO_DEG(aicam->mHorizontalFOV);
		camera.second.near = aicam->mClipPlaneNear;
		camera.second.far = aicam->mClipPlaneFar;
		
		aiMatrix4x4 aiViewMatrix;
		aicam->GetCameraMatrix(aiViewMatrix);
		loadMatrix(camera.first, aiViewMatrix);
	}
	
	// Second, load all scene nodes and place them
	// Kick it off!
	traverseAssimpScene(&level, level.getRoot(), scene->mRootNode, meshes, lights, cameras);
	return true;
}
