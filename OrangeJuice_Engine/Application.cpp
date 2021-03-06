#include "Application.h"
#include <fstream>
#include <iomanip>

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	mesh = new ModuleMesh(this);
	gui = new GuiManager(this);
	time = new ModuleTime(this);
	file = new ModuleFile(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(file);
	AddModule(mesh);

	// Scenes
	AddModule(scene);
	AddModule(gui);
	AddModule(time);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); ++item)
	{

		if (*item != NULL)
		{
			delete* item;
			*item = NULL;
		}
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	is_console = true;

	// Call Init() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->Start();
	}

	MaxFrames(0);

	//Hardware specs from the pc executing the program
	SDL_GetVersion(&systemSpecs.sdlVersion);
	systemSpecs.cpus = SDL_GetCPUCount();
	systemSpecs.systemRam = SDL_GetSystemRAM() / 1000.0f;
	systemSpecs.cpu_chache = SDL_GetCPUCacheLineSize();
	systemSpecs.altivec = SDL_HasAltiVec();
	systemSpecs.rdtsc = SDL_HasRDTSC();
	systemSpecs.mmx = SDL_HasMMX();
	systemSpecs.sse = SDL_HasSSE();
	systemSpecs.sse2 = SDL_HasSSE2();
	systemSpecs.sse3 = SDL_HasSSE3();
	systemSpecs.sse41 = SDL_HasSSE41();
	systemSpecs.sse42 = SDL_HasSSE42();
	systemSpecs.threeDnow = SDL_Has3DNow();
	systemSpecs.avx = SDL_HasAVX();
	
	msTimer.Start();
	Load();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frameCount++;
	lastSecFrameCount++;
	dt = (float)msTimer.Read() / 1000.0f;
	if (dt > maxFrames)
		dt = maxFrames;
	msTimer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (frameCount == frameCount++)
	{
		frameMs = msTime.Read();
		msTime.Start();
	}
	
	if (lastSecFrameTime.Read() > 1000)
	{
		lastSecFrameTime.Start();
		prevLastSecFrameCount = lastSecFrameCount;
		lastSecFrameCount = 0;
	}

	lastFrameMs = msTimer.Read();
	framesOnLastUpdate = prevLastSecFrameCount;

	if (framerateCap > 0 && lastFrameMs < milisecCap)
	{
		SDL_Delay(milisecCap - lastFrameMs);
	}
	lastFrameMs = msTimer.Read();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->PreUpdate(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->Update(dt);
	}
		
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; ++item)
	{
		ret = (*item)->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	is_console = false;

	Save();

	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && ret == true; ++item)
	{
		ret = (*item)->CleanUp();
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::RequestBrowser(const char* url)
{
	ShellExecuteA(GetActiveWindow(), "open", url, NULL, NULL, SW_SHOWNORMAL);
}

std::string Application::NormalizePath(const char* full_path)
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\' || newPath[i] == '//')			
			newPath[i] = '/';	
	}

	return newPath;
}

void Application::Pause(bool wantToPause)
{
	if (wantToPause)
		SetTimeScale(0.f);
	else
		SetTimeScale(1.f);
}

void Application::MaxFrames(int max)
{
	framerateCap = max;
	milisecCap = (framerateCap > 0) ? 1000 / max : 0;
}

void Application::GetFrames(int& frames, float& millisec)
{
	frames = framesOnLastUpdate - 1;
	millisec = frameMs;
}

const float Application::GetTimeScale() const
{
	return timeScale;
}

void Application::SetTimeScale(float scale, int frameNum)
{
	timeScale = scale;
	scaleFrames = frameNum;
}

bool Application::Save()
{
	bool ret = true;
	using jsonf = nlohmann::json;
	jsonf jsonfile;

	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && ret == true; ++item)
	{
		ret = (*item)->Save(jsonfile);
	}
	std::fstream file("Save.json");
	file << jsonfile;
	LOG("Save()");
	return ret;
}

bool Application::Load()
{
	bool ret = true;

	nlohmann::json j;
	std::ifstream ifs("Save.json");
	if (!ifs.is_open())
	{
		LOG("Error to load file", SDL_GetError());
	}
	else
	{
		ifs >> j;

		for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && ret == true; ++item)
		{
			ret = (*item)->Load(j);
		}
		LOG("Load()");
	}
	return ret;
}