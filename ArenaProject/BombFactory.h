#pragma once
#include "Timer.h"
#include "PowerUp.h"
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

using std::string;
using std::vector;
using std::unordered_map;
using std::to_string;

typedef unordered_map<string, vector<SDL_Rect>> bomb_positions;

enum Explosion_Textures
{
	LEFT_END,
	LEFT_MIDDLE,
	TOP_END,
	TOP_MIDDLE,
	MIDDLE,
	BOTTOM_MIDDLE,
	BOTTOM_END,
	RIGHT_MIDDLE,
	RIGHT_END
};

class Game;

class Bomb;

class BombFactory
{
public:
	BombFactory();
	~BombFactory();

	static const int BOMB_FRAME_DELAY = 500; // delay between each bomb frame

	Bomb* createBomb(Game &gameInstance, const vector<PowerUp> powerUps, const SDL_Rect currCoords);

private:

	bomb_positions* _bombLocsPtr;
};

