#pragma once

#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
//#include "ResourceScene.h"
//#include "ResourceAnimation.h"
#include "ModuleFile.h"

#include <map>

struct ResourceData
{
	std::string name;
	ResourceType type = ResourceType::UNKNOWN;
	std::string assetsFile;
	std::string libraryFile;
};

class ModuleResources : public Module
{

public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	//bool Start()override;
	//update_status Update(float dt)override;
	//bool CleanUp()override;

	uint Find(const char* file_in_assets) const;
	const char* Find(uint UID) const;
	uint ImportFile(const char* new_file_in_assets);
	uint GenerateNewUID();

	const Resource* RequestResource(uint uid) const;
	Resource* RequestResource(uint uid);
	void ReleaseResource(uint uid);

	Resource* GetResource(uint uid);

private:

	Resource* CreateNewResource(ResourceType type);

private:

	std::map<uint, Resource*> resources;
	std::map<uint, ResourceData> resourcesData;
};