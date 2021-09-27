#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new GuiManager(this);


	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(gui);

	// Scenes
	AddModule(scene);

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
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
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