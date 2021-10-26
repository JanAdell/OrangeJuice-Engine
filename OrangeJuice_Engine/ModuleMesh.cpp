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

	float* texCoords = nullptr;
	int textID = 0;

	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_Quality);

	if (scene->HasMaterials())
		if (scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{

			aiString textPath;
			scene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &textPath);
			textID = LoadTexture(textPath.C_Str());
		}

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			Geometry* data = new Geometry();
			//Load vertex
			data->numVertices = scene->mMeshes[i]->mNumVertices;
			data->vertices = new float[data->numVertices * 3];
			data->textureID = textID;
			memcpy(data->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * data->numVertices * 3);
			LOG("New mesh with %d vertices", data->vertices);

			if (scene->mMeshes[i]->HasFaces())
			{
				data->numIndices = scene->mMeshes[i]->mNumFaces * 3;
				data->indices = new uint[data->numIndices * 3];
				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
						memcpy(&data->indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint));
				}
				//load normals
				if (scene->mMeshes[i]->HasNormals())
				{
					data->normals = new float[scene->mMeshes[i]->mNumVertices * 3];
					memcpy(data->normals, scene->mMeshes[i]->mNormals, sizeof(float) * scene->mMeshes[i]->mNumVertices * 3);
				}

			}
			if (scene->mMeshes[i]->HasTextureCoords(0))
			{
				//texCoords = new float[scene->mMeshes[i]->mNumVertices * 2];
				data->numCoords = scene->mMeshes[i]->mNumVertices * 2;
				data->uvCoord = new float[data->numCoords];
				for (int k = 0; k < scene->mMeshes[i]->mNumVertices; ++k) {

					//texCoords[k * 2] = scene->mMeshes[i]->mTextureCoords[0][k].x;
					//texCoords[k * 2 + 1] = scene->mMeshes[i]->mTextureCoords[0][k].y;
					data->uvCoord[k * 2] = scene->mMeshes[i]->mTextureCoords[0][k].x;
					data->uvCoord[k * 2 + 1] = scene->mMeshes[i]->mTextureCoords[0][k].y;
					//LOG("Texture coords: %f", texCoords[k]);
				}
			}
			Geometry* geo = new Geometry(data);
			geometry.push_back(geo);
			LOG("New mesh created from %s", file_name);
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", file_name);

	return ret;
}

GLuint ModuleMesh::LoadTexture(const char* p_tex)
{
	ILuint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);

	//path
	std::string text = "../Assets/";
	std::string tex = p_tex;
	std::string res = text + tex;
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
	glGenTextures(1, &imgID);
	glBindTexture(GL_TEXTURE_2D, imgID);
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
	
	return 0;
}


float ModuleMesh::TriangleCenterAxis(const float& p1, const float& p2, const float& p3)
{
	float middle_point = p1 - p2;

	return (middle_point + p3) * 0.5;
}