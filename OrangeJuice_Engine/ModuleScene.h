#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include "Globals.h"
#include "Geometry.h"
#include "Primitive.h"
#include "Image.h"
#include "GameObject.h"


#include <vector>
#include <string>

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
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(std::string name);
	
	bool SaveScene(char* path);

public:
	std::vector<GameObject*> gameObjects;
	std::vector<int> textures;
	GameObject* gameObjectSelect = nullptr;

	bool isInGame;

private:

	bool debugDraw = false;
	uint idVertex = 0;
	uint idIndex = 0;
};
#endif