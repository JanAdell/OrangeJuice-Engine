#include "ModuleResources.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

uint ModuleResources::Find(const char* assets_file) const
{
	return 0;
}

const char* ModuleResources::Find(uint uid) const
{
	return nullptr;
}

uint ModuleResources::ImportFile(const char* new_file_in_assets)
{
	return uint();
}

uint ModuleResources::GenerateNewUID()
{
	return uint();
}

const Resource* ModuleResources::RequestResource(uint uid) const
{
	return nullptr;
}

Resource* ModuleResources::RequestResource(uint uid)
{
	return nullptr;
}

void ModuleResources::ReleaseResource(uint uid)
{
}

Resource* ModuleResources::CreateNewResource(const char* assetsPath, ResourceType type)
{
	return nullptr;
}
