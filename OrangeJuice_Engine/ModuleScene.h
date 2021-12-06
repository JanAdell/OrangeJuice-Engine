#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include "Globals.h"
#include "Geometry.h"
#include "Primitive.h"
#include "Image.h"
#include "GameObject.h"
#include "Octree.h"
#include "ModuleCamera3D.h"

#include <vector>
#include <string>

struct PhysBody3D;
struct PhysMotor3D;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Init();
	bool Start();
	void CreateCamera();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	void DeleteTexture(Image* tex);
	bool CleanUp();

	GameObject* CreateGameObject(std::string name);
	
	bool SaveScene(char* path);
	void RemoveSceneContent();

public:
	GameObject* GetGameObjectByName(std::string name);
	std::vector<GameObject*> gameObjects;
	std::vector<Image*> textures;
	GameObject* gameObjectSelect = nullptr;
	Octree* octree;

	bool isInGame;
	bool debugDraw = false;

private:
	
	uint idVertex = 0;
	uint idIndex = 0;
};
#endif