#include "BombFactory.h"
#include "Game.h"
#include "Bomb.h"


BombFactory::BombFactory()
{
}

Bomb* BombFactory::createBomb(Game &gameInstance, const vector<PowerUp> powerUps, SDL_Rect currCoords)
{
	// default explosion range = 2
	int explosionRange = 2;

	// handle power ups here later

	// remove player's extra height
	currCoords.y += dPlayerHeight;
	currCoords.h = TILE_SIZE;

	// explosion texture positions on sprite sheet
	vector<int> expTextures;

	// screen positions for explosions
	vector<vector<SDL_Rect>> expPositions;

	SDL_Rect explosionPosition;
	explosionPosition.w = TILE_SIZE;
	explosionPosition.h = TILE_SIZE;

	// treat center of explosion individually
	vector<SDL_Rect> centerPiece;
	centerPiece.push_back(currCoords);
	expPositions.push_back(centerPiece);

	// use a for-loop for the remaining bombs - bombSide can be x or y
	for (int bombSide = 0; bombSide < 2; bombSide++)
	{
		// create two vectors for minus and negative sides of axis
		vector<SDL_Rect> decVector;
		vector<SDL_Rect> incVector;

		// create pointer to current position
		int* editPosition = NULL;
		int originalValue;

		// selects which axis is being dealt with
		switch (bombSide)
		{
		case(0) :
			editPosition = &explosionPosition.x;
			explosionPosition.y = currCoords.y;
			originalValue = currCoords.x;
			break;
		case(1) :
			editPosition = &explosionPosition.y;
			explosionPosition.x = currCoords.x;
			originalValue = currCoords.y;
			break;
		}

		// for each position, up until the end of the explosion range
		for (int position = 0; position < explosionRange; position++)
		{
			// ensure that we are at 0,0 (relative to the bomb's coordinates)
			(*editPosition) = originalValue;

			// increase tile position, add to vector
			(*editPosition) += ((position + 1) * TILE_SIZE);
			incVector.push_back(explosionPosition);

			// set to 0,0 again
			(*editPosition) = originalValue;

			// then decrease tile position (opposite side) and add to vector
			(*editPosition) -= ((position + 1) * TILE_SIZE);
			decVector.push_back(explosionPosition);
		}

		// add these groups of SDL_Rects to a vector
		expPositions.push_back(incVector);
		expPositions.push_back(decVector);
	}

	// create bomb at player loc
	Bomb* newBomb = new Bomb(explosionRange, currCoords, SDL_GetTicks(), expPositions);

	// return the created bomb
	return newBomb;
}


BombFactory::~BombFactory()
{
}
