#pragma once
#include "Resource.h"
#include "json.hpp"

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

	uint GetWidth();
	uint GetHeight();
	uint GetDepth();
	uint GetGpuID();
	Format GetFormat();

public:

	uint width;
	uint height;
	uint depth;
	uint mips;
	uint bytes;
	uint gpu_id;
	Format format;

};

