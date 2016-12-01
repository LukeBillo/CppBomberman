#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <time.h>
#include <cmath>
#include <Windows.h>
#include "MainMenu.h"
#include "Player.h"
#include "XboxController.h"
#include "Timer.h"
#include "BombFactory.h"
#include "Bomb.h"

//#include <SDL_thread.h>

#define SCREEN_WIDTH 1056
#define SCREEN_HEIGHT 800

#define TILE_SIZE 32
#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 49

// difference between player height and tile size (delta player height)
#define dPlayerHeight (PLAYER_HEIGHT - TILE_SIZE)

#define NUM_OBJECTS 3
#define NUM_POWERUPS 12
#define NUM_EXPLOSIONS 5

#define EXPLOSION_ROWS 5
#define EXPLOSION_COLUMNS 5

#define BLOCKS_ROW 0
#define BOMB_ROW 1


using std::vector;
using std::unordered_map;
using std::string;
using std::ifstream;
using std::to_string;

typedef unordered_map<string, SDL_Rect> sprite_positions;
typedef unordered_map<string, vector<SDL_Rect>> bomb_positions;
typedef unordered_map<string, SDL_Texture*> sprite_sheets;
typedef vector<Player> player_group;

enum collisions
{
	WALL_BLOCK = -1,
	STONE_BLOCK = -2
};

struct Tile
{
	vector<SDL_Rect> Locs;
	SDL_Rect* image;
};

class Game
{
public:

	/* --------------------------------- *
	*  ----------- Variables ----------- *
	*  --------------------------------- */

	// number of game objects - does not include players; these are environment
	// used for recognition of sprite sheet object locations; named each rect using sprite_positions

	const string object_names[NUM_OBJECTS] = { "grass", "stone", "wall" };
	const string explosions[NUM_EXPLOSIONS] = { "explosion_1", "explosion_2", "explosion_3", "explosion_4", "explosion_5" };
	const string powerUps[NUM_POWERUPS]; // FILL IN LATER

	/* --------------------------------- *
	 * -------- End of Variables ------- *
	 * --------------------------------- */

	/* --------------------------------- *
	 * ----------- Functions ----------- *
	 * --------------------------------- */

	Game();
	~Game();
	int play();
	int checkCollision(vector<Tile> &checkTiles, SDL_Rect checkRect, int playerID);
	void renderImage(SDL_Texture *imageSurface, SDL_Rect clipLoc, SDL_Rect renderLoc);

	template<typename Type>
	int vectorSearch(vector<Type> searchVector, Type findValue)
	{
		for (unsigned int i = 0; i < searchVector.size(); i++)
		{
			if (searchVector[i] == findValue)
			{
				return i;
			}
		}

		return -1;
	}

	/* --------------------------------- *
	 * -------- End of Functions ------- *
	 * --------------------------------- */

private:

	/* --------------------------------- *
	 * ------------ Variables ---------- *
	 * --------------------------------- */

	int NUM_PLAYERS = 4;

	SDL_Window* _window = NULL;
	SDL_Surface* _surface = NULL;
	SDL_Rect* _imagePtr = NULL;
	SDL_Renderer* _renderer = NULL;

	ifstream imageFile{"images/imageList.txt"};

	player_group _playersGrp;

	SDL_Texture* texture_sheet;
	SDL_Texture* _explosions;

	sprite_positions image_locs;
	bomb_positions bombImg_locs;
	// SDL_Rect screenRect;

	int _createBombs = 0;
	vector<Bomb*> _bombGroup;

	Tile Wall;
	Tile Stone;

	vector<Tile> _collisionTiles;

	bool playing = true;

	/* --------------------------------- *
	 * -------- End of Variables ------- *
	 * --------------------------------- */


	 /* --------------------------------- *
	  * ----------- Functions ----------- *
	  * --------------------------------- */

	int	initSDL();
	int loadImage(string imgName, int imageGroup);
	//static int threadCall(void* data);
	void createMap();
	void destroyTile(SDL_Rect imageLoc);
	//void manageBombs(BombFactory bombFactory, Player player);
	void close();

	/* --------------------------------- *
	 * -------- End of Functions ------- *
	 * --------------------------------- */

};

