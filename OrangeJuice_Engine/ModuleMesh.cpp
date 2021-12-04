#include "Application.h"
#include "ModuleMesh.h"
#include <vector>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental\filesystem>
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

#include "GameObject.h"
#include "ModuleScene.h"
#include "Geometry.h"

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
	if(App->gui->debugDraw)
		App->scene->octree->Draw();
	
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
	/*else if (format.compare("png") == 0 || format.compare("dds") == 0 || format.compare("jpg") == 0)
	{
		LOG("FORMAT IS AN IMAGE");
		LoadTextureFile(file_name);
	}*/
	else LOG("File format '.%s' not recognized", format.c_str());

	return ret;
}

bool ModuleMesh::LoadFBXFile(const char* file_name)
{
	bool ret = true;
	
	const aiScene* scene = aiImportFile(App->NormalizePath(file_name).c_str(), aiProcessPreset_TargetRealtime_Quality);

	if (scene != nullptr)
	{
		if (scene->HasMaterials())
		{
			LoadTemporalMaterials(scene);
		}
		if (scene->HasMeshes())
		{
			GameObject* newfbx = new GameObject();
			newfbx->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM);
			// Use scene->mNumMeshes to iterate on scene->mMeshes array
			
			int index_material = 0;
			for (int i = 0; i < scene->mNumMeshes; ++i)
			{
				GameObject* game_object = new GameObject(newfbx);
				std::string g_name = LoadData(scene->mMeshes[i]);
				LoadMeshFromFormat(g_name.c_str(), game_object);
				Geometry* my_geo = dynamic_cast<Geometry*>(game_object->GetComponent(COMPONENT_TYPE::COMPONENT_MESH));
				my_geo->LoadBuffers();
							
				my_geo->transform = dynamic_cast<Transform*>(game_object->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM));
				my_geo->transform->Init(my_geo->vertices[0], my_geo->vertices[1], my_geo->vertices[2]);
				LOG("New mesh created from %s", file_name);
				
				index_material = scene->mMeshes[i]->mMaterialIndex;

				LoadMaterials(scene, game_object, file_name, index_material);
				LOG("New material created from %s", file_name);

				newfbx->children.push_back(game_object);
			}
			App->scene->gameObjects.push_back(newfbx);
		}
		
		aiReleaseImport(scene);
		tmpMaterial.clear();
	}
	else
		LOG("Error loading scene %s", file_name);


	return ret;
}

void ModuleMesh::ImportTextureToDDSFile(const char* file_name) const
{
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, NULL, 0); 
	if (size > 0) {
		data = new ILubyte[size]; 
		if (ilSaveL(IL_DDS, data, size) > 0)
			App->file->SaveTextureToDDS((char*)data, (uint)size, file_name);
		delete[] data;
	}
}

float ModuleMesh::TriangleCenterAxis(const float& p1, const float& p2, const float& p3)
{
	float middle_point = p1 - p2;

	return (middle_point + p3) * 0.5;
}

void ModuleMesh::ChangeTex(GameObject* object, const char* file_name, Image* texture)
{
	if (object->children.empty())
	{
		for (std::vector<Component*>::iterator iter = object->components.begin(); iter != object->components.end(); ++iter)
		{
			if ((*iter)->type == COMPONENT_TYPE::COMPONENT_MATERIAL)
			{
				Image* tex = dynamic_cast<Image*>(*iter);
				if (texture == nullptr)
				{
					tex->textureId = LoadImages(file_name);
					tex->tmpId = tex->textureId;
					tex->pTex.assign(file_name);
					App->scene->textures.push_back(tex);
					
				}
				else
				{
					tex = texture;
				}
				for (std::vector<Component*>::iterator it = object->components.begin(); it != object->components.end(); ++it)
				{
					if ((*it)->type == COMPONENT_TYPE::COMPONENT_MESH)
					{
						Geometry* mesh = dynamic_cast<Geometry*>(*it);
						mesh->texture = tex;
						break;
					}
				}
				break;
			}
		}
	}
	else
	{
		for (std::vector<GameObject*>::iterator iter = object->children.begin(); iter != object->children.end(); ++iter)
			ChangeTex(*iter, file_name, texture);
	}
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

bool ModuleMesh::IsCulling(Geometry* g)
{
	return currentCamera->frustum.Contains(g->GetParentObject()->bbox->aabb);
}

std::string ModuleMesh::LoadData(aiMesh* mesh)
{
	//Create temporal data that we will fill
	uint num_vertices = 0;
	float* vertices = nullptr;
	uint num_indices = 0;
	uint* indices = nullptr;
	uint num_normals = 0;
	float* normals = nullptr;
	uint num_face_normals = 0;
	float* face_normals = nullptr;
	uint num_coords = 0;
	float* uv_coord = nullptr;
	uint id_coords = 0;

	//Vertices	
	num_vertices = mesh->mNumVertices;
	vertices = new float[num_vertices * 3];
	memcpy(vertices, mesh->mVertices, sizeof(float) * num_vertices * 3);
	LOG("New mesh with %d vertices", vertices);

	//Indices
	if (mesh->HasFaces())
	{
		num_indices = mesh->mNumFaces * 3;
		indices = new uint[num_indices * 3];
		for (uint j = 0; j < mesh->mNumFaces; ++j)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
			}
			else
				memcpy(&indices[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(uint));
		}

		//Normals
		if (mesh->HasNormals())
		{
			num_normals = mesh->mNumVertices * 3;
			normals = new float[mesh->mNumVertices * 3];
			memcpy(normals, mesh->mNormals, sizeof(float) * mesh->mNumVertices * 3);
		}

		num_face_normals = num_vertices * 3;
		face_normals = new float[num_face_normals];
		uint j = 0;
		for (uint i = 0; i < num_vertices * 3; i += 9)
		{
			float u[3] = { (vertices[i + 3] - vertices[i + 0]),(vertices[i + 4] - vertices[i + 1]),(vertices[i + 5] - vertices[i + 2]) };
			float v[3] = { (vertices[i + 6] - vertices[i + 3]),(vertices[i + 7] - vertices[i + 4]),(vertices[i + 8] - vertices[i + 5]) };
			face_normals[j] = (vertices[i] + vertices[i + 3] + vertices[i + 6]) / 3;
			face_normals[j + 1] = (vertices[i + 1] + vertices[i + 4] + vertices[i + 7]) / 3;;
			face_normals[j + 2] = (vertices[i + 2] + vertices[i + 5] + vertices[i + 8]) / 3;;
			face_normals[j + 3] = face_normals[j] + (u[1] * v[2] - u[2] * v[1]);
			face_normals[j + 4] = face_normals[j + 1] + (u[2] * v[0] - u[0] * v[2]);
			face_normals[j + 5] = face_normals[j + 2] + (u[0] * v[1] - u[1] * v[0]);
			j += 6;
		}
	}
	//Coordinates
	if (mesh->HasTextureCoords(0))
	{
		num_coords = mesh->mNumVertices * 2;
		uv_coord = new float[num_coords];
		for (uint i = 0; i < mesh->GetNumUVChannels(); ++i)
		{
			for (uint k = 0; k < mesh->mNumVertices; ++k) {
				uv_coord[k * 2] = mesh->mTextureCoords[i][k].x;
				uv_coord[k * 2 + 1] = mesh->mTextureCoords[i][k].y;
			}
		}
	}

	//Generate random name if it doesn't have one
	std::string name = RandomName(mesh);

	//Copying and saving the mesh info into the mesh file on our library
	App->file->SaveMeshToFormat(name.c_str(), mesh->mNumVertices, mesh->mNumFaces * 3, vertices, indices, normals, num_normals, num_face_normals, uv_coord, num_coords, id_coords);

	return name;
}

void ModuleMesh::LoadMeshFromFormat(const char* file_name, GameObject* g_object)
{
	char* data = nullptr;

	//See if the file was correctly created
	if (App->file->LoadDataFromLibrary(&data, file_name, "Meshes", ".oj") == false)
	{
		return;
	}
	//num vertex - num index - vertex - index - has normals - has text coords - normals - text coords - num_normals - num_face_normals - num_coords - id_coords

	//Filling mesh variables
	uint num_vert = 0;
	uint num_ind = 0;
	uint num_normals = 0;
	uint num_face_normals = 0;
	uint num_coords = 0;
	uint id_coords = 0;
	bool b_normals = false;
	bool b_coords = false;
		
	char* cursor = data;
	uint tag[] = { -1 };
	uint size_of = sizeof(uint);
	memcpy(tag, cursor, size_of);

	if (*tag != 2)
	{
		LOG("ERROR: this is not a mesh");
		return;
	}
	cursor += size_of;

	//UINTS
	uint ranges[6];
	size_of = sizeof(ranges);
	memcpy(ranges, cursor, size_of);
	num_vert = ranges[0];
	num_ind = ranges[1];
	num_normals = ranges[2];
	num_face_normals = ranges[3];
	num_coords = ranges[4];
	id_coords = ranges[5];
	cursor += size_of;

	//VERTICES
	float* vert = new float[num_vert * 3];
	size_of = sizeof(float) * num_vert * 3;
	memcpy(vert, cursor, size_of);
	cursor += size_of;

	//INDICES
	unsigned int* ind = new unsigned int[num_ind];
	size_of = sizeof(float) * num_ind;
	memcpy(ind, cursor, size_of);
	cursor += size_of;

	//BOOLS
	bool bools[2];

	float* normals = nullptr;
	float* texture_coord = nullptr;

	//NORMALS
	size_of = sizeof(bool);
	memcpy(&bools[0], cursor, size_of);
	cursor += size_of;
	if (bools[0])
	{
		normals = new float[num_vert * 3];
		size_of = sizeof(float) * num_vert * 3;
		memcpy(normals, cursor, size_of);
		cursor += size_of;
	}

	//COORDS
	size_of = sizeof(bool);
	memcpy(&bools[1], cursor, size_of);
	cursor += size_of;
	if (bools[1])
	{
		texture_coord = new float[num_vert * 2];
		size_of = sizeof(float) * num_vert * 2;
		memcpy(texture_coord, cursor, size_of);
		cursor += size_of;
	}

	LOG("Loaded %s mesh in our format complete", file_name);
	delete[] data;

	//Finally fill the new component with all the variables that we previously saved in our file 
	Geometry* mesh;
	mesh = dynamic_cast<Geometry*>(g_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH));
	mesh->vertices = vert;
	mesh->indices = ind;
	mesh->numVertices = num_vert;
	mesh->numIndices = num_ind;
	mesh->normals = normals;
	mesh->uvCoords = texture_coord;
	mesh->numCoords = num_coords;
	mesh->numFaceNormals = num_face_normals;
	mesh->numCoords = num_coords;
	mesh->idCoords = id_coords;
	mesh->CalculateParentBBox(mesh->parent);

	LOG("Loaded %s mesh successfully", file_name);
}

GLuint ModuleMesh::GetID()
{
	//Gen image
	ILuint img_id = 0;
	ilGenImages(1, &img_id);
	ilBindImage(img_id);
	return img_id;
}

std::string ModuleMesh::RandomName(aiMesh* mesh)
{
	std::string name = mesh->mName.C_Str();
	if (name.empty())
	{
		name = "GameObject";
		name += "_mesh_";
		name += std::to_string(randId);
		randId++;
	}
	return name;
}

void ModuleMesh::LoadTemporalMaterials(const aiScene* scene)
{
	//We get all the materials and we assign them -1, to later save them only the first time
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		std::pair<aiMaterial*, int> pair;
		pair.first = scene->mMaterials[i];
		pair.second = -1;
		tmpMaterial.push_back(pair);
	}
}

GLuint ModuleMesh::LoadImages(const char* p_tex)
{
	ILuint img_id = GetID();

	//load from path
	ilLoadImage(p_tex);

	ILuint devilError1 = ilGetError();
	if (devilError1 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError1));
		return 0;
	}

	// If the image is flipped
	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);



	ILuint devilError2 = ilGetError();
	if (devilError2 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError2));
		return 0;
	}

	//Send texture to GPU
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &img_id);
	glBindTexture(GL_TEXTURE_2D, img_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	ILuint devilError3 = ilGetError();
	if (devilError3 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError3));
		return 0;
	}

	/*tex_dimension[0] = ilGetInteger(IL_IMAGE_WIDTH);
	tex_dimension[1] = ilGetInteger(IL_IMAGE_HEIGHT);*/

	return img_id;
}

void ModuleMesh::LoadTextureFromFormat(const char*, GameObject* g_object)
{

}

void ModuleMesh::LoadMaterials(const aiScene* scene, GameObject* g_object, const char* file_name, int last_mat_ind)
{
	Timer load;
	load.Start();
	int mat_id = 0;
	if (!tmpMaterial.empty())
	{
		//tex->LoadMaterials(scene, file_name, tmp_material, last_mat_ind);
		uint check_id = 0;
		int tmp_id = 0;
		int texture_id = 0;
		std::string p_tex;
		if (scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString text_path;
			tmpMaterial[last_mat_ind].first->GetTexture(aiTextureType_DIFFUSE, 0, &text_path);
			std::string  tex = text_path.C_Str();
			std::string  p_geo = file_name;

			//We change the name of the fbx for the texture name, with this made we have the general path
			if (std::strcmp(p_geo.c_str(), "") == 0)
			{
				while (p_geo.back() != '\\')
				{
					p_geo.pop_back();
				}
			}
			p_geo += tex;
			p_tex = p_geo;
			p_tex = std::experimental::filesystem::path(tex).stem().string().c_str();

			//Look for if the texture has been already loaded
			if (tmpMaterial[last_mat_ind].second == -1)
			{
				texture_id = LoadImages(p_geo.c_str());
				tmp_id = texture_id;
				//Sending texture to our texture folder inside library folder
				ImportTextureToDDSFile(tex.c_str());

				tmpMaterial[last_mat_ind].second = texture_id;
				if (texture_id == 0)
				{
					LOG("Warning: --------Scene missing textures");
				}
			}
			else
			{
				texture_id = tmpMaterial[last_mat_ind].second;
				tmp_id = texture_id;
			}
		}
		else
			LOG("Material could not be loaded");

		Image* tex = dynamic_cast<Image*>(g_object->CreateComponent(COMPONENT_TYPE::COMPONENT_MATERIAL));
		tex->checkId = check_id;
		tex->tmpId = tmp_id;
		tex->textureId = texture_id;
		tex->pTex = p_tex;
		tex->LoadCheckerTexture();
		dynamic_cast<Geometry*>(g_object->GetComponent(COMPONENT_TYPE::COMPONENT_MESH))->texture = tex;
		tex->toDelete;
		LOG("Mesh Loaded in : %i", load.Read());
	}
}

