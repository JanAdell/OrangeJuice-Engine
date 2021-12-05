#include "Image.h"
#include "Application.h"
#include "ModuleScene.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental\filesystem>
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
	//LoadCheckerTexture();
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

void Image::Save(FILE* file)
{
	fputs("texture_id: ", file);
	fprintf(file, "%i", textureId);
	fputs(";\n", file);
}

void Image::LoadCheckerTexture()
{
	GLubyte checkImage[64][64][4];
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkId);
	glBindTexture(GL_TEXTURE_2D, checkId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, NULL);
}

int Image::GetTextureId()
{
	return textureId;
}

int Image::SetTextureId(int id)
{
	textureId = id;
	return textureId;
}

std::string Image::GetTexturePath()
{
	return pTex;
}