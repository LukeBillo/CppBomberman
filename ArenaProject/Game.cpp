#pragma once
#include "Game.h"

Game::Game()
{
	// if error
	if (initSDL() != 0)
	{
		playing = false;
	}
}

int Game::initSDL()
{
	// if error on init() call
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		printf("SDL failed to initialize video. #%i.", SDL_GetError());

		// error code -1 for video
		return -1;
	}

	// setting window pointer - window centered.
	_window = SDL_CreateWindow("Bomberman Showdown", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (_window == NULL)
	{
		printf("A window could not be created. #%s", SDL_GetError());

		// error code -2 for window
		return -2;
	}

	// set surface pointer to window pointer (surface->window)
	_surface = SDL_GetWindowSurface(_window);

	// create a renderer for blitting textures using hardware acceleration
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

	/*
	int init_flags = IMG_INIT_PNG;
	int init_call = IMG_Init(init_flags);

	if (!init_call)
	{
		printf("Could not load support for .PNG images. Error: %s", IMG_GetError());
		return -3;
	}
	*/


	/*
	/	Creating each player with their
	/	own individual controller.
	/	They are then added to the player group.
	/
	/	STARTING PLAYER POSITIONS
	/	(1,1) for p1
	/	(1, 46) for p2
	/	(30, 1) for p3
	/	(30, 46) for p4
	*/

	for (int i = 0; i < NUM_PLAYERS; i++)
	{
		SDL_Rect start;

		/*
		/	SEE ABOVE COMMENT TO SEE WHERE
		/	THESE NUMBERS ARE FROM.
		*/

		switch (i)
		{
		case(0) :
			start = { TILE_SIZE, TILE_SIZE * 2 - PLAYER_HEIGHT };
				break;
		case(1) :
			start = { TILE_SIZE, SCREEN_HEIGHT - TILE_SIZE - PLAYER_HEIGHT};
				break;
		case(2) :
			start = { SCREEN_WIDTH - 2 * TILE_SIZE, TILE_SIZE * 2 - PLAYER_HEIGHT};
				break;
		default:
			start = { SCREEN_WIDTH - 2 * TILE_SIZE, SCREEN_HEIGHT - TILE_SIZE - PLAYER_HEIGHT};
			break;
		}

		start.w = PLAYER_WIDTH;
		start.h = PLAYER_HEIGHT;

		Player newPlayer(start, i);

		_playersGrp.push_back(newPlayer);
		_playersGrp[i].updateState();

		if ( !( _playersGrp[i].checkConnected() ) && (i == 0) )
		{
			printf("Error! Xbox controller not connected.");
			return -3;
		}
		else if (!(_playersGrp[i].checkConnected()))
		{
			_playersGrp.erase(_playersGrp.begin() + i);
			NUM_PLAYERS = i;
			break;
		}
	}

	/*
	/	loading images from imageFile (found in /images/ folder)
	/	images are in format player_[playerNum].bmp
	/	e.g. player_1.bmp is bomberman player 1 sprite sheet 
	/
	/	If no underscore in .bmp name, it's a game object sprite sheet
	/	e.g. explosions.bmp or main-sheet.bmp
	*/

	if (imageFile)
	{
		int underscLoc;
		const char* charArray;
		int groupID;
		string imageID;

		while (!imageFile.eof())
		{
			std::getline(imageFile, imageID);
			underscLoc = imageID.find("_");

			if (underscLoc != imageID.npos)
			{
				string playerCheck = imageID.substr(underscLoc + 1, underscLoc + 2);
		
				charArray = playerCheck.c_str();
				groupID = atoi(charArray);

				if (NUM_PLAYERS <= groupID)
				{
					continue;
				}
			}
			else
			{
				charArray = imageID.c_str();
				
				switch (charArray[0])
				{
				case('e') :
					groupID = -2;
					break;
				default:
					groupID = -1;
					break;
				}
			}

			int returnVal = loadImage(imageID, groupID);

			// if image loading error
			if (returnVal != 0)
			{
				printf("Image failed to load! #%s", SDL_GetError());
				return -5;
			}
		}
	}

	imageFile.close();

	return 0;
}

int Game::loadImage(string imgName, int imageGroup)
{

	/*
	/	imageList.txt has file names without .png extension, so string concatenation is used
	/	also converted to c_str for IMG_load() function
	*/
	string filePath = "images/" + imgName + ".bmp";
	const char* charPointer = (filePath).c_str();

	// Loading BMP files at the path specified and setting colour key
	SDL_Surface* image = SDL_LoadBMP(charPointer);

	if (SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(image->format, 0x0, 0xFF, 0x0)))
	{
		const char* error = SDL_GetError();
		printf(error);
		return -6;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, image);

	// if error
	if (!image)
	{
		return -1;
	}

	// Find which player it belongs to and insert into player's struct

	SDL_Rect spriteArea;

	switch (imageGroup)
	{
	case(-1) :
	{
		string objName;
		vector<SDL_Rect> bombLocs;

		for (int j = 0; j < (image->h / TILE_SIZE); j++)
		{
			for (int i = 0; i < NUM_OBJECTS; i++)
			{
				spriteArea.h = TILE_SIZE;
				spriteArea.w = TILE_SIZE;
				spriteArea.x = i * TILE_SIZE;
				spriteArea.y = j * TILE_SIZE;
				
				if (j == BLOCKS_ROW)
				{
					objName = object_names[i];
					image_locs.insert({ objName, spriteArea });
				}

				else if (j == BOMB_ROW)
				{
					bombLocs.push_back(spriteArea);
				}

				else
				{
					// power ups later?
				}
			}
		}

		bombImg_locs.insert({ "bomb", bombLocs });

		texture_sheet = *&texture;
		break;
	}
	
	case(-2) :
	{
		// explosions - merge with main sheet or find alt?
		string name;

		for (int explosionNum = 0; explosionNum < NUM_EXPLOSIONS; explosionNum++)
		{
			name = "explosion_" + to_string(explosionNum + 1);
			vector<SDL_Rect> explosionVector;

			for (int x = 0; x < EXPLOSION_COLUMNS; x++)
			{
				spriteArea.h = TILE_SIZE;
				spriteArea.w = TILE_SIZE;
				spriteArea.x = (explosionNum * 5 * TILE_SIZE) + (x * TILE_SIZE);

				if (x == 2)
				{
					for (int y = 0; y < EXPLOSION_ROWS; y++)
					{
						spriteArea.y = y * TILE_SIZE;
						explosionVector.push_back(spriteArea);
					}
				}

				else
				{
					spriteArea.y = TILE_SIZE * 2;
					explosionVector.push_back(spriteArea);
				}
			}

			bombImg_locs.insert({ name, explosionVector });
		}


		_explosions = *&texture;
		break;
	}

	default:	
		/*
		/	i - for three rows of sprites
		/	j - each row has 2 types of animation on it
		/	k - each type of animation has 3 frames of anims
		/	animation "dead" is a special case; has 6x frames of anim
		*/
		SDL_Rect framePos;
		framePos.w = PLAYER_WIDTH;
		framePos.h = PLAYER_HEIGHT;

		// size of one set of animations (3 frames per anim)
		const int ANIM_AREA_WIDTH = PLAYER_WIDTH * 3;

		for (int i = 0; i < 3; i++)
		{
			// 3 frames per animation type
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					framePos.x = (j * ANIM_AREA_WIDTH) + (k * PLAYER_WIDTH);
					framePos.y = (i * PLAYER_HEIGHT);

					// This just gives a unique ID
					int animGroup = (i * i) + (j * j) + (2 * i) - (2 * j);

					_playersGrp[imageGroup].addAnim(framePos, animGroup);
				}
			}
		}

		_playersGrp[imageGroup].setSprite(*&texture);	
		break;
	}

	return 0;
}

void Game::createMap()
{
	// set the images for the "Tile" structs
	Wall.image = &(image_locs["wall"]);
	Stone.image = &(image_locs["stone"]);

	// set the image for grass (doesn't have a tile struct)
	SDL_Rect* grassImg = &image_locs["grass"];

	// divides screen into grid positions of 32 in x and y
	const int ENDPOS_X = (SCREEN_WIDTH / TILE_SIZE) - 1;
	const int ENDPOS_Y = (SCREEN_HEIGHT / TILE_SIZE) - 1;

	// mersenne twister algorithm - random number generator
	std::mt19937_64 randGen(time(NULL));

	// random number between 0-1
	std::uniform_int_distribution<int> range(0, 1);

	SDL_Rect imageLoc;

	imageLoc.h = TILE_SIZE;
	imageLoc.w = TILE_SIZE;

	// iterating through each x,y coordinate
	for (int x = 0; x <= ENDPOS_X; x++)
	{
		for (int y = 0; y <= ENDPOS_Y; y++)
		{
			imageLoc.x = x * TILE_SIZE;
			imageLoc.y = y * TILE_SIZE;

			// generates 0 or 1
			int randNum = range(randGen);
			
			// if it's the screen border, it's a wall
			if ((x == 0) || (y == 0) || (x == ENDPOS_X) || (y == ENDPOS_Y) || ((y % 2 == 0) && (x % 2 == 0)))
			{
				Wall.Locs.push_back(imageLoc);
				renderImage(texture_sheet, *Wall.image, imageLoc);
			}

			// else, if the random number is 1, a stone block is placed
			else
			{
				if (randNum == 1)
				{
					Stone.Locs.push_back(imageLoc);
					renderImage(texture_sheet, *Stone.image, imageLoc);
				}

				// if the random number is 0, it's a grass block
				else
				{
					renderImage(texture_sheet, *grassImg, imageLoc);
				}
			}
		}
	}

	const int SCREEN_START = 0;

	// the code below keeps each player's spawn area clear of blocks
	// this ensures that the player can move and place a bomb.
	for (int i = -2; i <= 2; i++)
	{
		switch (i)
		{
		case(-2) :
			imageLoc.x = (ENDPOS_X + i) * TILE_SIZE;
			imageLoc.y = (SCREEN_START + 1) * TILE_SIZE;
			renderImage(texture_sheet, *grassImg, imageLoc);
			destroyTile(imageLoc);

			imageLoc.y = (ENDPOS_Y - 1) * TILE_SIZE;
			renderImage(texture_sheet, *grassImg, imageLoc);
			destroyTile(imageLoc);
			break;
		case(-1) :
			imageLoc.x = (ENDPOS_X + i) * TILE_SIZE;
			for (int j = 1; j <= 2; j++)
			{
				imageLoc.y = (SCREEN_START + j) * TILE_SIZE;
				renderImage(texture_sheet, *grassImg, imageLoc);
				destroyTile(imageLoc);

				imageLoc.y = (ENDPOS_Y - j) * TILE_SIZE;
				renderImage(texture_sheet, *grassImg, imageLoc);
				destroyTile(imageLoc);

			}
			break;
		case(1):
			imageLoc.x = (SCREEN_START + i) * TILE_SIZE;
			for (int j = 1; j <= 2; j++)
			{
				imageLoc.y = (SCREEN_START + j) * TILE_SIZE;
				renderImage(texture_sheet, *grassImg, imageLoc);
				destroyTile(imageLoc);

				imageLoc.y = (ENDPOS_Y - j) * TILE_SIZE;
				renderImage(texture_sheet, *grassImg, imageLoc);
				destroyTile(imageLoc);

			}
			break;
		case(2):
			imageLoc.x = (SCREEN_START + i) * TILE_SIZE;
			imageLoc.y = (SCREEN_START + 1) * TILE_SIZE;
			renderImage(texture_sheet, *grassImg, imageLoc);
			destroyTile(imageLoc);

			imageLoc.y = (ENDPOS_Y - 1) * TILE_SIZE;
			renderImage(texture_sheet, *grassImg, imageLoc);
			destroyTile(imageLoc);

			break;
		case(0) :
			break;
		}
	}

	/*
	/  Player spawn locations:
	/  positions (1, 1), (1, 2) and (2, 1) = grass (Top-left corner)
	/  positions (1, 45), (1, 46), (2, 46) (Top-right corner)
	/  positions (30, 1), (30, 2), (29, 1) (Bottom-left corner)
	/  positions (30, 46), (30, 45), (29, 46)
	/  
	/  These are the positions which were cleared above.
	*/
}


void Game::destroyTile(SDL_Rect imageLoc)
{
	// search vector for imageLoc
	int index = vectorSearch(Stone.Locs, imageLoc);

	// if it was found...
	if (index != -1)
	{
		// erase that rect
		Stone.Locs.erase(Stone.Locs.begin() + index);
	}

}

int Game::play()
	{
		// checks that the game should be playing
		if (!playing)
		{
			return -1;
		}

		// create game event queue and timer
		SDL_Event game_event;
		Timer timer;

		// creating bomb factory - this is called to generate bombs later
		BombFactory bombFactory;
	
		// creating a map of walls, stones and grass
		createMap();

		while (playing)
		{
			// counter for number of dead players
			int deadPlayers = 0;
			
			// update collision tiles every frame - ensures that destroyed tiles are removed
			_collisionTiles = { Stone, Wall };

			// iterate through array of players
			for (int player = 0; player < NUM_PLAYERS; player++)
			{
				/*
				/	Updating the player's state, and then
				/	1. Returns their current position on-screen
				/	2. Returns their animation frame
				/	3. Checks if they've died
				*/

				_playersGrp[player].updateState();
				XINPUT_STATE currState = _playersGrp[player].returnState();
				SDL_Rect playersPos = _playersGrp[player].returnPos();
				int animation = _playersGrp[player].getAnimType();
				bool deadPlayer = _playersGrp[player].isDead();
				
				// Used if the player has died
				int animFrame;

				/*
				/	Used to redraw locations on the screen
				/	As opposed to redrawing the entire screen
				/	every frame.
				*/
				SDL_Rect redrawPoint;
				int pointIndex;

				// check tiles above, below and underneath the player; redraw them.
				for (int i = -1; i <= 1; i++)
				{
					// Setting point to redraw
					redrawPoint = { playersPos.x, (playersPos.y + dPlayerHeight) + (i * TILE_SIZE), TILE_SIZE, TILE_SIZE };

					// Checks if there's a wall there
					pointIndex = vectorSearch(Wall.Locs, redrawPoint);

					// If there's no wall (returns -1)
					if (pointIndex == -1)
					{
						/*
						/	Checks if there's a stone...
						/	Then sets ptr to block image.
						/	(Grass, Stone or Wall.)
						*/

						pointIndex = vectorSearch(Stone.Locs, redrawPoint);
						if (pointIndex != -1)
						{
							_imagePtr = Stone.image;
						}
						else
						{
							_imagePtr = &image_locs["grass"];
						}
					}
					else
					{
						_imagePtr = Wall.image;
					}

					// renders block in position
					renderImage(texture_sheet, *_imagePtr, redrawPoint);
				}

				if (!deadPlayer)
				{
					if (!(_playersGrp[player].checkConnected()))
					{
						/* 
						/	pause game
						/	implement later!

						/	Stop timer,
						/	Prevent players from moving
						/	Maybe need to break out?
						*/
					}

					else
					{
						/*
						/  Handle Xbox Controller input:
						/  Can only travel up or down each frame.
						/  Button presses handled after movement.
						*/

						// if moving along x-axis
						if (currState.Gamepad.sThumbLX)
						{
							if (currState.Gamepad.sThumbLX > 0)
							{
								playersPos.x += (TILE_SIZE);
								animation = ANIM_RIGHT;
							}
							else
							{
								playersPos.x -= (TILE_SIZE);
								animation = ANIM_LEFT;
							}
						}

						// moving along y-axis
						else if (currState.Gamepad.sThumbLY)
						{
							if (currState.Gamepad.sThumbLY > 0)
							{
								playersPos.y -= (TILE_SIZE);
								animation = ANIM_BACK;
							}
							else
							{
								playersPos.y += TILE_SIZE;
								animation = ANIM_FRONT;
							}
						}

						/* ------------------- *
						 * - End of Movement - *
						 * ------------------- */

						// placing down bomb
						if (currState.Gamepad.wButtons && XINPUT_GAMEPAD_A)
						{
							// NOTE: I decided against the threading approach; mutexes would complicate code.

							/*bombThreadData threadData{ this, bombFactory, _playersGrp[i] };
							bombThreadData* dataPtr = &threadData;*/

							// calls the bomb factory, creates a bomb and puts into _bombGroup array.

							if (_playersGrp[player].bombLimit() > 0)
							{
								_bombGroup.push_back(bombFactory.createBomb(*this, _playersGrp[player].returnPowers(), _playersGrp[player].returnPos()));
								_playersGrp[player].decBomb();
							}
							
						}

						// check collision between player-tiles
						if (checkCollision(_collisionTiles, playersPos, player))
						{
							// if there is collision, revert player's position (does not apply controller movement)
							playersPos = _playersGrp[player].returnPos();
						}

						// call player's update
						_playersGrp[player].update(playersPos, animation);
					}

				}
				else
				{
					// if player has died...

					// selects death animation
					animFrame = _playersGrp[player].getAnimFrame();

					// if death animation is less than 5 (only 5 death animations)
					if (animFrame < 5)
					{
						_playersGrp[player].update(playersPos, animation);
					}

					// else, player has finished death animation
					else if (animFrame == 5)
					{
						// moves player off screen, increments death counter
						playersPos = { -32, -32, 32, 32 };
						deadPlayers += 1;
					}
				}

				// creates rect for rendering
				SDL_Rect renderPos{ playersPos.x, playersPos.y, PLAYER_WIDTH, PLAYER_HEIGHT };

				// render player at position
				renderImage(_playersGrp[player].getSprites(), _playersGrp[player].currentFrame(), renderPos);

				// FPS CONTROL
				if (timer.deltaTime() < Timer::MS_PER_FRAME)
				{
					SDL_Delay(Timer::MS_PER_FRAME - timer.deltaTime());

				}
			
			}

			for (int bomb = 0; bomb < _bombGroup.size(); bomb++)
			{
				// select the bomb and return position & explosion range
				Bomb* currentBomb = _bombGroup[bomb];
				SDL_Rect bombPoint = currentBomb->returnPos();
				float bombRange = currentBomb->returnRange();

				// redraw the bomb's location
				SDL_Rect redrawPoint = bombPoint;
				int pointIndex;

				// render grass at the position, then...
				renderImage(texture_sheet, image_locs["grass"], redrawPoint);

				// update the bomb with the current game time
				currentBomb->update(SDL_GetTicks());

				// check if the bomb has detonated, and current animation
				bool detonate = currentBomb->detonateState();
				int bombAnimNum = currentBomb->returnAnim();

				if (detonate)
				{
					// if detonated, iterate through explosion animations (located in an unordered_map)
					string explosionName = "explosion_" + to_string(bombAnimNum);
					SDL_Rect textureRect;
					SDL_Rect explosionRect;

					// fetch explosion rects
					vector<vector<SDL_Rect>> expPositions = _bombGroup[bomb]->returnExplosions();

					// place down explosion animations in order
					for (int bombSide = 0; bombSide < expPositions.size(); bombSide++)
					{
						for (int position = 0; position < expPositions[bombSide].size(); position++)
						{
							explosionRect = expPositions[bombSide][position];

							if (position != (expPositions[bombSide].size() - 1))
							{
								switch (bombSide)
								{
								case(1) :
									textureRect = bombImg_locs[explosionName][RIGHT_MIDDLE];
									break;
								case(2) :
									textureRect = bombImg_locs[explosionName][LEFT_MIDDLE];
									break;
								case(3) :
									textureRect = bombImg_locs[explosionName][BOTTOM_MIDDLE];
									break;
								case(4) :
									textureRect = bombImg_locs[explosionName][TOP_MIDDLE];
									break;
								default:
									break;
								}
							}
							else
							{
								switch (bombSide)
								{
								case(0) :
									textureRect = bombImg_locs[explosionName][MIDDLE];
									break;
								case(1) :
									textureRect = bombImg_locs[explosionName][RIGHT_END];
									break;
								case(2) :
									textureRect = bombImg_locs[explosionName][LEFT_END];
									break;
								case(3) :
									textureRect = bombImg_locs[explosionName][BOTTOM_END];
									break;
								case(4) :
									textureRect = bombImg_locs[explosionName][TOP_END];
									break;
								default:
									break;
								}
							}

							int collision = checkCollision(_collisionTiles, explosionRect, -1);

							switch (collision)
							{
							case(WALL_BLOCK) :
								// if the location was a wall
								position = expPositions[bombSide].size();
								break;
							case(STONE_BLOCK) :
								// if the location was a stone brick
								if (bombAnimNum == 5)
								{
									// destroy stone
									destroyTile(explosionRect);
								}
								// jump to end of for-loop
								position = expPositions[bombSide].size();

								// fall-through intentional, renders the explosion!
							case(0) :
								// in the case that nothing was found, renders explosion
								renderImage(_explosions, textureRect, explosionRect);
								break;
							default:
								// if it collides with anything above 0, it's a playerID...
								_playersGrp[collision - 1].kill();
								break;
							}
						}
					}

					// after each animation, redraw the location
					if (bombAnimNum == 5)
					{
						for (int redrawTile_x = -bombRange; redrawTile_x <= bombRange; redrawTile_x++)
						{
							// works in the same way as the player redraw
							_imagePtr = &image_locs["grass"];
							redrawPoint.x = bombPoint.x + (redrawTile_x * TILE_SIZE);
							redrawPoint.y = bombPoint.y;

							if (redrawTile_x == 0)
							{
								for (int redrawTile_y = -bombRange; redrawTile_y <= bombRange; redrawTile_y++)
								{
									_imagePtr = &image_locs["grass"];
									redrawPoint.y = bombPoint.y + (redrawTile_y * TILE_SIZE);

									if (vectorSearch(Wall.Locs, redrawPoint) != -1)
									{
										_imagePtr = Wall.image;
									}
									else if (vectorSearch(Stone.Locs, redrawPoint) != -1)
									{
										_imagePtr = Stone.image;
									}

									renderImage(texture_sheet, *_imagePtr, redrawPoint);
								}
							}

							else
							{

								if (vectorSearch(Wall.Locs, redrawPoint) != -1)
								{
									_imagePtr = Wall.image;
								}
								else if (vectorSearch(Stone.Locs, redrawPoint) != -1)
								{
									_imagePtr = Stone.image;
								}

								renderImage(texture_sheet, *_imagePtr, redrawPoint);
							}
						}

						// and then remove the bomb from memory
						delete &(*currentBomb);
						currentBomb = nullptr;
						_bombGroup.erase(_bombGroup.begin() + bomb);
					}
				}

				else
				{
					renderImage(texture_sheet, bombImg_locs["bomb"][bombAnimNum], bombPoint);
				}

			}

			if (deadPlayers >= NUM_PLAYERS - 1)
			{
				playing = false;
			}

			
			while (SDL_PollEvent(&game_event) != 0)
			{

				if (game_event.type == SDL_QUIT)
				{
					playing = false;
					close();
				}
			}

			SDL_RenderPresent(_renderer);
		}

		return 0;
}


int Game::checkCollision(vector<Tile> &checkTiles, SDL_Rect checkRect, int playerID)
{
	// change rect if we're checking collision for a player
	// because players are taller (32x47) than standard blocks (32x32)
	SDL_Rect alteredRect;
	if (playerID >= 0)
	{
		// + 1 so that they intersect with blocks
		alteredRect = { checkRect.x + 1, checkRect.y + dPlayerHeight + 1, checkRect.w - 2, checkRect.h - dPlayerHeight - 2 };
	}
	else
	{
		alteredRect = { checkRect.x, checkRect.y,  checkRect.w, checkRect.h };
	}
	

	const SDL_Rect* ptr_checkRect = &alteredRect;
	SDL_bool collision;
	
	for (unsigned int i = 0; i < checkTiles.size(); i++)
	{
		for (unsigned int j = 0; j < checkTiles[i].Locs.size(); j++)
		{
			const SDL_Rect* ptr_TileRect = &checkTiles[i].Locs[j];

			// check if the rectangles intersect (bool)
			collision = SDL_HasIntersection(ptr_TileRect, ptr_checkRect);
			
			switch (collision)
			{
			case(true) :
				// if it's a stone block or this is a player
				if ((i == 1) || (playerID >= 0))
				{
					return -1;
				}
				else
				{
					// else it's not a player, and it's a stone block
					return -2;
				}
			case(false) :
				// if no collision, next
				continue;
			}
		}
	}

	// check if players collide with one another
	SDL_Rect playerRect;

	for (int player = 0; player < NUM_PLAYERS; player++)
	{
		const SDL_Rect* ptr_playerRect;

		if (player == playerID)
		{
			// ensures player can't collide with themself
			continue;
		}

		else
		{
			// else, checks collision with other players, but ignores extra-head height
			playerRect = _playersGrp[player].returnPos();
			playerRect = { playerRect.x + 1, playerRect.y + dPlayerHeight + 1, playerRect.w - 2, playerRect.h - dPlayerHeight - 2 };
			ptr_playerRect = &playerRect;

			switch (SDL_HasIntersection(ptr_playerRect, ptr_checkRect))
			{
			case(true) :
				return (player + 1);
			case(false) :
				continue;
			}	
		}
	}

	return 0;
}

inline void Game::renderImage(SDL_Texture* imageSurface, SDL_Rect clipLoc, SDL_Rect renderLoc)
{
	SDL_Rect* clipRectPtr = &clipLoc;
	SDL_Rect* renderLocPtr = &renderLoc;
	SDL_RenderCopy(_renderer, imageSurface, clipRectPtr, renderLocPtr);
}


void Game::close()
{
	// free all surface pointers and then exit SDL

	/*SDL_FreeSurface(_imagePtr);
	_imagePtr = NULL;*/

	SDL_FreeSurface(_surface);
	_surface = NULL;

	SDL_DestroyWindow(_window);
	_window = NULL;

	// unloads SDL_image libraries
	IMG_Quit();

	SDL_Quit();
}

Game::~Game()
{
	// destroying object
	printf("Exiting game...");
	MainMenu::MainMenu();
}
