#include "Application.h"
#include "ModuleMesh.h"
#include <vector>
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

#include "GameObject.h"
#include "ModuleScene.h"

#pragma comment(lib, "DevIL/lib/x86/Release/ILU.lib")
#pragma comment(lib, "DevIL/lib/x86/Release/DevIL.lib")
#pragma comment(lib, "DevIL/lib/x86/Release/ILUT.lib")
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

	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);

	ILuint devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError));
	}

	//Check DevIL version
	else if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION || iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION || ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
	{
		LOG("DevIL version is different!\n");
	}
	else LOG("DevIL succesfully loaded");

	return ret;
}

update_status ModuleMesh::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleMesh::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

bool ModuleMesh::LoadFile(const char* file_name)
{
	bool ret = false;

	std::string p_file = file_name;
	modelName = GenerateNameFromPath(file_name);
	std::string format;

	LOG("Loading file from %s", file_name);

	//We want to get the format so we read the path from the end
	while (p_file.back() != '.')
	{
		format.push_back(p_file.back());
		p_file.pop_back();
	}
	std::transform(format.begin(), format.end(), format.begin(), ::tolower);
	//Since its backwards we reverse it to then compare it with the formats that the engine supports
	std::reverse(format.begin(), format.end());
	if (format.compare("fbx") == 0)
	{
		LOG("FORMAT IS FBX");
		LoadFBXFile(file_name);
	}
	else if (format.compare("png") == 0 || format.compare("dds") == 0 || format.compare("jpg") == 0)
	{
		LOG("FORMAT IS AN IMAGE");
		LoadTextureFile(file_name);
	}
	else LOG("File format '.%s' not recognized", format.c_str());

	return ret;
}

bool ModuleMesh::LoadFBXFile(const char* file_name)
{
	bool ret = true;
	
	const aiScene* scene = aiImportFile(App->NormalizePath(file_name).c_str(), aiProcessPreset_TargetRealtime_Quality);

	if (scene != nullptr && scene->HasMeshes())
	{
		GameObject* parent = new GameObject();
		parent->name = modelName;
		
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			GameObject* game_object = new GameObject(parent);
			Geometry* data = dynamic_cast<Geometry*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH));

			data->LoadData(scene->mMeshes[i]);

			data->transform = dynamic_cast<Transform*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
			data->transform->Init(data->vertices[0], data->vertices[1], data->vertices[2]);
			if (scene->HasMaterials())
			{
				Image* tex = dynamic_cast<Image*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
				tex->LoadCoords(scene->mMeshes[i]);
				tex->LoadMaterials(scene, file_name);
				data->texture = tex;

			}
			parent->children.push_back(game_object);
			LOG("New mesh created from %s", file_name);
		}
		App->scene->gameObjects.push_back(parent);
		aiReleaseImport(scene);

	}
	else
		LOG("Error loading scene %s", file_name);


	return ret;
}

bool ModuleMesh::LoadTextureFile(const char* file_name)
{
	bool ret = true;

	if (App->scene->gameObjectSelect != nullptr)
	{
		ChangeTex(App->scene->gameObjectSelect, file_name);
	}
	else
	{
		Image* tex = new Image(nullptr);
		tex->textureId = tex->LoadImage(file_name);
		App->scene->textures.push_back(tex->textureId);
	}
	return ret;
}

float ModuleMesh::TriangleCenterAxis(const float& p1, const float& p2, const float& p3)
{
	float middle_point = p1 - p2;

	return (middle_point + p3) * 0.5;
}

void ModuleMesh::ChangeTex(GameObject* object, const char* file_name)
{
	if (object->children.empty())
	{
		for (std::vector<Component*>::iterator iter = object->components.begin(); iter != object->components.end(); ++iter)
		{
			if ((*iter)->type == COMPONENT_TYPE::COMPONENT_MATERIAL)
			{
				Image* tex = dynamic_cast<Image*>(*iter);
				tex->textureId = tex->LoadImage(file_name);
				App->scene->textures.push_back(tex->textureId);
			}
		}
	}
	else
	{
		for (std::vector<GameObject*>::iterator iter = object->children.begin(); iter != object->children.end(); ++iter)
			ChangeTex(*iter, file_name);
	}
}

void ModuleMesh::CreateAABB()
{
	bbox.SetNegativeInfinity();
	//bbox.Enclose(vertices, num_vertices);
}

AABB ModuleMesh::GetAABB()
{
	return bbox;
}

std::string ModuleMesh::GenerateNameFromPath(std::string path)
{
	std::string name = "";
	std::string normalizedPath = path;

	//Normalize path
	for (int i = 0; i < path.length(); i++)
	{
		if (normalizedPath[i] == '\\')
			normalizedPath[i] = '/';
	}

	size_t posSlash = normalizedPath.find_last_of("\\/");
	size_t posDot = normalizedPath.find_last_of(".");

	for (int i = posSlash + 1; i < posDot; i++) {
		name += normalizedPath[i];
	}
	return name;
}