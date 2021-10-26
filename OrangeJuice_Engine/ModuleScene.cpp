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
	
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	App->mesh->LoadFile("../Assets/BakerHouse.fbx");

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

	// Vertical line at the center of the grid to mark (0,0,0)
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 5.f, 0.f);
	glEnd();
	glLineWidth(1.5f);
	//

	BasePlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		debugDraw = !debugDraw;
		
		if (debugDraw == true) LOG("debugDraw on, showing normals.");
	}

	// checking drawing 
	/*glLineWidth(5.0f);
	glBegin(GL_LINES);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 10.f, 0.f);
	glEnd();
	glLineWidth(5.0f);*/

	//Direct mode drawing
	/*glBegin(GL_TRIANGLES);
	//1
	glVertex3f(0.f, 0.f, 2.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(2.f, 0.f, 0.f);
	//2
	glVertex3f(2.f, 0.f, 2.f);
	glVertex3f(0.f, 0.f, 2.f);
	glVertex3f(2.f, 0.f, 0.f);
	//3
	glVertex3f(2.f, 0.f, 2.f);
	glVertex3f(2.f, 0.f, 0.f);
	glVertex3f(2.f, 2.f, 0.f);
	//4
	glVertex3f(2.f, 2.f, 2.f);
	glVertex3f(2.f, 0.f, 2.f);
	glVertex3f(2.f, 2.f, 0.f);
	//5
	glVertex3f(2.f, 0.f, 2.f);
	glVertex3f(2.f, 2.f, 2.f);
	glVertex3f(0.f, 2.f, 2.f);
	//6
	glVertex3f(0.f, 0.f, 2.f);
	glVertex3f(2.f, 0.f, 2.f);
	glVertex3f(0.f, 2.f, 2.f);
	//7
	glVertex3f(2.f, 2.f, 2.f);
	glVertex3f(2.f, 2.f, 0.f);
	glVertex3f(0.f, 2.f, 0.f);
	//8
	glVertex3f(0.f, 2.f, 2.f);
	glVertex3f(2.f, 2.f, 2.f);
	glVertex3f(0.f, 2.f, 0.f);
	//9
	glVertex3f(0.f, 2.f, 2.f);
	glVertex3f(0.f, 2.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	//10
	glVertex3f(0.f, 0.f, 2.f);
	glVertex3f(0.f, 2.f, 2.f);
	glVertex3f(0.f, 0.f, 0.f);
	//11
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 2.f, 0.f);
	glVertex3f(2.f, 2.f, 0.f);
	//12
	glVertex3f(2.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(2.f, 2.f, 0.f);
	glEnd();
	*/

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	DrawGeometry();

	
	
	return UPDATE_CONTINUE;
}

void ModuleScene::OnCollision()
{
	

}

void ModuleScene::DrawGeometry()
{
	for (std::vector<Geometry*>::iterator it = App->mesh->geometry.begin(); it != App->mesh->geometry.end(); it++)
	{
		(*it)->Draw();

		if (debugDraw) (*it)->DebugDraw();
	}
}