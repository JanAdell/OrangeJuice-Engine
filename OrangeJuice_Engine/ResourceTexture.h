#pragma once
#include "Resource.h"
#include "glew/include/GL/glew.h"
#include "json.hpp"
#include <string>

enum Format 
{
	COLOR_INDEX,
	RGB,
	RGBA,
	BGR,
	BGRA,
	LUMINANCE,
	NOTKNOWN
};

class ResourceTexture : public Resource
{
public:
	ResourceTexture(uint id);
	virtual ~ResourceTexture();

	//bool LoadInMemory() override;
	//void Save(nlohmann::json & config) const override;
	//void Load(const nlohmann::json & config) override;
	void SetData(uint textureId, const char* textureName);

	uint GetWidth();
	uint GetHeight();
	uint GetDepth();
	uint GetId();
	Format GetFormat();
	std::string GetTextureName();

public:

	std::string textureName;
	uint width;
	uint height;
	uint depth;
	uint mips;
	uint bytes;
	uint id;
	Format format;

};

