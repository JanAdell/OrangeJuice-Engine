#pragma once
#include "Module.h"
#include "Globals.h"

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
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision();

public:
};
