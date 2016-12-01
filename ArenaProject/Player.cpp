#include "Player.h"

#define BOMB_COOLDOWN 1000

Player::Player(SDL_Rect startingPos, int controllerIndex) 
	: XboxController(controllerIndex)
{
	_playerIndex = controllerIndex;
	_currCoords = startingPos;
}

void Player::update(SDL_Rect newLoc, int newAnimation)
{
	// checks if player is dead
	if (_deadFlag)
	{
		// cycle dead animations and force function to return
		_currAnim += 1;
		_currFrame = _dead[_currAnim];
		return;
	}

	// update player coordinates
	_prevCoords = _currCoords;
	_currCoords = newLoc;

	// update animation
	_currAnimType = newAnimation;

	// checks if player has moved
	if (_currCoords != _prevCoords)
	{
		// if moved, increase or decrease animation index
		if (_animDec)
		{
			if (_currAnim == 0)
			{
				_animDec = false;
				_currAnim += 1;
			}
			else
			{
				_currAnim -= 1;
			}
		}
		else
		{
			if (_currAnim == 2)
			{
				_animDec = true;
				_currAnim -= 1;
			}
			else
			{
				_currAnim += 1;
			}
		}
	}
	else
	{
		_currAnim = 1;
	}

	// check orientation and select correct animation
	switch (_currAnimType)
	{
	case(-1) :
		_currFrame = _left[_currAnim];
		break;
	case(0) :
		_currFrame = _back[_currAnim];
		break;
	case(2) :
		_currFrame = _front[_currAnim];
		break;
	case(3) :
		_currFrame = _right[_currAnim];
		break;
	default:
		break;
	}

	if (_bombLimit < 2)
	{
		Uint32 currentTicks = SDL_GetTicks();

		for (int timer = 0; timer < _bombTimer.size(); timer++)
		{
			if ( (currentTicks - _bombTimer[timer]) >= BOMB_COOLDOWN)
			{
				_bombLimit += 1;
				_bombTimer.erase(_bombTimer.begin() + timer);
			}
		}
	}
}

SDL_Rect Player::returnPos()
{
	return _currCoords;
}

SDL_Rect Player::currentFrame()
{
	return _currFrame;
}

SDL_Texture* Player::getSprites()
{
	return _spriteSheet;
}

int Player::getAnimType()
{
	return _currAnimType;
}

int Player::getAnimFrame()
{
	return _currAnim;
}

void Player::setSprite(SDL_Texture* spriteSheet)
{
	// set the player's sprite sheet - unique to player
	_spriteSheet = spriteSheet;
	_currAnim = 3;

	// always start at this animation
	_currFrame = _front[0];
}

void Player::addAnim(SDL_Rect newImage, int frameGroup)
{
	// add animations to vectors at beginning of the game
	switch (frameGroup)
	{
	case(-1) :
		_left.push_back(newImage);
		break;
	case(0) :
		_back.push_back(newImage);
		break;
	case(2) :
		_right.push_back(newImage);
		break;
	case(3) :
		_front.push_back(newImage);
		break;
	default:
		_dead.push_back(newImage);
	}
}

void Player::revertPos()
{
	_currCoords = _prevCoords;
}

int Player::bombLimit()
{
	return _bombLimit;
}

void Player::decBomb()
{
	_bombLimit -= 1;
	_bombTimer.push_back(SDL_GetTicks());
}

void Player::kill()
{
	_currAnim = 0;
	_deadFlag = true;
}

bool Player::isDead()
{
	return _deadFlag;
}

vector<PowerUp> Player::returnPowers()
{
	return _powerUps;
}


bool operator!=(SDL_Rect rect1, SDL_Rect rect2)
{
	// allows comparison between two SDL_Rects, checking if not equal
	if ((rect1.x == rect2.x) && (rect1.y == rect2.y))
	{
		return false;
	}

	return true;
}

bool operator==(SDL_Rect rect1, SDL_Rect rect2)
{
	// allows comparison between two SDL_Rects, check if they are equal
	if ((rect1.x == rect2.x) && (rect1.y == rect2.y))
	{
		return true;
	}

	return false;
}
