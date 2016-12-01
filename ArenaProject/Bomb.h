#pragma once
#include <SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;

typedef unordered_map<string, vector<SDL_Rect>> bomb_positions;

class Bomb
{
public:
	Bomb(int explosionRange, SDL_Rect bombPos, Uint32 timeCreated, vector<vector<SDL_Rect>> explPositions);
	~Bomb();

	void update(Uint32 gameTime);
	SDL_Rect returnPos();
	int returnRange();
	int returnAnim();
	bool detonateState();
	vector<vector<SDL_Rect>> Bomb::returnExplosions();

	Uint32 bombTicks;

private:
	int _explosionRange;
	int _currentAnim = 0;
	int _prevAnim;
	int _animLoops;
	bool _detonateFlag = false;
	SDL_Rect _bombPos;
	Uint32 _timeSinceAnim;

	vector<vector<SDL_Rect>> _explosionPositions;
};

