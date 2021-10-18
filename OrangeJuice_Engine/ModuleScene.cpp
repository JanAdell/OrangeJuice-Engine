#include "Globals.h"
#include <time.h> 
#include "Application.h"
#include "ModuleScene.h"

#include "Primitive.h"

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
	
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	BasePlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	
	// checking drawing 
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 10.f, 0.f);
	glEnd();
	glLineWidth(5.0f);

	return UPDATE_CONTINUE;
}

void ModuleScene::OnCollision()
{
	

}