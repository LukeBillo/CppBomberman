#pragma once
#include <vector>
#include <unordered_map>
#include <SDL.h>
#include "XboxController.h"
#include "PowerUp.h"

using std::vector;
using std::unordered_map;
using std::string;

typedef unordered_map<string, SDL_Rect> sprite_positions;

enum Animation
{
	ANIM_LEFT = -1,
	ANIM_BACK = 0,
	ANIM_FRONT = 2,
	ANIM_RIGHT = 3,
	ANIM_DEAD = 4
};

class Player : virtual public XboxController
{
public:
	/* --------------------------------- */
	/* ----------- Variables ----------- */
	/* --------------------------------- */

	/* --------------------------------- */
	/* -------- End of Variables ------- */
	/* --------------------------------- */

	/* --------------------------------- */
	/* ----------- Functions ----------- */
	/* --------------------------------- */

	Player(SDL_Rect startingPos, int controllerIndex);

	SDL_Rect returnPos();
	SDL_Rect currentFrame();
	SDL_Texture* getSprites();
	int getAnimType();
	int getAnimFrame();

	void update(SDL_Rect newLoc, int newAnimation);
	void setSprite(SDL_Texture* spriteSheet);
	void addAnim(SDL_Rect newImage, int frameGroup);
	void revertPos();
	int bombLimit();
	void decBomb();
	void kill();
	bool isDead();

	vector<PowerUp> returnPowers();

	friend bool operator!=(SDL_Rect rect1, SDL_Rect rect2);
	friend bool operator==(SDL_Rect rect1, SDL_Rect rect2);

	/* --------------------------------- */
	/* -------- End of Functions ------- */
	/* --------------------------------- */

private:
	/* --------------------------------- */
	/* ----------- Variables ----------- */
	/* --------------------------------- */
	int _playerIndex;

	SDL_Texture* _spriteSheet;
	vector<SDL_Rect> _front;
	vector<SDL_Rect> _back;
	vector<SDL_Rect> _left;
	vector<SDL_Rect> _right;
	vector<SDL_Rect> _dead;

	// these coords are in terms of actual pixel coords - not (1,1) for example.
	SDL_Rect _currCoords;
	SDL_Rect _prevCoords;
	SDL_Rect _currFrame;

	int _currAnimType;
	int _currAnim;

	bool _animDec = true;
	bool _deadFlag = false;

	int _bombLimit = 2;
	vector<Uint32> _bombTimer; // this is used when a bomb is placed.

	vector<PowerUp> _powerUps;

	/* --------------------------------- */
	/* -------- End of Variables ------- */
	/* --------------------------------- */

	/* --------------------------------- */
	/* ----------- Functions ----------- */
	/* --------------------------------- */

	/* --------------------------------- */
	/* -------- End of Functions ------- */
	/* --------------------------------- */
};

