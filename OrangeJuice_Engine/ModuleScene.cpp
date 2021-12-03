#include "Globals.h"
#include <time.h> 
#include "Application.h"
#include "ModuleScene.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"
#include "GuiManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleFile.h"

#include "../OrangeJuice_Engine/MathGeoLib/MathGeoLib.h"
#include "../OrangeJuice_Engine/MathGeoLib/MathBuildConfig.h"
#include <fstream>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	isInGame = false;
}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Init()
{
	float3 aux[8] = { float3(-100,-100,-100),float3(-100,-100,100), float3(-100,100,-100), float3(-100,100,100), float3(100,-100,-100), float3(100,-100,100), float3(100,100,-100), float3(100,100,100) };
	AABB first;
	first.SetNegativeInfinity();
	first.Enclose(&aux[0], 8);
	octree = new Octree(first, 2, 4, 1);

	return true;
}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();
	
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	GameObject* camera = CreateGameObject("Camera");
	camera->cam = (Camera*)camera->CreateComponent(COMPONENT_TYPE::COMPONENT_CAMERA);

	App->mesh->LoadFile("../Assets/BakerHouse.fbx");
	
	return ret;
}

update_status ModuleScene::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	octree->Draw();
	for (std::vector<GameObject*>::iterator object = gameObjects.begin(); object != gameObjects.end(); ++object)
	{
		if ((*object)->toDelete)
		{
			gameObjects.erase(object);
			break;
		}
	}

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleScene::Update(float dt)
{
	BasePlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		debugDraw = !debugDraw;
		
		if (debugDraw == true) LOG("debugDraw on, showing normals.");
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (gameObjectSelect != nullptr)
			App->camera->GoAroundGeometry(gameObjectSelect);
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		char* path = "TestScene.oj";
		App->file->SaveScene(path, gameObjects);
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		char* path = "TestScene.oj";
		App->file->ImportScene(path);
	}

	for (std::vector<GameObject*>::iterator iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
		(*iter)->Draw();
	
	
	return UPDATE_CONTINUE;
}


update_status ModuleScene::PostUpdate(float dt)
{
	//gameObjectSelect = nullptr;

	for (std::vector<GameObject*>::iterator object = gameObjects.begin(); object != gameObjects.end(); ++object)
	{
		(*object)->Update();

	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
	
}

bool ModuleScene::CleanUp()
{
	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		if ((*it) != nullptr)
			delete (*it);
		(*it) = nullptr;
	}
	gameObjects.clear();

	return true;
}

GameObject* ModuleScene::CreateGameObject(std::string name)
{
	GameObject* go = new GameObject();
	go->name = name;
	go->transform = (Transform*)go->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM);
	gameObjects.push_back(go);
	return go;
}
/*
bool ModuleScene::SaveScene(char* path)
{
	bool ret = true;
	FILE* file;
	file = std::fopen(path, "wt");

	if (file == NULL)
	{
		SDL_GetError();
		return false;
	}
	for (std::vector<GameObject*>::iterator object = gameObjects.begin(); object != gameObjects.end(); ++object)
	{
		if (*object != nullptr)
		{
			(*object)->SaveMesh(file);
		}
	}
	std::fclose(file);
	return ret;
}*/