#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2
#define MAX_CIRCUITSIZE 700

struct PhysBody3D;
struct PhysMotor3D;

enum FLOOR_TYPE 
{
	TOP_FLOOR,
	BOTTOM_FLOOR,
	WALL,
	EMPTY_FLOOR,
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision();
	void PaintingAndManaging();
	int Size(int* vec);

public:
	

	

	Timer reset;
	Timer lvltime;
	Timer timetrial;
	Timer changing;
	Timer slide;

	bool win = true;
	bool first = true;
	bool move = true;
	bool run = true;
	bool trick = true;
	bool up = false;
	bool down = true;

	int count = 0;
	int lvlFx = 0;
	int gameWinFx = 0;
	int deadFx = 0;
	int saving = 0;
	int randomize = 0;
	int volume = 10;

	vec2 p[38];
	
	int circuit[MAX_CIRCUITSIZE];

};
