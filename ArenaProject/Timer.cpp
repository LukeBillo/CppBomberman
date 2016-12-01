#include "Timer.h"

Timer::Timer()
{
	// set starting ticks
	_ticks = SDL_GetTicks();
}

Uint32 Timer::deltaTime()
{
	// find new time
	Uint32 _newTicks = SDL_GetTicks();

	// calculate change in time since previous frame
	Uint32 deltaTicks = _newTicks - _ticks;

	_ticks = _newTicks;

	// return change in time
	return deltaTicks;
}
