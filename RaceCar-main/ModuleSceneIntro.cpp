#include "Globals.h"
#include <time.h> 
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleAudio.h"

#include "Primitive.h"
#include "PhysBody3D.h"


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
	
	sensor_tricky.Size(30, 1, 15); //Yellow sensor that moves around, lvl8
	sensor_tricky.color = Yellow;
	pb_tricky->SetPos(2010, 3, 210);
	pb_tricky->GetTransform(&sensor_tricky.transform);
	pb_tricky->SetAsSensor(true);
	pb_tricky->collision_listeners.add(this);

	//1 = path			 //2 = path limit
	//3 = flag			 //4 = slider 
	//5 = obstacle		 //6 = trap
	//7 = invisible road //8 = win condition
	//9 = change lvl	 //10 = ramp
	//11 = big wall		 //12 = end ramp 
	//13 = clued path
	
	int circuit0[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,2,2,2,
		2,2,2,1,2,2,2,
		2,2,2,10,2,2,2,
		2,2,2,12,2,2,2,
		2,2,2,1,2,2,2,
		2,2,2,1,2,2,2,
		2,2,2,10,2,2,2,
		2,2,2,9,2,2,2,
		2,2,2,2,2,2,2,
	};

	int circuit1[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,2,2,2,
		2,2,1,1,1,2,2,
		2,2,10,2,1,2,2,
		2,2,12,2,10,2,2,
		2,2,1,2,4,2,2,
		11,2,12,2,10,2,2,
		2,2,1,1,1,2,2,
		2,2,2,9,2,2,2,
		2,2,2,2,2,2,2,
	};

	int circuit2[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,2,2,2,
		2,2,1,1,1,2,2,
		2,2,1,1,1,2,2,
		11,2,13,2,6,2,2,
		2,2,1,2,1,2,2,
		2,2,1,2,1,2,2,
		2,2,1,1,1,2,2,
		2,2,2,9,2,2,2,
		2,2,2,2,2,2,2,
	};

	int circuit3[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,2,2,2,
		2,2,1,13,1,2,2,
		2,2,1,7,1,2,2,
		11,2,6,7,6,2,2,
		2,2,5,7,5,2,2,
		2,2,1,7,1,2,2,
		2,2,1,1,1,2,2,
		2,2,2,9,2,2,2,
		2,2,2,2,2,2,2,
	};

	int circuit4[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,2,2,2,
		2,2,1,1,1,2,2,
		2,7,13,2,13,7,2,
		11,7,6,2,6,7,2,
		2,7,5,2,5,7,2,
		2,2,1,2,1,2,2,
		2,2,1,1,1,2,2,
		2,2,2,9,2,2,2,
		2,2,2,2,2,2,2,
	};

	int circuit5[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,1,1,2,
		2,2,2,2,2,1,2,
		2,1,1,1,1,1,2,
		11,1,2,2,2,2,2,
		2,1,1,1,1,1,2,
		2,2,2,2,2,1,2,
		2,2,1,1,1,1,2,
		2,9,1,2,2,2,2,
		2,2,2,2,2,2,2,
	};

	int circuit6[70]
	{
		2,2,2,9,2,2,2,
		2,2,2,1,2,2,2,
		2,2,2,6,2,2,2,
		2,2,2,6,2,2,2,
		11,2,2,6,2,2,2,
		2,2,2,5,2,2,2,
		2,2,2,6,2,2,2,
		2,2,2,6,2,2,2,
		2,2,2,6,2,2,2,
		2,2,2,2,2,2,2,
	};

	int circuit7[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,2,2,2,
		2,7,7,1,2,2,2,
		2,7,2,10,2,2,2,
		11,1,1,1,2,2,2,
		2,5,2,2,2,2,2,
		2,1,1,1,1,1,2,
		2,2,2,7,2,10,2,
		2,9,1,1,1,5,2,
		2,2,2,2,2,2,2,
	};

	int circuit8[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,15,15,2,
		2,15,15,15,15,15,2,
		2,15,15,15,15,15,2,
		11,15,15,15,15,15,2,
		2,15,15,15,15,15,2,
		2,15,15,15,15,15,2,
		2,15,15,15,15,15,2,
		2,15,15,15,15,15,2,
		2,2,2,2,2,2,2,
	};

	int circuit9[70]
	{
		2,2,2,2,2,2,2,
		2,2,2,1,2,2,2,
		2,2,2,14,7,7,2,
		2,2,2,6,6,1,2,
		11,2,2,2,2,1,2,
		2,1,1,1,2,10,2,
		2,10,2,5,4,4,2,
		2,12,2,1,1,2,2,
		2,8,2,2,2,2,2,
		2,2,2,2,2,2,2,
	};

	//load circuit, only for 7-column circuits
	for (int i = 0; i < 10; ++i)
	{
		if (i == 0) LoadCircuit(circuit, circuit0, i);
		else if (i == 1) LoadCircuit(circuit, circuit1, i);
		else if (i == 2) LoadCircuit(circuit, circuit2, i);
		else if (i == 3) LoadCircuit(circuit, circuit3, i);
		else if (i == 4) LoadCircuit(circuit, circuit4, i);
		else if (i == 5) LoadCircuit(circuit, circuit5, i);
		else if (i == 6) LoadCircuit(circuit, circuit6, i);
		else if (i == 7) LoadCircuit(circuit, circuit7, i);
		else if (i == 8) LoadCircuit(circuit, circuit8, i);
		else if (i == 9) LoadCircuit(circuit, circuit9, i);

	}

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	lvltime.Start();
	srand(time(NULL));

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

	// player will recieve a clue after 30s in lvl
	if (lvltime.Read() / 1000 >= 30)
	{
	}

	//player restarts after some time


	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
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
	Cube* floor_cube = new Cube(5000.0f, 0.0f, 800.0f);
	floor_cube->color = Green;
	floor_cube->Render();
	sensor_tricky.Render();

	if (pb_cubes.Count() != 0 && s_cubes.Count() != 0 && s_cubes.Count() == pb_cubes.Count())
	{
		for (int i = 0; i < s_cubes.Count(); i++)
		{
			pb_cubes[i]->GetTransform(&s_cubes[i].transform);

			//only paints necessary cubes
			if (pb_cubes[i]->painting == true)
			{
				s_cubes[i].Render();
			}

			//If the cube is a slider apply up and down an impulse
			if (pb_cubes[i]->sliders == true)
			{
				if (move)
				{
					slide.Start();
					move = false;
				}
				if (slide.Read() <= 500)
				{
				
				}
				if (slide.Read() >= 1000 && slide.Read() <= 1500)
				{
				
				}
				if (slide.Read() >= 2000)
				{
					move = true;
				}
			}
			//The cubes that have the clue ability will change color when the game gives the clue
			

		}

	}

	delete floor_cube;
	floor_cube = nullptr;
}

void ModuleSceneIntro::LoadCircuit(int* lvlcircuit, int* circuitx, int poscircuit)
{	
	// distance between circuits
	int desp = poscircuit * 8 * 30;

	//choosing the right circuit
	for (int i = 0; i < Size(circuitx); ++i)
	{
		lvlcircuit[i] = circuitx[i];
	}
	//create map
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 7; i++) {
			CreateFloor(vec3(30, 1, 30), 30 * i + desp, 30 * j, circuit[(7 * j) + i]);
			
			if (circuit[(7 * j) + i] == 15)
			{
				p[saving].x = 30 * i + desp;
				p[saving].y = 30 * j;
				saving++;
			}
		}
	}
	//create sensors
	if (pb_limits.Count() != 0 && s_limits.Count() != 0 && s_limits.Count() == pb_limits.Count())
	{
		for (int i = 0; i < s_limits.Count(); i++)
		{
			pb_limits[i]->SetAsSensor(true);
			pb_limits[i]->collision_listeners.add(this);
		}

		for (int i = 0; i < s_endlvl.Count(); i++)
		{
			pb_endlvl[i]->SetAsSensor(true);
			pb_endlvl[i]->collision_listeners.add(this);
		}
	}
}

