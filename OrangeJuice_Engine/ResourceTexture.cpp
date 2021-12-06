#include "ResourceTexture.h"

ResourceTexture::ResourceTexture(uint id) : Resource(uid, ResourceType::TEXTURE)
{
	width = 0;
	height = 0;
	depth = 0;
	mips = 0;
	bytes = 0;
	id = 0;
	format = NOTKNOWN;
}

ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, &id);
}

void ResourceTexture::SetData(uint textureId, const char* textureName)
{
	id = textureId;
	this->textureName = textureName;
}

uint ResourceTexture::GetWidth()
{
	return width;
}

uint ResourceTexture::GetHeight()
{
	return height;
}

uint ResourceTexture::GetDepth()
{
	return depth;
}

uint ResourceTexture::GetId()
{
	return id;
}

Format ResourceTexture::GetFormat()
{
	return format;
}

std::string ResourceTexture::GetTextureName()
{
	return textureName;
}
