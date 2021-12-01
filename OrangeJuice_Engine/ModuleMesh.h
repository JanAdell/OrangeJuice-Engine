#ifndef _MODULEMESH_H_
#define _MODULEMESH_H_

#include "Globals.h"
#include "Geometry.h"
#include "Module.h"
#include "GameObject.h"

#include "glew/include/GL/glew.h"
#include <gl/GL.h>

#include <vector>
#include <string>

class ModuleMesh : public Module
{
public:
	ModuleMesh(Application* app, bool start_enabled = true);
	~ModuleMesh();

	bool Init();
	update_status PostUpdate(float dt)override;
	bool CleanUp();

	bool LoadFile(const char* fileName);
	bool LoadFBXFile(const char* file_name);
	bool LoadTextureFile(const char* file_name);
	void ChangeTex(GameObject* object, const char* file_name);

	std::string GenerateNameFromPath(std::string path);

private:
	float TriangleCenterAxis(const float& p1, const float& p2, const float& p3);
	
	std::string modelName;
};

#endif //_MODULEMESH_H_ 