#include "ModuleFile.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental\filesystem>

ModuleFile::ModuleFile(Application* app, bool start_enabled) : Module(app)
{
}

ModuleFile::~ModuleFile()
{
}

bool ModuleFile::LoadDataFromLibrary(char** data, const char* name, const char* directory, const char* extension) const
{
	std::string path;

#if _DEBUG
	path = "../Library/";
#else
	path = "Library/";
#endif

	path += directory;
	path += '/';
	path += name;
	path += extension;

	//Search file
	std::ifstream file(path, std::ifstream::binary);

	//Get file length
	file.seekg(0, file.end);
	std::streamsize length = file.tellg();
	file.seekg(0, file.beg);

	//Load data to buffer-----------------------------------------
	if (file.good() && file.is_open())
	{
		data[0] = new char[length];
		file.read(data[0], length);
		file.close();
		return true;
	}
	else
	{
		LOG("ERROR: could not load mesh %s", name);
		return false;
	}
}

void ModuleFile::SplitPath(const char* fullPath, std::string* path, std::string* fileExtension, std::string* file)
{
	if (fullPath != nullptr)
	{
		std::string full(fullPath);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (pos_separator < full.length())
				*path = full.substr(0, pos_separator + 1);
			else
				path->clear();
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
				*file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
			else
				*file = full.substr(0, pos_dot);
		}

		if (fileExtension != nullptr)
		{
			if (pos_dot < full.length())
				*fileExtension = full.substr(pos_dot + 1);
			else
				fileExtension->clear();
		}
	}
}

void ModuleFile::SplitPath(const char* fullPath, std::vector<std::string>* splits)
{
	std::string tmp;
	std::string substring;

	tmp = fullPath;
	while (tmp.find("/") != std::string::npos)
	{
		size_t found = tmp.find("/");
		substring = tmp.substr(0, found);
		tmp.erase(tmp.begin(), tmp.begin() + found + 1);
		splits->push_back(substring);
	}

	splits->push_back(tmp);
}

std::string ModuleFile::ToLowerCase(const char* path)
{
	std::string pth = path;
	std::transform(pth.begin(), pth.end(), pth.begin(), [](unsigned char tmp) { return std::tolower(tmp); });
	return pth;
}

bool ModuleFile::Exist(const char* file)
{
	//return PHYSFS_exists(file) != 0;
	return false;
}

std::string ModuleFile::GetFile(const char* path)
{
	std::string file;
	std::string filePath;
	std::string fileExtension;
	SplitPath(path, &filePath, &file, &fileExtension);
	return file + "." + fileExtension;
}

std::string ModuleFile::GetFileName(const char* path)
{
	std::string file;
	std::string file_path;
	SplitPath(path, &file_path, &file);
	return file;
}

std::string ModuleFile::GetFolder(const char* path)
{
	std::string folder;
	SplitPath(path, &folder);
	return folder;
}

char* ModuleFile::DataValue(char*& cursor, std::string info, int size, char* end)
{
	cursor = strstr(cursor, info.c_str());
	std::string aux;
	cursor += sizeof(char) * info.size();
	char* limit = strstr(cursor, end);

	while (size > 0)
	{
		aux.push_back(*cursor);
		if (cursor == limit)
			break;
		++cursor;
		--size;
	}
	char* value = new char[aux.size()];
	strcpy(value, aux.c_str());
	return value;
}

char* ModuleFile::DataValue(std::string& cursor, std::string info, int size, std::string end)
{
	if (cursor.find(info) == std::string::npos)
		return";";
	int pos = cursor.find(info) + sizeof(char) * (info.size());

	int final_pos = cursor.find(end, pos + sizeof(char) * info.size()) - sizeof(char) * (end.size());

	std::string value = cursor.substr(pos, final_pos);

	std::string a(cursor.substr(pos));
	if (!a.empty())
		cursor.assign(a);
	char* final_value = new char[value.size()];
	strcpy(final_value, value.c_str());
	return final_value;
}

bool ModuleFile::SaveScene(char* path, std::vector<GameObject*>objects)
{
	bool ret = true;
	FILE* file;
	file = std::fopen(path, "wt");

	if (file == NULL)
	{
		return false;
	}
	for (std::vector<GameObject*>::iterator object = objects.begin(); object != objects.end(); ++object)
	{
		if (*object != nullptr)
		{
			(*object)->SaveMesh(file);
		}
	}
	std::fclose(file);
	return ret;
}

bool ModuleFile::ImportScene(char* path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		file.open(path);
	}
	//save all scene in data
	uint i = 0;
	std::string data;
	while (!file.eof())
	{
		char character;
		file >> character;
		data.push_back(character);
		++i;
	}

	file.close();
	//craete a cursor to read data
	char* cursor = (char*)data.data();
	while (cursor <= cursor + sizeof(char) * data.size())
	{
		cursor = strstr(cursor, "GameObject:");

		if (cursor == NULL)
			break;


		GameObject* new_object = new GameObject();

		//save parent ID
		std::stringstream convertor(DataValue(cursor, "parentID:", 10));
		int parentID;
		convertor >> parentID;

		//save game object data
		char* end_object = strstr(cursor, "//");
		new_object->ImportMesh(cursor, end_object);


		//if don't have parent create new object else search parent
		if (parentID == 0)
		{
			importedObjects.push_back(new_object);
		}
		else
		{
			GameObject* parent_object = GetParentByID(parentID);
			if (parent_object != nullptr)
				parent_object->children.push_back(new_object);
		}
		
	}
	for (std::vector<GameObject*>::iterator iter = importedObjects.begin(); iter != importedObjects.end(); ++iter)
		App->scene->gameObjects.push_back(*iter);

	return true;
}

GameObject* ModuleFile::GetParentByID(const int& id)
{
	for (std::vector<GameObject*>::iterator iter = importedObjects.begin(); iter != importedObjects.end(); ++iter)
	{
		if ((*iter)->UUID == id)
			return *iter;
	}
	return nullptr;
}

void ModuleFile::SaveTextureToDDS(const char* data, uint size, const char* name)
{
	std::string file_path = CreateFolderInLibrary("Textures");
	file_path += (std::experimental::filesystem::path(name).stem().string());
	file_path += ".dds";

	//Write all to new file
	std::ofstream new_file(file_path.c_str(), std::ofstream::binary);

	if (new_file.good())
	{
		new_file.write(data, size);
		new_file.close();
		LOG("Saved texture %s to Library", name);
	}
	else
	{
		LOG("ERROR: Could not save texture to .DDS");
	}
}

bool ModuleFile::SaveMeshToFormat(const char* name, uint num_vert, uint num_ind, const float* vert, uint* ind, const float* normals, uint num_normals, uint num_face_normals, const float* texture_coords, uint num_coords, uint id_coords)
{
	bool ret = true;

	//DATA ORDER: num vertex - num index - vertex - index - has normals - has text coords - normals - text coords - num_normals - num_face_normals - num_coords - id_coords

	bool has_normals = false;
	bool has_text_coords = false;


	//calculate base size
	uint size = sizeof(uint) * 7 + sizeof(float) * num_vert * 3 + sizeof(uint) * num_ind + sizeof(bool) * 2 + 1;

	if (normals)
	{
		has_normals = true;
		size += sizeof(float) * num_vert * 3;
	}
	if (texture_coords)
	{
		has_text_coords = true;
		size += sizeof(float) * num_vert * 2;
	}


	char* data = new char[size];
	char* cursor = data;

	data[size - 1] = '\0';
	uint ranges[7] = { 02, num_vert, num_ind,  num_normals, num_face_normals, num_coords,  id_coords };
	uint size_of = sizeof(ranges);


	//Copy num vert & num ind
	memcpy(cursor, ranges, size_of);
	cursor += size_of;


	//Copy vert
	size_of = sizeof(float) * num_vert * 3;
	memcpy(cursor, vert, size_of);
	cursor += size_of;

	//Copy ind
	size_of = sizeof(uint) * num_ind;
	memcpy(cursor, ind, size_of);
	cursor += size_of;

	//Copy normals and text coords
	bool has[2] = { has_normals, has_text_coords };

	size_of = sizeof(bool);
	memcpy(cursor, &has[0], size_of);
	cursor += size_of;

	if (has_normals)
	{
		size_of = sizeof(float) * num_vert * 3;
		memcpy(cursor, normals, size_of);
		cursor += size_of;
	}

	size_of = sizeof(bool);
	memcpy(cursor, &has[1], size_of);
	cursor += size_of;

	if (has_text_coords)
	{
		size_of = sizeof(float) * num_vert * 2;
		memcpy(cursor, texture_coords, size_of);
		cursor += size_of;
	}

	std::string file_path = CreateFolderInLibrary("Meshes");
	file_path += name;
	file_path += ".oj";

	//Write all to new file
	std::ofstream new_file(file_path.c_str(), std::ofstream::binary);

	if (new_file.good())
	{
		new_file.write(data, size);
		new_file.close();
	}
	else
	{
		LOG("ERROR: Could not save file to .oj");
		ret = false;
	}

	delete[] data;

	return ret;
}

std::string ModuleFile::CreateFolderInLibrary(const char* folder)
{
	std::string path;

#if _DEBUG
	path = "..\\Library";

#else 
	path = ".\\Library";
#endif

	CreateDirectory(path.c_str(), NULL);
	SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);

	path += "\\";
	path += folder;

	CreateDirectory(path.c_str(), NULL);
	SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_HIDDEN);

	path += "\\";

	return path;
}
