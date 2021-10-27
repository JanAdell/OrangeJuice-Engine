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

#include "../OrangeJuice_Engine/MathGeoLib/MathGeoLib.h"
#include "../OrangeJuice_Engine/MathGeoLib/MathBuildConfig.h"


ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

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

	App->mesh->LoadFile("../Assets/BakerHouse.fbx");
	

	return ret;
}

update_status ModuleScene::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
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

	

	return UPDATE_CONTINUE;
}


update_status ModuleScene::PostUpdate(float dt)
{
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