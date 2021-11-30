#include "ResourceTexture.h"
#include "glew/include/GL/glew.h"

ResourceTexture::ResourceTexture(uint id) : Resource(uid, ResourceType::TEXTURE)
{
	width = 0;
	height = 0;
	depth = 0;
	mips = 0;
	bytes = 0;
	gpu_id = 0;
	format = NOTKNOWN;
}

ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, &gpu_id);
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

uint ResourceTexture::GetGpuID()
{
	return gpu_id;
}

Format ResourceTexture::GetFormat()
{
	return format;
}
