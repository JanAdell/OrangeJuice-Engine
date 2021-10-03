#pragma once

#include <list>
#include <vector>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "GuiManager.h"
#include "Timer.h"

struct HardwareSpecs
{
public:

	SDL_version sdlVersion;
	float systemRam = 0;
	int cpus = 0;
	int cpu_chache = 0;
	bool rdtsc;
	bool mmx;
	bool sse;
	bool sse2;
	bool sse3;
	bool sse41;
	bool sse42;
	bool threeDnow;
	bool avx;
	bool avx2;
	bool altivec;

};

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	GuiManager* gui;


private:

	Timer	ms_timer;
	std::list<Module*> list_modules;

public:

	Timer					ptimer;
	Timer					startupTime;
	Timer					frameTime;
	Timer					lastSecFrameTime;
	Timer					msTime;
	uint					lastFrameMs;
	uint					frameMs;
	uint					frameCount = 0u;
	uint					lastSecFrameCount = 0u;
	uint					prevLastSecFrameCount = 0u;
	uint					framesOnLastUpdate = 0u;
	uint					maxFrames = 60u;
	uint16_t				framerateCap = 0u;
	float					avgFps = 0.0f;
	float					dt;

	std::list<uint>		lastFrames[100];

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void GetFrames(int& frames, float& miliseconds);
	void RequestBrowser(const char*);
	HardwareSpecs systemSpecs;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};