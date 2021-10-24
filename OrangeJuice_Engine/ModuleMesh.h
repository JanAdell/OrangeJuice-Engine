#ifndef _MODULEMESH_H_
#define _MODULEMESH_H_

#include "Globals.h"
#include "Geometry.h"
#include "Module.h"
#include <vector>

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
	std::vector<Geometry*> geometry;

private:
	float TriangleCenterAxis(const float& p1, const float& p2, const float& p3);
};

#endif //_MODULEMESH_H_ 