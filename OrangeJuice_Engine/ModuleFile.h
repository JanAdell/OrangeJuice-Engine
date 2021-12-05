#ifndef _MODULEFILE_H_
#define _MODULEFILE_H_

#include "Module.h"
#include "GameObject.h"
#include "PhysFS/include/physfs.h"
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
	bool Exist(const char* file);

	std::string GetFile(const char* path);
	std::string GetFileName(const char* path);
	std::string GetFolder(const char* path);
	GameObject* GetParentByID(const int& ID);

	void SaveTextureToDDS(const char* data, uint size, const char* name);

	bool SaveMeshToFormat(const char* name, uint num_vert, uint num_ind, const float* vert, uint* ind, const float* normals, uint num_normals, uint num_face_normals, const float* texture_coords, uint num_coords, uint id_coords);

	std::string CreateFolderInLibrary(const char* folder);

	
	bool SaveScene(char* path, std::vector<GameObject*> objects);
	char* DataValue(char*& cursor, std::string info, int size, char* end = ";");
	char* DataValue(std::string& cursor, std::string info, int size, std::string end = ";");

public:
	std::vector<GameObject*> importedObjects;

};

#endif