#ifndef _MODULEMESH_H_
#define _MODULEMESH_H_

#include "Globals.h"
//#include "glew/include/GL/glew.h"
//#include <gl/GL.h>
#include "Geometry.h"
#include "Module.h"
#include <vector>

struct Mesh_data
{
	uint idIndex = 0; // index in VRAM
	uint numIndex = 0;
	uint* index = nullptr;

	uint idVertex = 0; // unique vertex in VRAM
	uint numVertex = 0;
	float* vertex = nullptr;
};


class ModuleMesh : public Module
{
public:
	ModuleMesh(Application* app, bool start_enabled = true);
	~ModuleMesh();

	bool Init();
	update_status PostUpdate(float dt)override;
	bool CleanUp();

	bool LoadFile(const char* fileName);

public:
	Mesh_data* data;
	std::vector<Geometry*> geometry;
};

#endif //_MODULEMESH_H_ 