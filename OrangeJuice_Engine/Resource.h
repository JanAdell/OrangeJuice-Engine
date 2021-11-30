#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"

#include <string>
#include "json.hpp"

enum ResourceType
{
	MODEL,
	TEXTURE,
	SCENE,
	MESH,
	MATERIAL,
	//ANIMATION,
	//AUDIO,
	//BONE,
	UNKNOWN
};

class Resource
{
public:
	Resource(uint uid, ResourceType type);
	virtual ~Resource();

	ResourceType GetResourceType() const;
	uint GetUID() const;
	const std::string GetAssetFile() const;
	const std::string GetLibraryFile() const;
	uint GetReferenceCount() const;

	void SetUID(uint uid);

	//virtual uint Save(nlohmann::json& j);
	//virtual void Load(nlohmann::json& j);

public:

	std::string name;
	std::string assetsFile;
	std::string libraryFile;
	int referenceCount;

protected:

	uint uid;
	ResourceType resourceType; 
};

#endif