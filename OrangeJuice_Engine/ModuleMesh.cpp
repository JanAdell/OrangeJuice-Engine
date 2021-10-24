#include "Application.h"
#include "ModuleMesh.h"
#include <vector>
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleMesh::ModuleMesh(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModuleMesh::~ModuleMesh()
{}

bool ModuleMesh::Init()
{
	bool ret = true;
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

update_status ModuleMesh::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleMesh::CleanUp()
{
	for (std::vector<Geometry*>::iterator it = geometry.begin(); it != geometry.end(); it++)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	aiDetachAllLogStreams();
	return true;
}

bool ModuleMesh::LoadFile(const char* file_name)
{
	bool ret = false;

	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			data = new Mesh_data();
			data->numVertex = scene->mMeshes[i]->mNumVertices;
			data->numIndex = scene->mMeshes[i]->mNumFaces * 3;
			data->vertex = new float[data->numVertex * 3];
			memcpy(data->vertex, scene->mMeshes[i]->mVertices, sizeof(float) * data->numVertex * 3);
			data->index = new uint[data->numIndex * 3];
			LOG("New mesh with %d vertices", data->vertex);
			if (scene->mMeshes[i]->HasFaces())
			{
				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
						memcpy(&data->index[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint));
				}

			}
			Geometry* newGeometry = new Geometry(data->vertex, data->index, scene->mMeshes[i]->mNumVertices, scene->mMeshes[i]->mNumFaces);

			geometry.push_back(newGeometry);
			LOG("New mesh created from %s", file_name);
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", file_name);

	return ret;
}