#include "Bomb.h"

#define ANIM_PAUSE 2000
#define NUM_BOMB_ANIMS 3
#define NUM_EXPLOSION_ANIMS 9
#define BOMB_LOOPS 5

Bomb::Bomb(int explosionRange, SDL_Rect bombPos, Uint32 timeCreated, vector<vector<SDL_Rect>> explPositions)
{
	_explosionRange = explosionRange;
	_bombPos = bombPos;
	_timeSinceAnim = timeCreated;
	_animLoops = 0;
	_explosionPositions = explPositions;
}

void Bomb::update(Uint32 gameTime)
{
	// calculate time since last animation
	_timeSinceAnim = gameTime - _timeSinceAnim;

	// if amount of time elapsed is greater than or equal to time between animations...
	if (_timeSinceAnim >= ANIM_PAUSE)
	{
		// change animation
		_prevAnim = _currentAnim;

		// check if detonating
		if (_detonateFlag)
		{
			_currentAnim += 1;
		}
		else
		{
			// select correct animation
			switch (_currentAnim)
			{
			case(0) :
				_currentAnim += 1;
			case(1) :
				if (_prevAnim > 1)
				{
					_currentAnim -= 1;
				}
				else
				{
					_currentAnim += 1;
				}
					break;
			case(2) :
				_currentAnim -= 1;
				_animLoops += 1;

				// controls number of animation loops before detonation
				if (_animLoops == BOMB_LOOPS)
				{
					_detonateFlag = true;
					_currentAnim = 1;
				}
			}
		}
	}
}

int Bomb::returnAnim()
{
	return _currentAnim;
}

vector<vector<SDL_Rect>> Bomb::returnExplosions()
{
	return _explosionPositions;
}

bool Bomb::detonateState()
{
	return _detonateFlag;
}

SDL_Rect Bomb::returnPos()
{
	return _bombPos;
}

int Bomb::returnRange()
{
	return _explosionRange;
}

Bomb::~Bomb()
{
}
