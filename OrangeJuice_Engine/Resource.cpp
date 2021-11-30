#include "Resource.h"

Resource::Resource(uint uid, ResourceType type)
{
	uid = 0;
	resourceType = UNKNOWN;
	referenceCount = 0;
}

Resource::~Resource()
{
}

ResourceType Resource::GetResourceType() const
{
	return resourceType;
}

uint Resource::GetUID() const
{
	return uid;
}

const std::string Resource::GetAssetFile() const
{
	return assetsFile;
}

const std::string Resource::GetLibraryFile() const
{
	return libraryFile;
}

uint Resource::GetReferenceCount() const
{
	return referenceCount;
}

void Resource::SetUID(uint uid)
{
	this->uid = uid;
}