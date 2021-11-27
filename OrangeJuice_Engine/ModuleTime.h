#pragma once

#include "Application.h"
#include "Module.h"
#include "Timer.h"

class ModuleTime : public Module
{
public:
	ModuleTime(Application* app, bool start_enabled = true);
	~ModuleTime();

	bool Start() override;
	update_status PreUpdate(float dt) override;
	bool CleanUp() override;

	void Play();
	void Resume();
	void Pause();
	void Stop();

	const float GetGameTimeInSeconds()const;
	const float GetGameDeltaTime() const;

	const float GetRealTimeInSeconds();
	const bool IsPaused() const;

	float* GetTimeScale();
	void SetTimeScale(float val);

private:

	float timeScale;
	float dt;

	Timer timer;
	float gameClock;
	bool isPaused;
};