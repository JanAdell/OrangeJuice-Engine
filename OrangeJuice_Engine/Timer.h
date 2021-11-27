#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();

	Uint32 Read();
	float ReadSeconds();

	void Resume();
	void Reset();

private:

	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
	Uint32 resumed_at;
};

#endif //__TIMER_H__