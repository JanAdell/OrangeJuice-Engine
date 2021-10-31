#include "Image.h"
#include "Application.h"
#include "ModuleScene.h"

#include "ImGui/imgui.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include <vector>

#pragma comment(lib, "DevIL/lib/x86/Release/ILU.lib")
#pragma comment(lib, "DevIL/lib/x86/Release/DevIL.lib")
#pragma comment(lib, "DevIL/lib/x86/Release/ILUT.lib")



Image::Image(GameObject* parent) : Component(parent, COMPONENT_TYPE::COMPONENT_MATERIAL)
{
	
}

Image::~Image()
{
}

void Image::Enable()
{
}

void Image::Update()
{
}

void Image::Disable()
{
	for (std::vector<Image*>::iterator iter = App->scene->textures.begin(); iter != App->scene->textures.end(); ++iter)
	{
		if (*iter = this)
		{
			App->scene->textures.erase(iter);
			break;
		}
	}
}


GLuint Image::LoadImage(const char* p_tex)
{
	ILuint imgID = GetID();

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

	texDimension[0] = ilGetInteger(IL_IMAGE_WIDTH);
	texDimension[1] = ilGetInteger(IL_IMAGE_HEIGHT);

	return imgID;
}

GLuint Image::GetID()
{
	//Gen image
	ILuint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);
	return imgID;
}

bool Image::LoadMaterials(const aiScene* scene, std::string file_name)
{
	if (scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString textPath;
		scene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &textPath);
		std::string  tex = textPath.C_Str();
		std::string  pGeo = file_name;

		while (pGeo.back() != '/')
		{
			pGeo.pop_back();
		}
		pGeo += tex;
		pTex = pGeo;
		textureId = LoadImage(pGeo.c_str());

		for (std::vector<Image*>::iterator iter = App->scene->textures.begin(); iter != App->scene->textures.end(); ++iter)
		{
			if (pGeo == (*iter)->pTex)
			{
				return false;
				break;
			}
		}
	}
	else LOG("No material detected");
	return true;
}

int Image::GetTextureId()
{
	return textureId;
}

std::string Image::GetTexturePath()
{
	return pTex;
}