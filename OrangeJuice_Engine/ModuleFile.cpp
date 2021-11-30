#include "ModuleFile.h"

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

GameObject* ModuleFile::GetParentByID(const int& id)
{
	for (std::vector<GameObject*>::iterator iter = importedObjects.begin(); iter != importedObjects.end(); ++iter)
	{
		if ((*iter)->UUID == id)
			return *iter;
	}
	return nullptr;
}