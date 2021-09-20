#include "Globals.h"
#include <time.h> 
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleAudio.h"

#include "Primitive.h"



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	App->audio->PlayMusic("audio/democide.ogg");
	Mix_VolumeMusic(volume);
	lvlFx = App->audio->LoadFx("audio/lvl.ogg");
	gameWinFx = App->audio->LoadFx("audio/win.ogg");
	deadFx = App->audio->LoadFx("audio/dead.ogg");
	
	
	//1 = path			 //2 = path limit
	//3 = flag			 //4 = slider 
	//5 = obstacle		 //6 = trap
	//7 = invisible road //8 = win condition
	//9 = change lvl	 //10 = ramp
	//11 = big wall		 //12 = end ramp 
	//13 = clued path
	
	
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	
	

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	PaintingAndManaging();
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision()
{
	

}


int ModuleSceneIntro::Size(int* vec)
{
	int count = 0;
	for (int i = 0; vec[i] <= 15 && vec[i] >= 1; ++i)
	{
		count++;
	}
	return count;
}

void ModuleSceneIntro::PaintingAndManaging()
{

}



