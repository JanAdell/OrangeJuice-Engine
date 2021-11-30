#ifndef _MODULEFILE_H_
#define _MODULEFILE_H_

#include "Module.h"
#include "GameObject.h"

#include <vector>
#include <string>
#include <fstream>

class ModuleFile : public Module
{

public:
	ModuleFile(Application* app, bool start_enabled = true);
	~ModuleFile();

	bool LoadDataFromLibrary(char** data, const char* name, const char* directory, const char* extension) const;
	void SplitPath(const char* fullPath, std::string* path, std::string* extension = nullptr, std::string* file = nullptr);
	void SplitPath(const char* fullPath, std::vector<std::string>* splits);

	std::string ToLowerCase(const char* path);


	std::string GetFile(const char* path);
	std::string GetFileName(const char* path);
	std::string GetFolder(const char* path);
	GameObject* GetParentByID(const int& ID);

public:
	std::vector<GameObject*> importedObjects;

};

#endif