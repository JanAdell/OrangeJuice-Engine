#pragma once
#include "Module.h"
#include "Globals.h"
#include "Geometry.h"
#include "par_shapes.h"
#include "Primitive.h"

#define MAX_SNAKE 2
#define MAX_CIRCUITSIZE 700

struct PhysBody3D;
struct PhysMotor3D;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	//update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();
	void DrawGeometry();
	void OnCollision();

private:

	uint idVertex = 0;
	uint idIndex = 0;
};
