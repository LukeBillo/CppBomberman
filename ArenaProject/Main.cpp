#include "Game.h"

int main(int argc, char* argv[])
{
	Game gameObject;
	int returnVal = gameObject.play();

	if (returnVal != 0)
	{
		printf("An error occurred when playing the game. Error: %s", returnVal);
		return -1;
	}

	return 0;
}