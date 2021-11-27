#include "ModuleTime.h"
#include "ModuleScene.h"

ModuleTime::ModuleTime(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	timeScale = 1.0f;
	dt = 0.0f;

	gameClock = 0.0f;
	isPaused = true;
}


ModuleTime::~ModuleTime()
{
}

update_status ModuleTime::PreUpdate(float deltaTime)
{
	if (!isPaused) {
		dt = deltaTime * timeScale;
		gameClock += dt;
	}
	return UPDATE_CONTINUE;
}

bool ModuleTime::CleanUp()
{
	return true;
}

bool ModuleTime::Start()
{
	timer.Stop();
	timer.Reset();

	return true;
}

void ModuleTime::Play()
{
	isPaused = false;
	timer.Start();
}

void ModuleTime::Resume()
{
	timer.Resume();
	isPaused = false;
}

void ModuleTime::Pause()
{
	timer.Stop();
	isPaused = true;
}

void ModuleTime::Stop()
{
	timer.Stop();
	timer.Reset();
	isPaused = true;
	gameClock = 0;
	dt = 0;
}

void ModuleTime::SetTimeScale(float scale)
{
	timeScale = scale;
}

const float ModuleTime::GetGameTimeInSeconds() const
{
	return gameClock;
}

const float ModuleTime::GetGameDeltaTime() const
{
	return dt;
}

const float ModuleTime::GetRealTimeInSeconds()
{
	return timer.ReadSeconds();
}

float* ModuleTime::GetTimeScale()
{
	return &timeScale;
}

const bool ModuleTime::IsPaused() const
{
	return isPaused;
}